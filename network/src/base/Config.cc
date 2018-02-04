#include "base/Config.hh"
#include "types.hh" // libcore
#include "network_types.hh"

namespace xtd {
namespace network {
namespace base {


Config::Config(void) :
  m_noDelay(false),
  m_reuseAddr(false),
  m_linger(false),
  m_lingerTime(0),
  m_dnsCacheTTL(1800),
  m_connectTimeoutMs(1800),
  m_sendTimeoutMs(1800),
  m_receiveTimeoutMs(1800)
{
}

Config::Config(uint32_t p_flags, uint32_t p_lingerInterval):
  m_noDelay(false),
  m_reuseAddr(false),
  m_linger(false),
  m_lingerTime(0),
  m_dnsCacheTTL(1800),
  m_connectTimeoutMs(1800),
  m_sendTimeoutMs(1800),
  m_receiveTimeoutMs(1800)
{
  setNoDelay   (p_flags & valueof(socket_opt::nodelay));
  setReuseAddr (p_flags & valueof(socket_opt::reuseaddr));
  setLinger    (p_flags & valueof(socket_opt::linger), p_lingerInterval);
}


}}} // end namespaces
