# include <unistd.h>
# include <ios>
# include <iostream>
# include <fstream>
# include <string>

# include <boost/format.hpp>
# include <boost/algorithm/string.hpp>
# include <boost/filesystem.hpp>
# include <boost/filesystem/convenience.hpp>

# include <json_parser.hpp>      // libcommon
# include <logger.hh>            // libcommon
# include <counters.hh>          // libcounters
# include <http/Request.hh>      // libnetwork
# include <http/Response.hh>     // libnetwork

# include "app/HttpServer.hh"
# include "app/HtmlOArchive.hh"
# include "param/Handler.hh"
# include "param/Visitor.hh"




namespace xtd {
namespace servers {
namespace app {

using namespace network;
using namespace boost;
namespace       bfs = boost::filesystem;
namespace       ba  = boost::algorithm;
namespace       bpt = boost::property_tree;

const char HttpServer::mcs_defaultListenInterface[] = "0.0.0.0";

HttpServer::HttpServer(bool p_isDebug) :
  Application(),
  http_net(),
  m_config(0),
  m_configPath(),
  m_nbThread(0),
  m_timeoutMs(mcs_defaultTimeoutMs),
  m_isModeProbe(false),
  m_isDebug(p_isDebug),
  m_snmpPath("/var/run/xtd/snmp/"),
  m_probeDelay(mcs_defaultProbeDelay),
  m_prober(),
  m_ramCounter(),
  m_perfCounter(),
  m_thresholdMs(mcs_defaultThresholdMs),
  m_httpHost(mcs_defaultListenInterface),
  m_httpPort(0),
  m_httpConfig(),
  m_httpConfigPath("/usr/lib/xtd/www"),
  m_adminDir("/var/run/xtd/snmp/admin"),
  m_actions()
{
  addOption('c', "config-file", argument::mandatory, requirement::mandatory, "use <file> as input config file",                bindFile(m_configPath));
  addOption('w', "http-port",   argument::mandatory, requirement::optional,  "use <port> as http server port",                 bindNumber(m_httpPort));
  addOption('n', "nb-thread",   argument::mandatory, requirement::optional,  "allocates <nbr> threads for incomming requests", bindNumber(m_nbThread));
  addOption('t', "timeout",     argument::mandatory, requirement::optional,  "server timeout in ms",                           bindNumber(m_timeoutMs));

  m_httpConfig.setReuseAddr(true);
  addSignalHandler(SIGUSR2, boost::bind(&HttpServer::handleUSR2, this));
  addSignalHandler(SIGTERM, boost::bind(&HttpServer::handleTERM, this));
}

HttpServer::~HttpServer(void)
{
}

void
HttpServer::handleTERM(void)
{
  logger::crit("servers.app.http", "received 'SIGTERM' signal");
  stop();
}

void
HttpServer::start(void)
{
  m_params->initialize();
  m_prober->start();
  http_net::start();
  logger::crit("servers.app.http", "app::http server started");
}

void
HttpServer::stop(void)
{
  http_net::stop();
  logger::crit("servers.app.http", "app::http server stopped");
  m_prober->stop();
}

void
HttpServer::parseConfig(void)
{
  string   l_key;
  string   l_strValue;
  string   l_common    = getCommonConfKey();
  string   l_specific  = getSpecificConfKey();
  uint32_t l_timeoutMs = 0;;

  // Retrieve configuration file
  try
  {
    if (0 == m_configPath.size())
    {
      error_nohelp(1, "option --config-file is mandatory");
    }

    if ((false == boost::filesystem::is_regular_file(m_configPath)) &&
        (false == boost::filesystem::is_symlink(m_configPath)))
    {
      error_nohelp(1, "invalid --config-file='%s' option, file unreadable", m_configPath);
    }

    m_config.reset(new ConfParser(m_configPath));
  }
  catch (std::exception& l_error)
  {
    error_nohelp(1, "could not load config file '%s' : %s", m_configPath, l_error.what());
  }


  // log level
  if (false == isOptionGiven("e"))
  {
    l_key = str(format("%s:server:log_level") % l_common);
    readConf(l_key, m_logLevel, m_logLevel);
  }

  // snmp path
  l_key      = str(format("%s:server:snmp_dir") % l_common);
  l_strValue = str(format("/var/run/snmp/%s") % l_common);
  readConf(l_key, m_snmpPath, l_strValue);

  // action dir
  l_key      = str(format("%s:server:action_dir") % l_specific);
  l_strValue = str(format("%s/admin") % m_snmpPath);
  readConf(l_key, m_adminDir, l_strValue);


  // refresh delay
  l_key = str(format("%s:server:snmp_refresh_delay_sec") % l_common);
  readConf(l_key, m_probeDelay);

  // assets dir
  l_key = str(format("%s:server:http:assets_dir") % l_common);
  readConf(l_key, m_httpConfigPath, m_httpConfigPath);

  // listen interface
  l_key = str(format("%s:server:http:listen") % l_common);
  readConf(l_key, m_httpHost, string(mcs_defaultListenInterface));

  // threads
  if (false == isOptionGiven("n"))
  {
    l_key = str(format("%s:server:http:threads") % l_common);
    readConf(l_key, m_nbThread);
  }

  l_key = str(format("%s:server:http:threshold_ms") % l_common);
  readConf(l_key, m_thresholdMs, m_thresholdMs);

  // http port : delegate to child class
  if (false == isOptionGiven("http-port"))
    parseHttpPort();

  // http receive timeout
  l_key = str(format("%s:server:http:rcv_timeout_ms") % l_common);
  readConf(l_key, l_timeoutMs, m_httpConfig.getReceiveTimeoutMs());
  m_httpConfig.setReceiveTimeoutMs(l_timeoutMs);

  // http send timeout ms
  l_key = str(format("%s:server:http:send_timeout_ms") % l_common);
  readConf(l_key, l_timeoutMs, m_httpConfig.getSendTimeoutMs());
  m_httpConfig.setSendTimeoutMs(l_timeoutMs);
}

void
HttpServer::parseHttpPort(void)
{
  string l_key = boost::str(boost::format("%s:server:http:port") % getSpecificConfKey());
  readConf(l_key, m_httpPort);
}


void
HttpServer::checkOptions(void)
{
  if ((m_httpPort == 0) || (m_httpPort > 65535))
    error_nohelp(1, "invalid --http-port '%d', must have 0 < port < 65536", m_httpPort);

  if ((m_nbThread == 0) || (m_nbThread > 1000))
    error_nohelp(1, "invalid --nb-thread='%d', must have 0 < nbr <= 1000", m_nbThread);

  if (m_timeoutMs < 100)
    error_nohelp(1, "invalid --timeout='%d', must have 100 <= nbr", m_nbThread);
}

status
HttpServer::addProbe(t_counter p_counter, const string& p_path)
{
  if (status::ok != m_prober->add(p_counter, p_path))
    return status::error;
  return status::ok;
}


status
HttpServer::defineProbes(void)
{
  string l_dir = m_isDebug ? "server_http" : "server";

  if ((status::ok != addProbe(m_ramCounter,                    ""                          )) ||
      (status::ok != addProbe(http_app::m_perfCounter,         l_dir                       )) ||
      (status::ok != addProbe(http_net::getReceiveTotal(),     l_dir,        "qry.total"   )) ||
      (status::ok != addProbe(http_net::getReceiveError(),     l_dir,        "qry.error"   )) ||
      (status::ok != addProbe(http_net::getReceiveTimeout(),   l_dir,        "qry.timeout" )) ||
      (status::ok != addProbe(http_net::getReceiveSuccess(),   l_dir,        "qry.success" )) ||
      (status::ok != addProbe(http_net::getReceivedLastTime(), l_dir,        "qry.lasttime")) ||
      (status::ok != addProbe(http_net::getSendTotal(),        l_dir,        "rsp.total"   )) ||
      (status::ok != addProbe(http_net::getSendSuccess(),      l_dir,        "rsp.success" )) ||
      (status::ok != addProbe(http_net::getSendTimeout(),      l_dir,        "rsp.timeout" )) ||
      (status::ok != addProbe(http_net::getSendError(),        l_dir,        "rsp.error"   )) ||
      (status::ok != addProbe(http_net::getCnxTotal(),         l_dir,        "cnx.total"   )) ||
      (status::ok != addProbe(http_net::getCnxAccepted(),      l_dir,        "cnx.accepted")) ||
      (status::ok != addProbe(http_net::getCnxRejected(),      l_dir,        "cnx.rejected")) ||
      (status::ok != addProbe(http_net::getNbCurrentThread(),  l_dir,        "threads"     )))
    return status::error;
  return status::ok;
}


void
HttpServer::initialize(void)
{
  m_ramCounter.reset(new counters::Value32("ram.usage"));
  // 60000ms = 60s = 1min : timed window size
  http_app::m_perfCounter.reset(new counters::AvgTimedValue("", m_nbThread, 60000, m_thresholdMs));
  m_prober.reset(new counters::CounterManager(m_probeDelay, getSnmpPath()));
  m_params.reset(new param::Handler(m_adminDir));

  // Link admin console contexts
  bind_public("/admin",    h(&HttpServer::h_admin,      this), "administration");
  bind_public("/action",   h(&HttpServer::h_actionList, this), "actions");
  bind_public("/counter*", h(&HttpServer::h_counter,    this), "counters");
  bind_public("/conf",     h(&HttpServer::h_conf,       this), "configuration");
  bind_public("/ident",    h(&HttpServer::h_ident,      this), "identity");
  bind_public("/log",      h(&HttpServer::h_log,        this), "logs");
  bind_public("/index",    h(&HttpServer::h_index,      this), "index");


  bind_dir("/css/images/*", m_httpConfigPath + "/css/images", "images/png",      true);
  bind_dir("/css/*",        m_httpConfigPath + "/css",        "text/css",        true);
  bind_dir("/img/gif/*",    m_httpConfigPath + "/img/gif",    "image/gif",       true);
  bind_dir("/img/png/*",    m_httpConfigPath + "/img/png",    "image/png",       true);
  bind_dir("/js/*",         m_httpConfigPath + "/js",         "text/javascript", true);

  bind_redirect("/", "/index");

  // Add mode probe handler
  bind_any(h(&HttpServer::h_probe, this),
           f(&HttpServer::f_cgi_exist, this, "mode") &&
           f(&HttpServer::f_cgi_equal, this, "mode", "probe"));
}

int
HttpServer::process(void)
{
  if (status::ok != defineProbes())
    error_nohelp(1, "unable to create exploit probes");


  try
  {
    // 1.
    http_net::initialize(m_httpHost, m_httpPort, m_httpConfig, m_nbThread);
    // 2.
    start();
    // 3.
    http_net::join();
  }
  catch (std::exception& l_error)
  {
    error_nohelp(1, "unable to run http server on %s:%d, %s", m_httpHost, m_httpPort, l_error.what());
    return 1;
  }

  m_prober->stop();

  return 0;
}


/**
 ** @details
 ** gestion vip hebex desactivation du service http
 ** et reponse 503 mode probe.
 */
void
HttpServer::handleUSR2(void)
{
  logger::crit("servers.app.http", "Received a 'SIGUSR2' signal : mode PROBE activated", HERE);
  modeProbeActivation();
}

void HttpServer::modeProbeActivation(void)
{
  m_isModeProbe = true;
}

void
HttpServer::bind_action(const string& p_name,
                        const string& p_description,
                        h             p_action)
{
  t_action l_action(new Action(p_name, p_description));

  m_actions.insert(std::pair<string, t_action>(p_name, l_action));
  bind("/action/" + p_name, h(&HttpServer::h_runAction, this, p_name, p_action));
}

status
HttpServer::h_runAction(const string&        p_name,
                        h                    p_action,
                        const uint32_t       p_requestID,
                        const http::Request& p_req,
                        http::Response&      p_res)
{
  status              l_ret    = status::error;
  string              l_status = "FAILED";
  string              l_data;
  http::Json          l_tmpl;
  string              l_text;
  t_actions::iterator c_action;

  c_action = m_actions.find(p_name);

  if (c_action == m_actions.end())
  {
    l_status = "UNKNOWN";
    logger::crit("servers.app.http", "can't find action '%s' into map actions ; action aborted.", p_name, HERE);
  }
  else
  {
    string l_log;
    p_req.getCgi("log", l_log);

    l_ret = p_action(p_requestID, p_req, p_res);
    if (l_ret == status::ok)
    {
      l_status = "DONE";
      p_res.setStatus(http::code::ok);
      c_action->second->m_timestamp = boost::posix_time::microsec_clock::local_time();
      c_action->second->m_log       = l_log;
    }
    logger::crit("servers.app.http", "action '%s' - execution status : %s", p_name, l_status, HERE);
  }

  // Create Json response
  l_tmpl.add("status", p_name, l_status);

  // Export Json to string
  if (status::ok != l_tmpl.resolve(l_text))
    return h_error_text(l_tmpl.getError(), p_requestID, p_req, p_res);

  // Set response
  p_res.addHeader("Content-Type", l_tmpl.getContentType());
  p_res.setData(l_text);
  return l_ret;
}


status
HttpServer::h_actionList(const uint32_t       p_requestID,
                         const http::Request& p_req,
                         http::Response&      p_res)
{
  http::Json l_tmpl;

  for (auto& cc_value : m_actions)
  {
    l_tmpl.add(cc_value.first, "description", cc_value.second->m_description);
    l_tmpl.add(cc_value.first, "log",         cc_value.second->m_log);

    tm     l_tm = to_tm(cc_value.second->m_timestamp);
    time_t l_date = mktime(&l_tm);

    l_tmpl.add(cc_value.first, "timestamp", lexical_cast<uint32_t>(l_date));
  }

  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}

status
HttpServer::h_probe(const uint32_t                /*p_requestID*/,
                    const http::Request& p_req,
                    http::Response&      p_res)
{
  p_res.setVersion(p_req.getVersionStr());
  if (true == m_isModeProbe)
  {
    p_res.setStatus(http::code::service_unavailable);
    p_res.setData(http::Response::statusToMessage(http::code::service_unavailable));
  }
  else
  {
    p_res.setStatus(http::code::ok);
    p_res.setData(http::Response::statusToMessage(http::code::ok));
  }
  return status::ok;
}


/* -------------------------------------------------------------------- */

string
HttpServer::getCommonConfKey(void) const
{
  return m_binName;
}

string
HttpServer::getSpecificConfKey(void) const
{
  return getCommonConfKey();
}

string
HttpServer::getSnmpPath(void) const
{
  return m_snmpPath;
}


status
HttpServer::h_index(const uint32_t       p_requestID,
                    const http::Request& p_req,
                    http::Response&      p_res)
{
  using http::Template;
  using http::HtmlTemplate;

  Handler::t_listof l_list;
  HtmlTemplate      l_tmpl(true);
  Template::t_list  l_links;
  string            l_title = str(format("%s : Handler list") % m_binName);

  getMatchingHandlers(p_req, l_list);

  std::sort(l_list.begin(),
            l_list.end(),
            boost::bind(std::less<string>(),
                        boost::bind(&Handler::m_descr, _2),
                        boost::bind(&Handler::m_descr, _1)));

  BOOST_FOREACH(const Handler& c_handler, l_list)
  {
    string          l_url = c_handler.m_path;
    Template::t_map l_link;

    trim_right_if(l_url, is_any_of("*"));
    l_link["link"] = Template::val(l_url);
    l_link["text"] = Template::val(c_handler.m_descr);
    l_links.push_back(Template::val(l_link));
  }

  l_tmpl
    .setTitle(l_title)
    .add("title", l_title)
    .add("links", l_links);

  return h_template_file(l_tmpl, m_httpConfigPath + "/tpl/index.tpl", p_requestID, p_req, p_res);
}


/**
 ** @details
 ** 1. recuperation des informations vmstats
 ** 3. récupération de tous les compteurs et filtrage
 ** 3. création de la réponse http
 */
status
HttpServer::h_ident(const uint32_t       p_requestID,
                    const http::Request& p_req,
                    http::Response&      p_res)
{
  http::Json l_tmpl;
  string               l_key;
  string               l_value;

  enum class state { key, value, ignore };
  state l_state = state::ignore;

  for (auto c_char : m_rcsid)
  {
    if (c_char == '$')
    {
      if (l_state == state::ignore)
        l_state = state::key;
      else
      {
        l_tmpl.add("", boost::trim_copy(l_key), boost::trim_copy(l_value));
        l_key.clear();
        l_value.clear();
        l_state = state::ignore;
      }
      continue;
    }

    if ((c_char == ':') && (l_state == state::key))
      l_state = state::value;
    else if (l_state == state::key)
      l_key += c_char;
    else if (l_state == state::value)
      l_value += c_char;
  }
  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}

/**
 ** @details
 ** 1. recuperation des informations vmstats
 ** 3. récupération de tous les compteurs et filtrage
 ** 3. création de la réponse http
 */
status
HttpServer::h_counter(const uint32_t       /* p_requestID */,
                      const http::Request&    p_req,
                      http::Response&         p_res)
{
  uint32_t     l_vmUsage;
  uint32_t     l_residentSet;
  stringstream l_responseData;

  // 1.
  process_mem_usage(l_vmUsage, l_residentSet);
  (*m_ramCounter) = l_residentSet;

  // 2.
  bpt::ptree l_elements;
  bpt::ptree l_root;
  string     l_subPath = p_req.getPath();

  ba::replace_all(l_subPath, "/", "!");
  ba::trim_if(l_subPath, ba::is_any_of("!"));
  bpt::ptree::path_type l_subPathType(l_subPath, '!');

  // Gets json tree representation of counters from prober
  m_prober->toJson(l_elements);

  try
  {
    l_root.put_child("counter", l_elements);
    bpt::ptree l_child = l_root.get_child(l_subPathType);
    write_json(l_responseData, l_child);
    p_res.setData(l_responseData.str());
    p_res.addHeader("Content-Type", "application/json");
    p_res.setStatus(http::code::ok);
  }
  catch (bpt::file_parser_error&)
  {
    string           l_name = l_subPath.substr(l_subPath.find_last_of("!")+1);
    bpt::ptree::path_type l_namePathType(l_name, '!');
    bpt::ptree            l_node;

    l_node.put<string>(l_namePathType, l_root.get<string>(l_subPathType));
    write_json(l_responseData, l_node);
    p_res.setData(l_responseData.str());
    p_res.addHeader("Content-Type", "application/json");
    p_res.setStatus(http::code::ok);
  }
  catch (bpt::ptree_bad_path&)
  {
    l_responseData << p_req.getPath() << " not found" << endl;
    p_res.setData(l_responseData.str());
    p_res.setStatus(http::code::bad_request);
  }

  return status::ok;
}

status
HttpServer::h_admin(const uint32_t       p_requestID,
                    const http::Request& p_req,
                    http::Response&      p_res)
{
  string l_providedLog;
  string l_log = "";

  // Verify integrity of given cgi parameters
  for (auto& c_param : p_req.getCgis())
  {
    if (c_param.first == "log")
      l_log = c_param.second;
    else
    {
      if (false == m_params->verify(c_param.first, c_param.second))
      {
        string l_msg = str(format("Parameter '%s' from cgi can't be setted to value '%s'") %c_param.first %c_param.second);
        logger::crit("servers.app.http", "h_admin: %s", l_msg, HERE);
        return h_error_text(l_msg, p_requestID, p_req, p_res);
      }
    }
  }

  //
  if(!l_log.empty() && p_req.getCgis().size()==1)
  {
    string l_msg = "Parameter 'log' is alone, parameter change skipped";
    logger::crit("servers.app.http", "h_admin: %s", l_msg, HERE);
    return h_error_text(l_msg, p_requestID, p_req, p_res);
  }
  else
  {
    stringstream ss;

    ss << "Actions applied : ";
    // Apply
    BOOST_FOREACH(const http::Request::t_param_map::value_type& c_param, p_req.getCgis())
    {
      if(c_param.first != "log")
      {
        if (false == m_params->fromStr(c_param.first, c_param.second, l_log))
        {
          string l_msg = str(format("unable to set parameter '%s' to value '%s' despite verification !!!")
                             % c_param.first
                             % c_param.second);
          logger::crit("servers.app.http", "h_admin: %s", l_msg, HERE);
          return h_error_text(l_msg, p_requestID, p_req, p_res);
        }

        ss << " | " << c_param.first << " : " << c_param.second;
      }
    }

    // Trace action
    logger::info("servers.app.http", "h_admin: update action executed : %s", ss.str(), HERE);
  }

  http::Json l_tmpl;
  param::JsonVisitor   l_json(l_tmpl);

  l_json.writeExtraInfo("query", p_req.getQS());

  m_params->accept(l_json);
  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}

status
HttpServer::h_conf(const uint32_t       p_requestID,
                   const http::Request& p_req,
                   http::Response&      p_res)
{
  http::Json l_tmpl;

  for (auto& cc_value : m_config->getValueMap())
  {
    string l_s = boost::replace_all_copy(cc_value.first, ":", "!");
    l_tmpl.add("", l_s, cc_value.second);
  }

  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}


status
HttpServer::h_log(const uint32_t       p_requestID,
                  const http::Request& p_req,
                  http::Response&      p_res)
{
  http::Json l_tmpl;
  logger&    l_logger = logger::get();

  const auto&          l_cgis = p_req.getCgis();

  if (0 != l_cgis.size())
  {
    for (const auto& c_param : p_req.getCgis())
    {
      if (false == l_logger.isValidStringLevel(c_param.second))
        continue;
      if (c_param.first == "all")
        l_logger.clearAll(l_logger.fromString(c_param.second));
      else if (c_param.first == "default")
        l_logger.setLevel("", l_logger.fromString(c_param.second));
      else
        l_logger.updateLevels(c_param.first, l_logger.fromString(c_param.second));
    }
    return h_redirect("/log", p_requestID, p_req, p_res);
  }

  for (const auto& cc_value : l_logger.getLevels())
  {
    if (cc_value.first == "")
      l_tmpl.add("", "default", l_logger.stringOf(cc_value.second));
    else
      l_tmpl.add("modules", cc_value.first, l_logger.stringOf(cc_value.second));
  }

  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}

void
HttpServer::process_mem_usage(uint32_t& p_vmUsage,
                              uint32_t& p_residentSet)
{
  p_vmUsage     = 0;
  p_residentSet = 0;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat", std::ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
              >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
              >> utime >> stime >> cutime >> cstime >> priority >> nice
              >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

  stat_stream.close();

  long page_size_kb = ::sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  p_vmUsage     = vsize / 1024.0;
  p_residentSet = rss * page_size_kb;
}

}}} //end namspaces
