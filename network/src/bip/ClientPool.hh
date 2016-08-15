#ifndef NETWORK_BIP_CLIENTPOOL_HH_
# define NETWORK_BIP_CLIENTPOOL_HH_

#include <deque>
#include <set>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "protocol/bip/Client.hh"
#include "utils/CommTypeDefs.hh"


namespace xtd {
namespace network {

namespace bip {

template<class TRequest, class TResponse, typename TDomain = utils::af_inet>
class ClientPool
{
public:
  class PersistentClient : protected bip::Client<TRequest, TResponse, TDomain>
  {
    friend class ClientPool<TRequest, TResponse, TDomain>;

  public:
    /**
     ** Constructor
     */
    PersistentClient(const string&        p_hostname,
                     const uint32_t       p_port,
                     const utils::Config& p_conf);
    ~PersistentClient(void);

  private:
    void    async_connect(void);
    status  wait_async_connect(void);
    status  send(const TRequest& p_request, bool  p_debug);
    status  receive(TResponse& p_response,  bool& p_debug);
    bool    isObsolete(uint32_t p_ttlMs) const;
    void    invalidate(void);

  private:
    const string&            m_hostname;
    const uint32_t           m_port;
    bool                     m_isConnected;
    boost::posix_time::ptime m_lastUsed;
  };

  typedef boost::shared_ptr<PersistentClient> t_client_sptr;
  typedef std::deque<t_client_sptr>           t_pool;

public:
  /**
   ** Constructor
   */
  ClientPool(const string&        p_hostname,
             const uint32_t       p_port,
             const utils::Config& p_conf,
             const uint32_t       p_ttlMs = 20000);

public:
  inline const uint32_t& getSendTotal(void) const             { return m_sendTotal;   }
  inline const uint32_t& getSendSuccess(void) const           { return m_sendSuccess; }
  inline const uint32_t& getSendError(void) const             { return m_sendError;   }
  inline const uint32_t& getSendTimeout(void) const           { return m_sendTimeout; }
  inline const uint32_t& getRcvTotal(void) const              { return m_rcvTotal;    }
  inline const uint32_t& getRcvSuccess(void) const            { return m_rcvSuccess;  }
  inline const uint32_t& getRcvError(void) const              { return m_rcvError;    }
  inline const uint32_t& getRcvTimeout(void) const            { return m_rcvTimeout;  }
  inline const uint32_t& getCurNbClient(void) const           { return m_curNbClient; }
  inline const uint32_t& getRecycleHit(void) const            { return m_recycleHit;  }
  inline const uint32_t& getRecycleMiss(void) const           { return m_recycleMiss; }

public:
  /**
   ** @brief Réservation d'un client
   */
  t_client_sptr acquire(void);

  /**
   ** @brief Envoie d'une requete en utilisant l'identifiant de client reservé
   */
  status send(t_client_sptr& p_client, const TRequest& p_request, bool  p_debug);

  /**
   ** @brief reception d'une reponse en utilisant l'identifiant de client reservé
   */
  status receive(t_client_sptr& p_client, TResponse& p_response,  bool& p_debug);

  /**
   ** @brief libration d'un client reservé
   */
  void release(t_client_sptr& p_client);

private:
  boost::mutex       m_mutex;
  t_pool             m_available;
  const string  m_hostname;
  const uint32_t m_port;
  const uint32_t m_ttlMs;
  const utils::Config m_conf;

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

#include "ClientPool.hxx"

#endif // !NETWORK_BIP_CLIENTPOOL_HH_
