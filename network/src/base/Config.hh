#ifndef NETWORK_BASE_CONFIG_HH_
# define NETWORK_BASE_CONFIG_HH_

# include <types.hh> // libcore

namespace xtd {
namespace network {
namespace base {


// , compress =
// inline bool     getCompress(void)         const;
// inline codec    getCompressionCodec(void) const;
// inline void setCompress(bool p_compress);
// inline void setCompressionCodec(codec p_compressionCodec);
// bool              m_compress;
// codec             m_compressionCodec;


class Config
{
public:
  /**
   ** @brief default constructor set nothing
   */
  Config(void);

  /**
   ** @brief Constructor make the socket setting
   ** @param p_flags    configuration flags
   ** @param p_lingerInterval linger interval
   */
  Config(uint32_t p_flags, uint32_t p_lingerInterval = 0);

public:
  inline bool     getLinger(void)           const;
  inline uint32_t getLingerTime(void)       const;
  inline bool     getNoDelay(void)          const;
  inline bool     getReuseAddr(void)        const;
  inline uint32_t getConnectTimeoutMs(void) const;
  inline uint32_t getSendTimeoutMs(void)    const;
  inline uint32_t getReceiveTimeoutMs(void) const;
  inline uint32_t getDnsCacheTTL(void)      const;

  inline void     setLinger(bool p_linger, uint32_t p_lingerTime);
  inline void     setNoDelay(bool p_noDelay);
  inline void     setReuseAddr(bool p_reuseAddr);
  inline void     setConnectTimeoutMs(uint32_t p_ms);
  inline void     setSendTimeoutMs(uint32_t p_ms);
  inline void     setReceiveTimeoutMs(uint32_t p_ms);
  inline void     setDnsCacheTTL(uint32_t p_ms);

private:
  bool     m_noDelay;
  bool     m_reuseAddr;
  bool     m_linger;
  uint32_t m_lingerTime;
  uint32_t m_dnsCacheTTL;
  uint32_t m_connectTimeoutMs;
  uint32_t m_sendTimeoutMs;
  uint32_t m_receiveTimeoutMs;
};

}}} // end namespaces

# include "base/Config.hxx"

#endif // !NETWORK_COMMCONFIGURATION_HXX_
