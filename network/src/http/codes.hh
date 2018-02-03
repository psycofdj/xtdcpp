#ifndef NETWORK_CODES_HH_
# define NETWORK_CODES_HH_
# include "types.hh" // libcore
# include "http/http_types.hh"

namespace xtd {
namespace network {
namespace http {

class codes
{
public:

public:
  static void   setMessage(uint32_t p_code, const string& p_message);
  static const string& getMessage(uint32_t p_code);
  static const string& getMessage(code p_code);

private:
  static map<uint32_t, string> ms_messages;
};

}}}

#endif // !NETWORK_CODES_HH_
