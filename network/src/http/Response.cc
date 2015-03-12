#include "Response.hh"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>




namespace xtd {
namespace network {
namespace http {


Response::Response(void) :
  m_status(code::bad_request),
  m_headers(),
  m_data(),
  m_cachable(false)
{
}

void
Response::setCachable(bool p_status)
{
  m_cachable = p_status;
}

void
Response::setStatus(code p_status)
{
  m_status = p_status;
}

void
Response::addHeader(const string& p_name, const string& p_value)
{
  m_headers[p_name] = p_value;
}

void
Response::setData(const string& p_data)
{
  m_data = p_data;
}


void
Response::appendData(const string& p_data)
{
  m_data += p_data;
}

void
Response::setVersion(const string& p_version)
{
  m_version = p_version;
}

code
Response::getStatus(void) const
{
  return m_status;
}

bool
Response::getHeader(const string& p_name, string& p_value) const
{
  t_header_map::const_iterator c_header = m_headers.find(p_name);
  if (c_header == m_headers.end())
    return false;
  p_value = c_header->second;
  return true;
}

const Response::t_header_map&
Response::getHeaders(void) const
{
  return m_headers;
}

const string&
Response::getData(void) const
{
  return m_data;
}

const string&
Response::getVersion(void) const
{
  return m_version;
}


void
Response::finalize(void)
{
  addHeader("Content-Size",   boost::lexical_cast<string>(getData().size()));
  addHeader("Content-Length", boost::lexical_cast<string>(getData().size()));

  if (true == m_cachable)
  {
    std::stringstream        l_out;
    uint64_t                 l_cacheTTLSec = 365 * 24 * 60 * 60;
    boost::posix_time::ptime l_time        =
      boost::posix_time::second_clock::universal_time() +
      boost::posix_time::seconds(l_cacheTTLSec);

    l_out.imbue(std::locale(l_out.getloc(), new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT")));
    l_out << l_time;

    addHeader("Expires",       l_out.str());
    addHeader("Cache-Control", boost::str(boost::format("private,maxage=%d") % l_cacheTTLSec));
  }
  else
  {
    addHeader("Pragma",        "no-cache");
    addHeader("Cache-Control", "no-cache");
  }
}


void
Response::write(std::ostream& p_response) const
{
  t_header_map::const_iterator c_header;
  string                       l_status = statusToMessage(getStatus());
  string                       l_data   = getData();

  p_response << boost::format("HTTP/%s %s\r\n") % getVersion() % l_status;
  for (c_header = getHeaders().begin();
       c_header != getHeaders().end();
       c_header++)
    p_response << boost::str(boost::format("%s: %s\r\n")
                             % c_header->first
                             % c_header->second);
  p_response << "\r\n";
  p_response << getData();
}

string
Response::statusToMessage(code p_status)
{
  string l_value;

  switch (p_status)
  {
    //Information 1xx
  case code::proceeed:            l_value = "100 Continue";              break;
  case code::switch_protocol:     l_value = "101 Switching protocol";    break;
    //Successful 2xx
  case code::ok:                  l_value = "200 OK";                    break;
  case code::created:             l_value = "201 Created";               break;
  case code::accepted:            l_value = "202 Accepted";              break;
  case code::no_content:          l_value = "204 No Content";            break;
  case code::partial_content:     l_value = "206 Partial content";       break;

    //Redirection 3xx
  case code::multiple_choices:    l_value = "300 Multiple Choices";      break;
  case code::moved_permanently:   l_value = "301 Moved Permanently";     break;
  case code::moved_temporarily:   l_value = "302 Moved Temporarily";     break;
  case code::not_modified:        l_value = "304 Not Modified";          break;
    //Client Error 4xx
  case code::bad_request:         l_value = "400 Bad Request";           break;
  case code::unauthorized:        l_value = "401 Unauthorized";          break;
  case code::forbidden:           l_value = "403 Forbidden";             break;
  case code::not_found:           l_value = "404 Not Found";             break;
  case code::method_not_allowed:  l_value = "405 Method not allowed";    break;
    //Server Error 5xx
  case code::internal_error:      l_value = "500 Internal Server Error"; break;
  case code::not_implemented:     l_value = "501 Not Implemented";       break;
  case code::bad_gateway:         l_value = "502 Bad Gateway";           break;
  case code::service_unavailable: l_value = "503 Service Unavailable";   break;
  }
  return l_value;
}

}}} //end namespaces
