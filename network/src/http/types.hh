#ifndef NETWORK_HTTP_TYPES_HH_
# define NETWORK_HTTP_TYPES_HH_
# include <types.hh> //libcore

namespace xtd {
namespace network {
namespace http {

enum class version : uint32_t {
  v1_0,
  v1_1
 };

enum class method : uint32_t {
  get    = 0,
  post   = 1,
  head   = 2,
  patch  = 3,
  put    = 4,
  del    = 5
 };

enum class code : uint32_t {
  //Information 1xx
  proceeed, //Continue
  switch_protocol, //Switching protocol
  //Successful 2xx
  ok, //OK
  created, //Created
  accepted, //Accepted
  no_content, //No Content
  partial_content, //Partial content
  //Redirection 3xx
  multiple_choices, //Multiple Choices
  moved_permanently, //Moved Permanently
  moved_temporarily, //Moved Temporarily
  not_modified, //Not Modified
  //Client Error 4xx
  bad_request, //Bad Request
  unauthorized, //Unauthorized
  forbidden, //Forbidden
  not_found, //Not Found
  method_not_allowed, //Method not allowed
  //Server Error 5xx
  internal_error, //Internal Server Error
  not_implemented, //Not Implemented
  bad_gateway, //Bad Gateway
  service_unavailable //Service Unavailable
 };

string str(version p_version);
string str(method  p_method);
string str(code    p_code);

}}}

#endif // !NETWORK_HTTP_TYPES_HH_
