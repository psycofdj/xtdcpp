#include "http/http_types.hh"
#include "http/codes.hh"

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

const string& str(code p_status)
{
  return codes::getMessage(p_status);
}


}}}
