// IWYU pragma: private, include "http/Server.hh"
#ifndef NETWORK_HTTP_SERVER_HXX_
# define NETWORK_HTTP_SERVER_HXX_
# include "http/Server.hh"
# include <fnmatch.h>
# include <boost/asio.hpp>
# include <boost/iostreams/device/back_inserter.hpp>
# include <boost/iostreams/filtering_stream.hpp>
# include <boost/range/iterator.hpp>
# include <boost/range/iterator_range_core.hpp>
# include <regex>
# include <boost/system/error_code.hpp>
# include <cmath>
# include <sstream>
# include "format.hh"       // libcore
# include "log/helpers.hh"  // libcore
# include "log/logtypes.hh" // libcore
# include "types.hh"        // libcore
# include "http/Connection.hh"
# include "http/Request.hh"
# include "http/Response.hh"
# include "http/Template.hh"
# include "http/http_types.hh"

namespace ba = boost::asio;
namespace bs = boost::system;

namespace xtd {
namespace network {
namespace http {


template<typename Domain>
Server<Domain>::Handler::Handler(void) :
  m_path(),
  m_process(),
  m_filter(),
  m_matchAny(false)
{}

template<typename Domain>
bool
Server<Domain>::Handler::less(const Handler& p_obj1, const Handler& p_obj2)
{
  if (p_obj1.m_matchAny == p_obj2.m_matchAny)
    return std::less<string>()(p_obj2.m_path, p_obj1.m_path);
  return std::less<bool>()(p_obj1.m_matchAny, p_obj2.m_matchAny);
}



template<typename Domain>
template<typename T, typename... Arguments>
typename Server<Domain>::handler
Server<Domain>::h(T p_fn, Arguments&&... p_args)
{
  return
    std::bind(p_fn, p_args...,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3);
}

template<typename Domain>
template<typename T, typename... Arguments>
typename Server<Domain>::filter
Server<Domain>::f(T p_fn, Arguments&&... p_args)
{
  return
    std::bind(p_fn, p_args...,
              std::placeholders::_1);
}


template<typename Domain>
Server<Domain>::Server(void) :
  TBase()
{
  handler l_handler = h(&Server::h_error_html, this,
                        "unhandled request, you must bind handler and "
                        "filter to manage this request");

  bind_default(l_handler);
}


template<typename Domain>
Server<Domain>::~Server(void)
{
}

template<typename Domain>
void
Server<Domain>::start(void)
{
  log::info("network.http.server", "starting network::http server...");
  TBase::start();
  log::info("network.http.server", "starting network::http server...done");
}

template<typename Domain>
void
Server<Domain>::stop(void)
{
  log::info("network.http.server", "stopping network::http server...");
  TBase::stop();
  log::info("network.http.server", "stopping network::http server...done");
}

template<typename Domain>
typename Server<Domain>::cnx_sptr_t
Server<Domain>::createCnx(string p_hostname, uint32_t p_port)
{
  return cnx_sptr_t(new Connection<Domain>(*this, *TBase::m_ioService, p_hostname, p_port));
}


template<typename Domain>
void
Server<Domain>::afterAccept(cnx_sptr_t p_conn)
{
  TBase::do_receive(p_conn);
}


/**
 ** @details
 ** En http 1.1, sauf mention contraire, la connexion est
 ** persistante. Elle est donc non fermee par le client et
 ** le serveur se place en ecoute (do_receive).
 ** Le client peut legitimement fermer la connexion
 ** de son cote et le serveur recoit un "eof" qui dans ce cas precis,
 ** n'est pas une erreur.
 */
template <typename Domain>
void
Server<Domain>::onReceiveError(const bs::error_code p_error, cnx_sptr_t p_conn)
{
  sptr<Connection<Domain> > l_conn =
    std::static_pointer_cast<Connection<Domain> >(p_conn);

  if ((p_error == ba::error::eof) && (false == l_conn->getClosedByServer()))
    log::info("network.http.server", "onReceivedError (%s) : closed by client", p_conn->info(), HERE);
  else
    TBase::onReceiveError(p_error, p_conn);
}


template <typename Domain>
void
Server<Domain>::onReceiveTimeout(const bs::error_code p_error, cnx_sptr_t p_conn)
{
  sptr<Connection<Domain> > l_conn = std::static_pointer_cast<Connection<Domain> >(p_conn);

  if (!l_conn->getClosedByServer())
  {
    log::info("network.http.server", "onReceivedTimeout (%s) : client did not recycle cnx before server timeout", p_conn->info(), HERE);
  }
  else
  {
    log::err("network.http.server", "onReceivedTimeout (%s) : receive aborted, no answer from client before timeout", p_conn->info(), HERE);
  }

  TBase::onReceiveTimeout(p_error, p_conn);
}



template<typename Domain>
void
Server<Domain>::afterReceive(cnx_sptr_t         p_conn,
                             sptr<vector<char>> p_inBuffer)
{
  boost::iostreams::filtering_istream l_in;
  Request                             l_req;
  Response                            l_res;
  vector<char>                        l_outBuff;
  string                              l_value;
  bool                                l_closeByServer = false;

  l_in.push(boost::make_iterator_range(*p_inBuffer));

  processRequest(p_conn->getProcessID(), l_in, l_req, l_res);

  if (version::v1_0 == l_req.getVersion())
    l_closeByServer = true;
  else if ((true == l_req.getHeader("Connection", l_value)) && ("close" == l_value))
    l_closeByServer = true;

  if (true == l_closeByServer)
  {
    sptr<Connection<Domain> > l_conn =
      std::static_pointer_cast<Connection<Domain> >(p_conn);
    l_conn->setClosedByServer(true);
    l_res.addHeader("Connection", "close");
  } else {
    float    l_rcvTimeout = this->getReceiveTimeoutMs();
    uint32_t l_to         = std::floor(l_rcvTimeout / 1000);
    l_res.addHeader("Keep-Alive", format::vargs("timeout=%d, max=100", l_to));
  }

  {
    boost::iostreams::filtering_ostream l_out;
    l_out.push(boost::iostreams::back_inserter(l_outBuff));
    l_res.write(l_out);
    l_out.flush();
  }

  TBase::do_send(p_conn, l_outBuff);
}


template<typename Domain>
void
Server<Domain>::afterSend(cnx_sptr_t p_conn)
{
  sptr<Connection<Domain> > l_conn =
    std::static_pointer_cast<Connection<Domain> >(p_conn);

  if (false == l_conn->getClosedByServer())
    TBase::do_receive(p_conn);
}


template<typename Domain>
void
Server<Domain>::processRequest(uint32_t       p_processID,
                               std::istream&  p_request,
                               Request&       p_req,
                               Response&      p_res)
{
  if (status::ok != p_req.read(p_request))
  {
    log::err("network.http.server", "invalid http request", HERE);
    p_res.setVersion(version::v1_0);
    p_res.setData("request parsing error...");
    p_res.setStatus(code::internal_server_error);
    return;
  }

  findHandler(p_req)(p_processID, p_req, p_res);

  p_res.setVersion(p_req.getVersion());
}


template<typename Domain>
typename Server<Domain>::handler&
Server<Domain>::findHandler(const Request& p_req)
{
  for (auto& c_handler : m_handlerList)
  {
    if ((false == c_handler.m_matchAny) &&
        (0 == fnmatch(c_handler.m_path.c_str(), p_req.getPath().c_str(), 0)) &&
        (true == c_handler.m_filter(p_req)))
      return c_handler.m_process;
  }

  for (auto& c_handler : m_handlerList)
  {
    if ((true == c_handler.m_matchAny) &&
        (true == c_handler.m_filter(p_req)))
      return c_handler.m_process;
  }

  return m_defaultHandler;
}

template<typename Domain>
bool
Server<Domain>::f_none(const Request& /* p_request */)
{
  return true;
}

template<typename Domain>
bool
Server<Domain>::f_cgi_exist(const string&  p_cgiName,
                            const Request& p_req)
{
  string l_value;


  return ((true == p_req.getCgi(p_cgiName, l_value)) &&
          (0    != l_value.size()));
}

template<typename Domain>
bool
Server<Domain>::f_one_cgi_exist(const vector<string>& p_cgiName,
                                const Request&        p_req)
{
  bool l_status = false;

  for (uint32_t i = 0 ; i < p_cgiName.size(); i++)
  {
    if (f_cgi_exist(p_cgiName[i], p_req) == true)
    {
      l_status = true;
      break;
    }
  }

  return l_status;
}


template<typename Domain>
bool
Server<Domain>::f_cgi_equal(const string&  p_cgiName,
                            const string&  p_value,
                            const Request& p_req)
{
  string l_value;

  return ((true == p_req.getCgi(p_cgiName, l_value)) &&
          (p_value == l_value));
}


template<typename Domain>
bool
Server<Domain>::f_cgi_match(const string&  p_cgiName,
                            const string&  p_regex,
                            const Request& p_req)
{
  std::regex l_regex(p_regex);
  string       l_value;

  return ((true == p_req.getCgi(p_cgiName, l_value)) &&
          (true == std::regex_match(l_value, l_regex)));
}


template<typename Domain>
bool
Server<Domain>::f_post_exist(const string&  p_postName,
                             const Request& p_req)
{
  string l_value;

  return ((true == p_req.getPost(p_postName, l_value)) &&
          (0    != l_value.size()));
}


template<typename Domain>
bool
Server<Domain>::f_param_exist(const string&  p_name,
                              const Request& p_req)
{
  string l_value;

  return ((true == p_req.getParam(p_name, l_value)) &&
          (0    != l_value.size()));
}


template<typename Domain>
bool
Server<Domain>::f_post_equal(const string&  p_postName,
                             const string&  p_value,
                             const Request& p_req)
{
  string l_value;

  return ((true == p_req.getPost(p_postName, l_value)) &&
          (p_value == l_value));
}


template<typename Domain>
bool
Server<Domain>::f_post_match(const string&  p_postName,
                             const string&  p_regex,
                             const Request& p_req)
{
  std::regex l_regex(p_regex);
  string     l_value;

  return ((true == p_req.getPost(p_postName, l_value)) &&
          (true == std::regex_match(l_value, l_regex)));
}



template<typename Domain>
bool
Server<Domain>::f_header_exist(const string&  p_headerName,
                               const Request& p_req)
{
  string l_value;

  return ((true == p_req.getHeader(p_headerName, l_value)) &&
          (0    != l_value.size()));
}


template<typename Domain>
bool
Server<Domain>::f_header_equal(const string&  p_headerName,
                               const string&  p_value,
                               const Request& p_req)
{
  string l_value;

  return ((true == p_req.getHeader(p_headerName, l_value)) &&
          (p_value == l_value));
}


template<typename Domain>
bool
Server<Domain>::f_header_match(const string&  p_headerName,
                               const string&  p_regex,
                               const Request& p_req)
{
  std::regex l_regex(p_regex);
  string     l_value;

  return ((true == p_req.getHeader(p_headerName, l_value)) &&
          (true == std::regex_match(l_value, l_regex)));
}


template<typename Domain>
status
Server<Domain>::h_error_text(const string&     p_msg,
                             const uint32_t /* p_requestId */,
                             const Request&    p_request,
                             Response&         p_response)
{
  std::stringstream l_requestText;
  string            l_message;

  l_requestText << p_request;
  l_message = format::vargs("http error : %s\n"
                            "dumping request ...\n"
                            "%s\n", p_msg, l_requestText.str());
  p_response.setStatus(code::internal_server_error);
  p_response.addHeader("Content-Type", "text/plain");
  p_response.setData(l_message);
  return status::error;
}


template<typename Domain>
status
Server<Domain>::h_error_html(const string&  p_msg,
                             const uint32_t p_requestId,
                             const Request& p_req,
                             Response&      p_res)
{
  HtmlTemplate      l_tmpl;
  std::stringstream l_str;
  string             l_text;

  l_tmpl.setTitle("http::Server error");
  l_tmpl.setTemplate("<span style='color:red'> error </span> : {$message} <br/><br/> "
                     "dumping request... <br/>"
                     "<pre>{$requestText}</pre>");
  l_str << p_req;

  l_tmpl
    .add("requestText", l_str.str())
    .add("message", p_msg);

  if (status::ok != l_tmpl.resolve(l_text))
    return h_error_text(l_tmpl.getError(), p_requestId, p_req, p_res);

  p_res.setStatus(code::internal_server_error);
  p_res.addHeader("Content-Type", "text/html");
  p_res.setData(l_text);
  return status::error;
}



template<typename Domain>
status
Server<Domain>::h_template_file(Template&      p_tmpl,
                                const string&  p_filePath,
                                const uint32_t p_requestID,
                                const Request& p_req,
                                Response&      p_res)
{
  if (status::ok != p_tmpl.loadTemplate(p_filePath))
    return h_error_text("unable to find tpl file " + p_filePath, p_requestID, p_req, p_res);
  return h_gen(p_tmpl, p_requestID, p_req, p_res);
}

template<typename Domain>
status
Server<Domain>::h_gen(Generator&      p_tmpl,
                      const uint32_t   p_requestId,
                      const Request& p_req,
                      Response&      p_res)
{
  string l_text;

  if (status::ok != p_tmpl.resolve(l_text))
    return h_error_text(p_tmpl.getError(), p_requestId, p_req, p_res);

  p_res.setStatus(code::ok);
  p_res.addHeader("Content-Type", p_tmpl.getContentType());
  p_res.setData(l_text);
  return status::ok;
}

template<typename Domain>
status
Server<Domain>::h_redirect(const string&     p_dst,
                           const uint32_t /* p_requestId*/,
                           const Request& /* p_request */,
                           Response&         p_response)
{
  p_response.addHeader("Location", p_dst);
  p_response.setStatus(code::moved_temporarily);
  return status::ok;
}


template<typename Domain>
status
Server<Domain>::h_raw(const string&      p_data,
                      const string&      p_contentType,
                      const uint32_t /*  p_requestId*/,
                      const Request& /*  p_request */,
                      Response&          p_response)
{
  p_response.setData(p_data);
  p_response.addHeader("Content-Type", p_contentType);
  p_response.setStatus(code::ok);
  return status::ok;
}


template<typename Domain>
status
Server<Domain>::h_file(const string&  p_filePath,
                       const string&  p_contentType,
                       bool           p_cachable,
                       const uint32_t   p_requestId,
                       const Request& p_request,
                       Response&      p_response)
{
  ifstream     l_file(p_filePath.c_str());
  stringstream l_content;

  if (false == l_file.is_open())
    return h_error_text(format::vargs("unable to open requested file '%s'", p_filePath),
                        p_requestId,
                        p_request,
                        p_response);


  copy(std::istreambuf_iterator<char>(l_file.rdbuf()),
       std::istreambuf_iterator<char>(),
       std::ostreambuf_iterator<char>(l_content.rdbuf()));

  p_response.appendData(l_content.str());
  p_response.setStatus(code::ok);
  p_response.addHeader("Content-Type", p_contentType);
  p_response.setCachable(p_cachable);

  l_file.close();
  return status::ok;
}


template<typename Domain>
status
Server<Domain>::h_dir(const string&  p_dirPath,
                      const string&  p_contentType,
                      bool           p_cachable,
                      const uint32_t p_requestId,
                      const Request& p_req,
                      Response&      p_res)
{
  string   l_filePath = p_req.getPath();
  ifstream l_file;

  l_filePath = l_filePath.substr(l_filePath.find_last_of("/") + 1);
  l_filePath = p_dirPath + "/" + l_filePath;

  return h_file(l_filePath, p_contentType, p_cachable, p_requestId, p_req, p_res);
}


template<typename Domain>
void
Server<Domain>::bind_redirect(const string& p_src,
                              const string& p_dst,
                              filter        p_filter)
{
  bind(p_src,
       h(&Server::h_redirect, this, p_dst),
       p_filter);
}

template<typename Domain>
void
Server<Domain>::bind_xml(const string& p_url,
                         const string& p_xmlPath,
                         const string& p_xsltPath,
                         filter        p_filter)
{
  bind_file(p_url, p_xmlPath, "text/xml", false, p_filter);
  if (false == isUrlBinded(p_xmlPath))
    bind_file(p_url, p_xsltPath, "application/xslt+xml", false);
}



template<typename Domain>
void
Server<Domain>::bind_file(const string& p_path,
                          const string& p_filePath,
                          const string& p_contentType,
                          bool          p_cachable,
                          filter        p_filter)
{
  bind(p_path,
       h(&Server::h_file, this, p_filePath, p_contentType, p_cachable),
       p_filter);
}

template<typename Domain>
void
Server<Domain>::bind_dir(const string& p_url,
                         const string& p_dirPath,
                         const string& p_contentType,
                         bool          p_cachable,
                         filter        p_filter)
{
  bind(p_url,
       h(&Server::h_dir, this, p_dirPath, p_contentType, p_cachable),
       p_filter);
}

template<typename Domain>
void
Server<Domain>::bind(const string&      p_path,
                     handler            p_handler,
                     filter             p_filter,
                     const string& p_descr)
{
  Handler l_handler;
  l_handler.m_path    = p_path;
  l_handler.m_process = p_handler;
  l_handler.m_filter  = p_filter;
  l_handler.m_descr   = p_descr;
  m_handlerList.push_back(l_handler);

  sort(m_handlerList.begin(), m_handlerList.end(), &Handler::less);
}


template<typename Domain>
void
Server<Domain>::bind_public(const string&      p_path,
                            handler            p_handler,
                            const string& p_descr)
{
  bind(p_path, p_handler, &Server::f_none, p_descr);
}


template<typename Domain>
void
Server<Domain>::bind_any(handler p_handler,
                         filter  p_filter)
{
  Handler l_handler;
  l_handler.m_process  = p_handler;
  l_handler.m_filter   = p_filter;
  l_handler.m_matchAny = true;
  m_handlerList.push_back(l_handler);

  sort(m_handlerList.begin(), m_handlerList.end(), &Handler::less);
}

template<typename Domain>
void
Server<Domain>::bind_default(handler p_handler)
{
  m_defaultHandler = p_handler;
}

template<typename Domain>
bool
Server<Domain>::isUrlBinded(const string& p_urlPath) const
{
  auto l_cmp = [&p_urlPath](const Handler& p_handler) {
    return p_handler.m_path == p_urlPath;
  };

  return 0 != count_if(m_handlerList.begin(), m_handlerList.end(), l_cmp);
}


template<typename Domain>
void
Server<Domain>::getMatchingHandlers(const Request&              p_req,
                                    typename Handler::t_listof& p_list) const
{
  for (const auto& c_handler : m_handlerList)
  {
    if ((0 != c_handler.m_descr.size()) && (true == c_handler.m_filter(p_req)))
      p_list.push_back(c_handler);
  }
}

}}} //end namespaces


#endif // !NETWORK_HTTP_SERVER_HXX_
