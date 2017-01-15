#ifndef NETWORK_HTTP_RESPONSE_HH_
# define NETWORK_HTTP_RESPONSE_HH_

# include <types.hh> //libcore
# include "http/Message.hh"

namespace xtd {
namespace network {
namespace http {

class Response : public Message
{
public:
  Response(void);

public:
  void setStatus(code p_status);
  void setData(const string& p_data);
  void appendData(const string& p_data);
  void setCachable(bool p_status);
  code getStatus(void) const;

public:
  void writeInitial(std::ostream& p_stream) const override;
  void writeHeaders(std::ostream& p_stream) const override;

private:
  code         m_status;
  bool         m_cachable;
};

std::ostream& operator<<(std::ostream& p_buf, const Response& p_obj);


}}} //end namespaces

#endif // !NETWORK_HTTP_RESPONSE_HH_
