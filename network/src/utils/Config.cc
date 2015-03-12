#include "utils/Config.hh"
#include <types.hh> // libcommon

namespace xtd {
namespace network {
namespace utils {

Config::Config(void) :
  m_noDelay(false),
  m_reuseAddr(false),
  m_linger(false),
  m_lingerTime(0),
  m_compress(false),
  m_compressionCodec(codec::zlib),
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
  m_compress(false),
  m_compressionCodec(codec::zlib),
  m_dnsCacheTTL(1800),
  m_connectTimeoutMs(1800),
  m_sendTimeoutMs(1800),
  m_receiveTimeoutMs(1800)
{
  setNoDelay   (p_flags & valueof(option::nodelay));
  setReuseAddr (p_flags & valueof(option::reuseaddr));
  setLinger    (p_flags & valueof(option::linger), p_lingerInterval);
  setCompress  (p_flags & valueof(option::compress));
}


}}} // end namespaces
