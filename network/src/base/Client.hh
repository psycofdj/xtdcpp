#ifndef NETWORK_BASE_CLIENT_HH_
# define NETWORK_BASE_CLIENT_HH_

# include <string>
# include <boost/asio.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <types.hh> // libcore
# include "base/Config.hh"
# include "utils/Utils.hh"
# include "utils/Resolver.hh"

namespace xtd {
namespace network {
namespace base {

// fwd decl
class ThreadManager;
template <typename Domain> class Connection;

/**
 * TODO explain private + shared_ptr
 * */
template <typename Domain>
class Client : public Config
{
protected:
  typedef sptr<Connection<Domain> > cnx_sptr_t;

  /**
   ** @brief Code pour l'état du client
   */
  enum class cnxstatus : int32_t
  {
    available = 0, /**< pret à envoyer une nouvelle requête (nouveau send) */
    reserved  = 1, /**< requête en cours d'envoi sur le réseau */
    sent      = 2, /**< requête envoyé, en attente de réception réseau de la réponse */
    received  = 3, /**< réponse réseau reçue, en attente de l'appel à client à receive */
    error     = 4, /**< envoi ou reception réseau en timeout, en attente d'un appel client à receive */
    timeout   = 5  /**< envoi ou reception réseau en erreur, en attente d'un appel client à receive */
  };

protected:
  Client(void);
  Client(const Client&) = delete;

public:
  virtual ~Client(void);

public:
  status connect(const string& p_hostname,
                 const uint32_t p_port);
  void   async_connect(const string& p_hostname,
                       const uint32_t p_port);
  status wait_async_connect();
  status reconnect(void);
  void   close(void);

public:
  inline const uint32_t& getCnxTotal(void) const        { return m_cnxTotal; }
  inline const uint32_t& getCnxSuccess(void) const      { return m_cnxSuccess; }
  inline const uint32_t& getCnxTimeout(void) const      { return m_cnxTimeout; }
  inline const uint32_t& getCnxError(void) const        { return m_cnxError; }
  inline const uint32_t& getSendTotal(void) const       { return m_sendTotal;      }
  inline const uint32_t& getSendSuccess(void) const     { return m_sendSuccess;    }
  inline const uint32_t& getSendTimeout(void) const     { return m_sendTimeout;    }
  inline const uint32_t& getSendError(void) const       { return m_sendError;      }
  inline const uint32_t& getReceiveTotal(void) const    { return m_receiveTotal;   }
  inline const uint32_t& getReceiveSuccess(void) const  { return m_receiveSuccess; }
  inline const uint32_t& getReceiveTimeout(void) const  { return m_receiveTimeout; }
  inline const uint32_t& getReceiveError(void) const    { return m_receiveError;   }
  inline const uint32_t& getLastRTTMs(void) const       { return m_lastRTTMs;      }

protected:
  virtual cnx_sptr_t createCnx(string p_hostname, uint32_t p_port) = 0;
  virtual void       onConnected(const boost::system::error_code p_error);

private:
  ThreadManager&                              m_threadManager;
  sptr<utils::Resolver<Domain> >              m_resolver;
  string                                      m_hostname;
  uint32_t                                    m_port;
  boost::interprocess::interprocess_semaphore m_semaphoreConnect;

protected:
  boost::asio::io_service& m_ioService;
  cnx_sptr_t               m_connection;
  status                   m_connectStatus;

protected:
  uint32_t m_cnxTotal;
  uint32_t m_cnxSuccess;
  uint32_t m_cnxTimeout;
  uint32_t m_cnxError;
  uint32_t m_sendTotal;
  uint32_t m_sendError;
  uint32_t m_sendSuccess;
  uint32_t m_sendTimeout;
  uint32_t m_receiveTotal;
  uint32_t m_receiveError;
  uint32_t m_receiveTimeout;
  uint32_t m_receiveSuccess;
  uint32_t m_lastRTTMs;
};

}}} //end namepaces


#endif // !NETWORK_BASE_CLIENT_HH_
