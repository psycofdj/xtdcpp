#ifndef NETWORK_HTTP_RESPONSE_HH_
# define NETWORK_HTTP_RESPONSE_HH_

#include <types.hh> //libcommon

namespace xtd {
namespace network {
namespace http {

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

class Response
{
public:
  typedef map<string, string> t_header_map;

public:
  Response(void);

public:
  void setStatus(code p_status);
  void addHeader(const string& p_name, const string& p_value);
  void setData(const string& p_data);
  void appendData(const string& p_data);
  void setVersion(const string& p_version);
  void setCachable(bool p_status);

  code          getStatus(void) const;
  bool                getHeader(const string& p_name, string& p_value) const;
  const t_header_map& getHeaders(void) const;
  const string&       getData(void) const;
  const string&       getVersion(void) const;
  static string       statusToMessage(code p_status);


public:
  void write(std::ostream& p_response) const;
  void finalize(void);

private:
  string       m_version;
  code         m_status;
  t_header_map m_headers;
  string       m_data;
  bool         m_cachable;
};


}}} //end namespaces

#endif // !NETWORK_HTTP_RESPONSE_HH_
