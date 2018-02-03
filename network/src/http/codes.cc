#include "http/codes.hh"
#include <iterator>
#include "types.hh" // libcore

namespace xtd {
namespace network {
namespace http {

map<uint32_t,string> codes::ms_messages = {
  { valueof(code::proceeed),                             "Continue"                             },
  { valueof(code::switch_protocol),                      "Switching Protocols"                  },
  { valueof(code::processing),                           "Processing"                           },
  //Successful 2xx
  { valueof(code::ok),                                   "OK"                                   },
  { valueof(code::created),                              "Created"                              },
  { valueof(code::accepted),                             "Accepted"                             },
  { valueof(code::non_authorative_information),          "Non-Authoritative Information"        },
  { valueof(code::no_content),                           "No Content"                           },
  { valueof(code::reset_content),                        "Reset Content"                        },
  { valueof(code::partial_content),                      "Partial Content"                      },
  { valueof(code::multi_status),                         "Multi-Status"                         },
  { valueof(code::content_dfferent),                     "Content Different"                    },
  { valueof(code::im_used),                              "IM Used"                              },
  //Redirection 3xx
  { valueof(code::multiple_choices),                     "Multiple Choices"                     },
  { valueof(code::moved_permanently),                    "Moved Permanently"                    },
  { valueof(code::moved_temporarily),                    "Moved Temporarily"                    },
  { valueof(code::see_other),                            "See Other"                            },
  { valueof(code::not_modified),                         "Not Modified"                         },
  { valueof(code::use_proxy),                            "Use Proxy"                            },
  { valueof(code::temporary_redirect),                   "Temporary Redirect"                   },
  { valueof(code::permanent_redirect),                   "Permanent Redirect"                   },
  { valueof(code::too_many_redirects),                   "Too many Redirects"                   },
  //Client Error 4xx
  { valueof(code::bad_request),                          "Bad Request"                          },
  { valueof(code::unauthorized),                         "Unauthorized"                         },
  { valueof(code::payment_required),                     "Payment Required"                     },
  { valueof(code::forbidden),                            "Forbidden"                            },
  { valueof(code::not_found),                            "Not Found"                            },
  { valueof(code::method_not_allowed),                   "Method Not Allowed"                   },
  { valueof(code::not_Acceptable),                       "Not Acceptable"                       },
  { valueof(code::proxy_auth_required),                  "Proxy Authentication Required"        },
  { valueof(code::request_timeout),                      "Request Time-out"                     },
  { valueof(code::conflit),                              "Conflict"                             },
  { valueof(code::gone),                                 "Gone"                                 },
  { valueof(code::length_required),                      "Length Required"                      },
  { valueof(code::precondition_ailed),                   "Precondition Failed"                  },
  { valueof(code::request_entity_too_large),             "Request Entity Too Large"             },
  { valueof(code::request_uri_too_long),                 "Request-URI Too Long"                 },
  { valueof(code::unsupported_media_type),               "Unsupported Media Type"               },
  { valueof(code::requested_range_unsatisfiable),        "Requested range unsatisfiable"        },
  { valueof(code::expectation_failed),                   "Expectation failed"                   },
  { valueof(code::im_a_teapot),                          "I’m a teapot"                         },
  { valueof(code::bad_mapping),                          "Bad mapping"                          },
  { valueof(code::unprocessable_entity),                 "Unprocessable entity"                 },
  { valueof(code::locked),                               "Locked"                               },
  { valueof(code::method_failure),                       "Method failure"                       },
  { valueof(code::unordered_collection),                 "Unordered Collection"                 },
  { valueof(code::upgrade_required),                     "Upgrade Required"                     },
  { valueof(code::precondition_required),                "Precondition Required"                },
  { valueof(code::too_many_requests),                    "Too Many Requests"                    },
  { valueof(code::request_header_fields_too_large),      "Request Header Fields Too Large"      },
  { valueof(code::retry_with),                           "Retry With"                           },
  { valueof(code::blocked_by_windows_parental_controls), "Blocked by Windows Parental Controls" },
  { valueof(code::unavailable_for_legal_reasons),        "Unavailable For Legal Reasons"        },
  { valueof(code::unrecoverable_error),                  "Unrecoverable Error"                  },
  { valueof(code::no_response),                          "No Response"                          },
  { valueof(code::ssl_certificate_error),                "SSL Certificate Error"                },
  { valueof(code::ssl_certificate_required),             "SSL Certificate Required"             },
  { valueof(code::http_request_sent_to_https_port),      "HTTP Request Sent to HTTPS Port"      },
  { valueof(code::client_closed_request),                "Client Closed Request"                },
  //Server Error 5xx
  { valueof(code::internal_server_error),                "Internal Server Error"                },
  { valueof(code::not_implemented),                      "Not Implemented"                      },
  { valueof(code::bad_gateway_ou_proxy_error),           "Bad Gateway ou Proxy Error"           },
  { valueof(code::service_unavailable),                  "Service Unavailable"                  },
  { valueof(code::gateway_time_out),                     "Gateway Time-out"                     },
  { valueof(code::http_version_not_supported),           "HTTP Version not supported"           },
  { valueof(code::variant_also_negotiates),              "Variant Also Negotiates"              },
  { valueof(code::insufficient_storage),                 "Insufficient storage"                 },
  { valueof(code::loop_detected),                        "Loop detected"                        },
  { valueof(code::bandwidth_limit_exceeded),             "Bandwidth Limit Exceeded"             },
  { valueof(code::not_extended),                         "Not extended"                         },
  { valueof(code::network_authentication_required),      "Network authentication required"      },
  { valueof(code::unknown_error),                        "Unknown Error"                        },
  { valueof(code::web_server_is_down),                   "Web Server Is Down"                   },
  { valueof(code::connection_timed_out),                 "Connection Timed Out"                 },
  { valueof(code::origin_is_unreachable),                "Origin Is Unreachable"                },
  { valueof(code::a_timeout_occurred),                   "A Timeout Occurred"                   },
  { valueof(code::ssl_handshake_failed),                 "SSL Handshake Failed"                 },
  { valueof(code::invalid_ssl_certificate),              "Invalid SSL Certificate"              },
  { valueof(code::railgun_error),                        "Railgun Error"                        },
  { 999,                                                 "Unknown Status Code"                  }
};


void
codes::setMessage(uint32_t p_code, const string& p_message)
{
  ms_messages[p_code] = p_message;
}


const string&
codes::getMessage(uint32_t p_code)
{
  const auto& c_value = ms_messages.find(p_code);
  if (ms_messages.end() == c_value)
    return ms_messages.rbegin()->second;
  return c_value->second;
}


const string&
codes::getMessage(code p_code)
{
  return getMessage(valueof(p_code));
}

}}}
