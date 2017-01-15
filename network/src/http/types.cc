#include "types.hh"

namespace xtd {
namespace network {
namespace http {

string str(version p_version)
{
  switch (p_version)
  {
  case version::v1_0:
    return "1.0";
  case version::v1_1:
    return "1.1";
  };
  return "";
}

string str(method  p_method)
{
  string l_value;

  switch (p_method)
  {
  case method::get:    l_value = "GET";    break;
  case method::post:   l_value = "POST";   break;
  case method::head:   l_value = "HEAD";   break;
  case method::patch:  l_value = "PATCH";  break;
  case method::put:    l_value = "PUT";    break;
  case method::del:    l_value = "DELETE"; break;
  }
  return l_value;
}

string str(code p_status)
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


}}}
