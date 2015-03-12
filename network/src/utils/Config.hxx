#ifndef NETWORK_UTILS_CONFIG_HXX_
# define NETWORK_UTILS_CONFIG_HXX_

namespace xtd {
namespace network {
namespace utils {


bool     Config::getLinger(void)           const                 { return m_linger;                                  }
uint32_t Config::getLingerTime(void)       const                 { return m_lingerTime;                              }
bool     Config::getNoDelay(void)          const                 { return m_noDelay;                                 }
bool     Config::getReuseAddr(void)        const                 { return m_reuseAddr;                               }
bool     Config::getCompress(void)         const                 { return m_compress;                                }
codec    Config::getCompressionCodec(void) const                 { return m_compressionCodec;                        }
uint32_t Config::getConnectTimeoutMs(void) const                 { return m_connectTimeoutMs;                        }
uint32_t Config::getSendTimeoutMs(void)    const                 { return m_sendTimeoutMs;                           }
uint32_t Config::getReceiveTimeoutMs(void) const                 { return m_receiveTimeoutMs;                        }
uint32_t Config::getDnsCacheTTL(void)      const                 { return m_dnsCacheTTL;                             }

void     Config::setNoDelay(bool p_noDelay)                      { m_noDelay = p_noDelay;                            }
void     Config::setLinger(bool p_linger, uint32_t p_lingerTime) { m_linger = p_linger; m_lingerTime = p_lingerTime; }
void     Config::setReuseAddr(bool p_reuseAddr)                  { m_reuseAddr = p_reuseAddr;                        }
void     Config::setCompress(bool p_compress)                    { m_compress = p_compress;                          }
void     Config::setCompressionCodec(codec p_compressionCodec)   { m_compressionCodec = p_compressionCodec;          }
void     Config::setConnectTimeoutMs(uint32_t p_ms)              { m_connectTimeoutMs = p_ms;                        }
void     Config::setSendTimeoutMs(uint32_t    p_ms)              { m_sendTimeoutMs    = p_ms;                        }
void     Config::setReceiveTimeoutMs(uint32_t p_ms)              { m_receiveTimeoutMs = p_ms;                        }
void     Config::setDnsCacheTTL(uint32_t p_ms)                   { m_dnsCacheTTL      = p_ms;                        }

}}} // end namespaces

#endif /* NETWORK_COMMCONFIGURATION_HXX_ */
