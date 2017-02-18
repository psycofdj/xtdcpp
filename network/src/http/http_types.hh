#ifndef NETWORK_HTTP_HH_
# define NETWORK_HTTP_HH_
# include <types.hh> // libcore

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
  proceeed                             = 100,
  switch_protocol                      = 101,
  processing                           = 102,

  //Successful 2xx
  ok                                   = 200,
  created                              = 201,
  accepted                             = 202,
  non_authorative_information          = 203,
  no_content                           = 204,
  reset_content                        = 205,
  partial_content                      = 206,
  multi_status                         = 207,
  content_dfferent                     = 210,
  im_used                              = 226,

  //Redirection 3xx
  multiple_choices                     = 300,
  moved_permanently                    = 301,
  moved_temporarily                    = 302,
  see_other                            = 303,
  not_modified                         = 304,
  use_proxy                            = 305,
  temporary_redirect                   = 307,
  permanent_redirect                   = 308,
  too_many_redirects                   = 309,

  //Client Error 4xx
  bad_request                          = 400,
  unauthorized                         = 401,
  payment_required                     = 402,
  forbidden                            = 403,
  not_found                            = 404,
  method_not_allowed                   = 405,
  not_Acceptable                       = 406,
  proxy_auth_required                  = 407,
  request_timeout                      = 408,
  conflit                              = 409,
  gone                                 = 410,
  length_required                      = 411,
  precondition_ailed                   = 412,
  request_entity_too_large             = 413,
  request_uri_too_long                 = 414,
  unsupported_media_type               = 415,
  requested_range_unsatisfiable        = 416,
  expectation_failed                   = 417,
  im_a_teapot                          = 418,
  bad_mapping                          = 421,
  unprocessable_entity                 = 422,
  locked                               = 423,
  method_failure                       = 424,
  unordered_collection                 = 425,
  upgrade_required                     = 426,
  precondition_required                = 428,
  too_many_requests                    = 429,
  request_header_fields_too_large      = 431,
  retry_with                           = 449,
  blocked_by_windows_parental_controls = 450,
  unavailable_for_legal_reasons        = 451,
  unrecoverable_error                  = 456,
  no_response                          = 444,
  ssl_certificate_error                = 495,
  ssl_certificate_required             = 496,
  http_request_sent_to_https_port      = 497,
  client_closed_request                = 499,

  //Server Error 5xx
  internal_server_error                = 500,
  not_implemented                      = 501,
  bad_gateway_ou_proxy_error           = 502,
  service_unavailable                  = 503,
  gateway_time_out                     = 504,
  http_version_not_supported           = 505,
  variant_also_negotiates              = 506,
  insufficient_storage                 = 507,
  loop_detected                        = 508,
  bandwidth_limit_exceeded             = 509,
  not_extended                         = 510,
  network_authentication_required      = 511,
  unknown_error                        = 520,
  web_server_is_down                   = 521,
  connection_timed_out                 = 522,
  origin_is_unreachable                = 523,
  a_timeout_occurred                   = 524,
  ssl_handshake_failed                 = 525,
  invalid_ssl_certificate              = 526,
  railgun_error                        = 527
 };

string        str(version p_version);
string        str(method  p_method);
const string& str(code    p_code);

}}}

#endif // !NETWORK_HTTP_HH_
