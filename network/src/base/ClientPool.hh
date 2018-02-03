#ifndef NETWORK_BASE_CLIENTPOOL_HH_
# define NETWORK_BASE_CLIENTPOOL_HH_

# include <deque>
# include <mutex>
# include <memory>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <types.hh> // libcore
# include "base/Client.hh"

namespace xtd {
namespace network {
namespace base {

template<typename TClient>
class ClientPool
{
private:
  class PersistentClient : protected TClient
  {
    friend class ClientPool<TClient>;

  public:
    PersistentClient(const string&  p_hostname,
                     const uint32_t p_port);
    ~PersistentClient(void);

  private:
    void    async_connect(void);
    status  wait_async_connect(void);
    bool    isObsolete(uint32_t p_ttlMs) const;
    void    invalidate(void);

    template<typename... Args>
    status send(Args&&... p_args);

    template<typename... Args>
    status receive(Args&&... p_args);

  private:
    const string&            m_hostname;
    const uint32_t           m_port;
    bool                     m_isConnected;
    boost::posix_time::ptime m_lastUsed;
  };

public:
  typedef sptr<PersistentClient> client_type;


public:
  /**
   ** Constructor
   */
  ClientPool(const string&  p_hostname,
             const uint32_t p_port,
             const uint32_t p_ttlMs);

public:
  inline const uint32_t& getSendTotal(void)   const { return m_sendTotal;   }
  inline const uint32_t& getSendSuccess(void) const { return m_sendSuccess; }
  inline const uint32_t& getSendError(void)   const { return m_sendError;   }
  inline const uint32_t& getSendTimeout(void) const { return m_sendTimeout; }
  inline const uint32_t& getRcvTotal(void)    const { return m_rcvTotal;    }
  inline const uint32_t& getRcvSuccess(void)  const { return m_rcvSuccess;  }
  inline const uint32_t& getRcvError(void)    const { return m_rcvError;    }
  inline const uint32_t& getRcvTimeout(void)  const { return m_rcvTimeout;  }
  inline const uint32_t& getCurNbClient(void) const { return m_curNbClient; }
  inline const uint32_t& getRecycleHit(void)  const { return m_recycleHit;  }
  inline const uint32_t& getRecycleMiss(void) const { return m_recycleMiss; }

public:
  /**
   ** @brief Réservation d'un client
   */
  client_type acquire(void);

  /**
   ** @brief Envoie d'une requete en utilisant l'identifiant de client réservé
   */
  template<typename... Args>
  status send(client_type& p_client, Args&&... p_args);

  /**
   ** @brief reception d'une reponse en utilisant l'identifiant de client réservé
   */
  template<typename... Args>
  status receive(client_type& p_client, Args&&... p_args);

  /**
   ** @brief libration d'un client réservé
   */
  void release(client_type& p_client);

private:
  std::mutex              m_mutex;
  std::deque<client_type> m_available;
  const string            m_hostname;
  const uint32_t          m_port;
  const uint32_t          m_ttlMs;

private:
  uint32_t m_sendTotal;
  uint32_t m_sendSuccess;
  uint32_t m_sendError;
  uint32_t m_sendTimeout;
  uint32_t m_rcvTotal;
  uint32_t m_rcvSuccess;
  uint32_t m_rcvError;
  uint32_t m_rcvTimeout;
  uint32_t m_curNbClient;
  uint32_t m_acquiredClients;
  uint32_t m_recycleHit;
  uint32_t m_recycleMiss;
};

}}}

#include "base/ClientPool.hxx"

#endif // !NETWORK_BIP_CLIENTPOOL_HH_
