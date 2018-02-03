#ifndef SERVERS_APP_SERVER_HXX_
# define SERVERS_APP_SERVER_HXX_
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/traits.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <iterator>
#include <limits>
#include "format.hh"         // libcore
#include "log/helpers.hh"    // libcore
#include "types.hh"          // libcore
#include "Application.hh"    // libcounters
#include "AvgTimedValue.hh"  // libcounters
#include "CounterManager.hh" // libcounters
#include "bip/Server.hh"     // libnetwork
#include "http/Request.hh"   // libnetwork
#include "http/Template.hh"  // libnetwork
#include "network_types.hh"  // libnetwork
#include "serializer.hh"     // libserializer
#include "objects/Doc.hh"    // libserializer
#include "app/HtmlOArchive.hh"
#include "app/HttpServer.hh"
#include "app/Server.hh"
#include "param/Handler.hh"
#include "param/Pod.hh"

namespace xtd { namespace network { namespace http { class Response; } } }
namespace xtd {
namespace servers {
namespace app {

using namespace network;

template <typename T>
const char Address<T>::mcs_value[] = "";
template <>
const char Address<network::af_inet>::mcs_value[] = "0.0.0.0";
template <>
const char Address<network::af_unix>::mcs_value[] = "@localhost";

template<typename TReq, typename TRes, typename Domain>
const char Server<TReq, TRes, Domain>::mcs_defaultListenInterface[] = "0.0.0.0";

template<typename TReq, typename TRes, typename Domain>
const string Server<TReq, TRes, Domain>::mcs_defaultStructGenDumpPath = "/tmp/structgen";



template<typename TReq, typename TRes, typename Domain>
Server<TReq, TRes, Domain>::Server(void) :
  bip_net(),
  http_app(true),
  m_bipHost(Address<Domain>::mcs_value),
  m_bipPort(0),
  m_bipNbThread(0),
  m_useCompression(false),
  m_stopperStatus(stopper_status::disabled),
  m_stopperResponse(),
  m_structgenMutex(),
  m_structgenFilename(),
  m_perfCounter(),
  m_assertRTT(new param::POD<bool>(false, "AssertRTT"))
{
  addOption('p', "bip-port", argument::mandatory, requirement::optional,
            "use <port> as bip server port",
            bindNumber(m_bipPort, 0u, uint32_t(std::numeric_limits<uint16_t>::max())));

  addOption('s', "stopper", argument::none, requirement::optional,
            "always reply with first valid response",
            bindValueIfGiven(m_stopperStatus, stopper_status::waitting));

  addOption('g', "structgen-dump", argument::mandatory, requirement::optional,
            "use structgen dump mode",
            bindString(m_structgenFilename));

  bip_net::setReuseAddr(true);
}



template<typename TReq, typename TRes, typename Domain>
Server<TReq, TRes, Domain>::~Server(void)
{
}

/**
 ** @details
 ** 1. [jflt] here getSpecificConfKey seems to be equal to getCommonConfKey
 ** one of them can be erase if we keep only one section in configuration files
 ** if we wanna have different values, the hierachy level must be specified
 ** i.e. Server::getSpecificConfKey()
 ** otherwise, the deepest method is called (the business one)
 */
template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::parseConfig(void)
{
  http_app::parseConfig();

  string l_key;
  string l_common   = getCommonConfKey();
  string l_specific = l_common;

  if (getSpecificConfKey().size() != 0)
  {
    l_specific = format::vargs("%s:%s", l_common, l_specific);
  }

  // bip port
  if (false == isOptionGiven("bip-port"))
    parseBipPort();



  // listen interface
  l_key = format::vargs("%s:listen", l_common);
  readConf(l_key, m_bipHost, string(mcs_defaultListenInterface));

  // use compression
  l_key = format::vargs("%s:useCompression", l_common);
  readConf(l_key,  m_useCompression, false);
  if (m_useCompression)
    bip_net::compressCodec(network::compress_codec::gzip);



  // Set m_bipNbThread value : represents max_accept value read in conf
  m_bipNbThread = m_nbThread;

  // Update m_nbThread : is now the sum of max_accept and the number of threads used by http debug server
  m_nbThread += mcs_httpNbThread;
}



template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::start(void)
{
  http_app::start();
  bip_net::start();
  log::crit("servers.app.bip", "app::bip server started");
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::stop(void)
{
  bip_net::stop();
  log::crit("servers.app.bip", "app::bip server stopped");
  http_app::stop();
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::parseBipPort(void)
{
  string l_key = format::vargs("%s:listenport", getSpecificConfKey());
  readConf(l_key, m_bipPort);
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::checkOptions(void)
{
  http_app::checkOptions();

  if ((m_bipPort == 0) || (m_bipPort > 65535))
    error_nohelp(1, "invalid --bip-port '%d', must have 0 < port < 65536", m_bipPort);
  if (m_bipPort == m_httpPort)
    error_nohelp(1, "invalid server ports : bip and http must be different");
}


/**
 ** @details
 ** 1. on ne s'appuie pas sur la methode de la classe mere
 */
template<typename TReq, typename TRes, typename Domain>
status
Server<TReq, TRes, Domain>::defineProbes(void)
{
  string l_serverDir = "server";
  string l_queryTimeoutLabel = "qry.timeout";

  if (bip_net::m_isPersistent)
    l_queryTimeoutLabel.append(".persistent");

  if ((status::ok != http_app::defineProbes())                                                    ||
      (status::ok != addProbe(bip_app::m_perfCounter,         l_serverDir                      )) ||
      (status::ok != addProbe(bip_net::getReceiveTotal(),     l_serverDir, "qry.total"         )) ||
      (status::ok != addProbe(bip_net::getReceiveError(),     l_serverDir, "qry.error"         )) ||
      (status::ok != addProbe(bip_net::getReceiveEof(),       l_serverDir, "qry.eof"           )) ||
      (status::ok != addProbe(bip_net::getReceiveTimeout(),   l_serverDir, l_queryTimeoutLabel )) ||
      (status::ok != addProbe(bip_net::getReceiveSuccess(),   l_serverDir, "qry.success"       )) ||
      (status::ok != addProbe(bip_net::getReceivedLastTime(), l_serverDir, "qry.lasttime"      )) ||
      (status::ok != addProbe(bip_net::getSendTotal(),        l_serverDir, "rsp.total"         )) ||
      (status::ok != addProbe(bip_net::getSendSuccess(),      l_serverDir, "rsp.success"       )) ||
      (status::ok != addProbe(bip_net::getSendTimeout(),      l_serverDir, "rsp.timeout"       )) ||
      (status::ok != addProbe(bip_net::getSendError(),        l_serverDir, "rsp.error"         )) ||
      (status::ok != addProbe(bip_net::getCnxTotal(),         l_serverDir, "cnx.total"         )) ||
      (status::ok != addProbe(bip_net::getCnxAccepted(),      l_serverDir, "cnx.accepted"      )) ||
      (status::ok != addProbe(bip_net::getCnxRejected(),      l_serverDir, "cnx.rejected"      )) ||
      (status::ok != addProbe(bip_net::getNbCurrentThread(),  l_serverDir, "threads"           )))
    return status::error;
  return status::ok;
}


template<typename TReq, typename TRes, typename Domain>
bool
Server<TReq, TRes, Domain>::isResponseValid(const TRes& /* p_response */)
{
  return true;
}

template<typename TReq, typename TRes, typename Domain>
bool
Server<TReq, TRes, Domain>::isStructgenMode(void) const
{
  return 0 != m_structgenFilename.size();
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::dumpRequest(const TReq& p_req)
{
  namespace bios = boost::iostreams;

  boost::mutex::scoped_lock              l_lock(m_structgenMutex);
  stringstream                           l_ss(stringstream::in | stringstream::out | stringstream::binary);
  bios::filtering_streambuf<bios::input> l_buf;

  if (true == boost::ends_with(m_structgenFilename, "gz"))
    l_buf.push(bios::gzip_compressor());
  l_buf.push(l_ss);

  if (true == boost::contains(m_structgenFilename, "xml"))
    l_ss.str(serializer::as_xml(p_req, false, serializer::option(true, false, false)));
  else
  {
    vector<char> l_data = serializer::as_bin(p_req, false, serializer::option(true, false, false));
    boost::copy(l_data, std::ostream_iterator<char>(l_ss));
  }

  ofstream l_out(m_structgenFilename.c_str(), std::ios_base::app | std::ios_base::out | std::ios_base::binary);
  bios::copy(l_buf, l_out);
  l_out.close();
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::processObjectRequest(uint32_t     p_requestID,
                                                 const TReq&  p_request,
                                                 const bool   p_requestDebug,
                                                 TRes&        p_response,
                                                 bool&        p_responseDebug)
{
  bip_app::m_perfCounter->startChrono(p_requestID);

  if (true == isStructgenMode())
    dumpRequest(p_request);

  switch (m_stopperStatus)
  {
  case stopper_status::enabled:
    p_response = m_stopperResponse;
    break;
  case stopper_status::waitting:
    processRequest(p_requestID,
                   p_request,  p_requestDebug,
                   p_response, p_responseDebug);

    if (true == isResponseValid(p_response))
    {
      m_stopperResponse = p_response;
      m_stopperStatus   = stopper_status::enabled;
    }
    break;
  case stopper_status::disabled:
    processRequest(p_requestID,
                   p_request,  p_requestDebug,
                   p_response, p_responseDebug);

    break;
  }
  bip_app::m_perfCounter->stopChrono(p_requestID);
}



template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::initialize(void)
{
  http_app::initialize();

  // 60000ms = 60s = 1min : timed window size
  bip_app::m_perfCounter.reset(new counters::AvgTimedValue("", m_nbThread, 60000, m_thresholdMs));

  bind("/query_data",
       h(&bip_app::h_query, this),
       f(&bip_app::f_post_exist, this, "xml"));

  bind_public("/query",
              h(&bip_app::h_ihm_query, this),
              "[IHM] génération de requête");

  //! Parameters
  m_params->add("assertRTT", m_assertRTT, true)
    ->listen<bool>(boost::bind(&bip_app::setAssertRTT, this, _2));
}


template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::setAssertRTT(bool p_assert)
{
  bip_app::m_perfCounter->setAssert(p_assert);
}


template<typename TReq, typename TRes, typename Domain>
int
Server<TReq, TRes, Domain>::process(void)
{
  if (status::ok != defineProbes())
    error_nohelp(1, "unable to create exploit probes");

  m_prober->start();
  m_params->initialize();

  try
  {
    // 1.
    http_net::initialize(m_httpHost, m_httpPort, mcs_httpNbThread);
    bip_net::initialize(m_bipHost, m_bipPort, m_bipNbThread);
    // 2.
    http_net::start();
    bip_net::start();
    // 3.
    http_net::join();
    bip_net::join();
  }
  catch (std::exception& l_error)
  {
    error_nohelp(1, "unable to run bip/http server on %s:%d / %s:%d", m_bipHost, m_bipPort, m_httpHost, m_httpPort, l_error.what());
    return 1;
  }

  m_prober->stop();

  return 0;
}

template<typename TReq, typename TRes, typename Domain>
status
Server<TReq, TRes, Domain>::h_query(const uint32_t                p_requestID,
                                    const network::http::Request& p_req,
                                    network::http::Response&      p_res)
{
  string                      l_message;
  string                      l_xmlData;
  TReq                        l_req;
  TRes                        l_res;
  stringstream                l_xmlStream;
  http::XmlTemplate l_tmpl;
  string                      l_xmlResult;
  bool                        l_reqDebug;
  bool                        l_resDebug;

  p_req.getPost("xml", l_xmlData);
  l_xmlStream.str(l_xmlData);

  try {
    serializer::serializer<serializer::mode::xml>::load(l_xmlStream, l_req, l_reqDebug);
  }
  catch (std::exception& l_error)
  {
    l_message = "unable to create request object from given xml, %s : \n [%s] \n";
    l_message = format::vargs(l_message, l_error.what(), l_xmlData);
    return h_error_text(l_message, p_requestID, p_req, p_res);
  }

  // The first m_bipNbThread are used by bip server, so we need to shift requestId
  processObjectRequest(m_bipNbThread + p_requestID, l_req, l_reqDebug, l_res, l_resDebug);

  l_xmlResult = serializer::as_xml(l_res, l_resDebug);
  l_xmlResult = l_xmlResult.substr(l_xmlResult.find_first_of("\n") + 1);
  l_xmlResult = l_xmlResult.substr(l_xmlResult.find_first_of("\n") + 1);

  l_tmpl.setContent(l_xmlResult);
  l_tmpl.setCharset("utf-8");
  return h_gen(l_tmpl, p_requestID, p_req, p_res);
}


template<typename TReq, typename TRes, typename Domain>
status
Server<TReq, TRes, Domain>::h_ihm_query(const uint32_t                p_requestID,
                                        const network::http::Request& p_req,
                                        network::http::Response&      p_res)
{
  stringstream                 l_jsData;
  http::HtmlTemplate l_tmpl(true);

  to_javascript<TReq>(l_jsData);

  l_tmpl
    .setTitle(format::vargs("BIP %s : query generator", m_binName))
    .addCss("/css/codemirror.css")
    .addJs("/js/codemirror.js")
    .addJs("/js/codemirror.xml.js")
    .add("__jsData", l_jsData.str());

  return h_template_file(l_tmpl, m_httpConfigPath + "/1.0/core/tpl/query.tpl", p_requestID, p_req, p_res);
}


}}} //end namespaces


#endif // !SERVERS_APP_SERVER_HXX_
