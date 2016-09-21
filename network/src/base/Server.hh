#ifndef NETWORK_BASE_SERVER_HH_
# define NETWORK_BASE_SERVER_HH_

# include <boost/noncopyable.hpp>
# include "utils/Config.hh"
# include "utils/CommTypeDefs.hh"
# include "utils/Utils.hh"

namespace xtd {
namespace network {
namespace utils { template<typename D> class Resolver; } // fwd decl
namespace base {

// fwd decl
template<typename D> class Connection;

/**
 ** @details
 ** 1. S'occupe de maintenir un identifiant de traitement (processID)
 **    croissant et contigue. Cet identifiant permettera au metier
 **    d'organiser ses ressources pour le travaille en parallele. Exemple :
 **    le metier instancier un tableau d'objet non thread-safe et utilise
 **    le processID comme index d'acces a sa resource.
 **
 ** 2. Cette classe ne derive pas de enable_shared_from_this
 **    car elle est destinee a etre utilisee avec une "xtd::Application"
 **    ce qui garantie la duree de vie de l'objet
 */
template <typename Domain>
class Server : private boost::noncopyable
{
protected:
  typedef typename std::shared_ptr<Connection<Domain> > cnx_sptr_t;

public:
  Server(void);
  virtual ~Server(void);


public:
  /**
   ** @brief Initialize server with params
   ** @param p_host : string hostname
   ** @param p_port : uint32_t port
   ** @param p_configuration : low level network configuration params
   ** @param p_nbThread : number of threads
   */
  virtual void initialize(const string&        p_host,
                          const uint32_t       p_port,
                          const utils::Config& p_configuration,
                          const size_t         p_nbThread);

  /**
   ** @brief Equivalent a start + join
   */
  void run(void);

  /**
   ** @brief attendre jusqu'a la fin des threads
   */
  void join(void);

  /**
   ** @brief Demarrer les threads et l'ecoute de nouvelle connexions
   */
  virtual void start(void);

  /**
   ** @brief Annule tout les operations en cours et coupe les threads
   */
  virtual void stop(void);



protected:
  virtual cnx_sptr_t createCnx(string p_hostname, uint32_t p_port)                  = 0;
  virtual void       afterAccept(cnx_sptr_t p_conn)                                 = 0;
  virtual void       afterSend(cnx_sptr_t   p_conn)                                 = 0;
  virtual void       afterReceive(cnx_sptr_t p_conn, utils::sharedBuf_t p_inBuffer) = 0;

  /**
   ** @details
   ** Est appelé lorsqu'une erreur de receive est détectée.
   ** Par défault, cela provoque un LOG_ERR et l'incrément du compteur
   ** d'erreur.
   ** Une classe fille peut surcharger ce comportement pour filtrer
   ** plus précisement les erreur à reporter.
   */
  virtual void onReceiveError(const boost::system::error_code p_error, cnx_sptr_t p_conn);

  /**
   ** @details
   ** Est appelé lorsqu'un timeout de receive est détecté.
   ** Par défault, cela provoque un LOG_ERR et l'incrément du compteur
   ** de timeout.
   ** Une classe fille peut surcharger ce comportement pour filtrer
   ** plus précisement les timeout à reporter.
   */
  virtual void onReceiveTimeout(const boost::system::error_code p_error, cnx_sptr_t p_conn);

protected:
  void do_receive(cnx_sptr_t p_conn);
  void do_send(cnx_sptr_t p_conn, const utils::vectorBytes_t& p_outData);

private:
  void accept(void);
  void onAccepted(const boost::system::error_code p_error, cnx_sptr_t p_conn);
  void onReceived(const boost::system::error_code p_error,
                  cnx_sptr_t                      p_conn,
                  utils::sharedBuf_t              p_request);
  void onSent(const boost::system::error_code p_error,
              cnx_sptr_t                      p_conn);

protected:
  inline uint32_t& getNbCurrentThread(void);
  inline uint32_t& getReceiveTotal(void);
  inline uint32_t& getReceiveError(void);
  inline uint32_t& getReceiveTimeout(void);
  inline uint32_t& getReceiveSuccess(void);
  inline uint32_t& getReceivedLastTime(void);
  inline uint32_t& getSendTotal(void);
  inline uint32_t& getSendSuccess(void);
  inline uint32_t& getSendError(void);
  inline uint32_t& getSendTimeout(void);
  inline uint32_t& getCnxTotal(void);
  inline uint32_t& getCnxAccepted(void);
  inline uint32_t& getCnxRejected(void);

protected:
  utils::Config                               m_conf;
  utils::deque_id<uint32_t>                                  m_dequeId;
  utils::ioServicePtr_t                                          m_ioService;
  utils::workPtr_t                                               m_work;
  std::shared_ptr<boost::asio::basic_socket_acceptor<Domain> > m_acceptor;

private:
  std::shared_ptr<utils::Resolver<Domain> > m_resolver;
  size_t                                 m_threadNb;
  boost::thread_group                         m_threadGroup;
  // counters
  uint32_t m_nbCurrentThread;
  uint32_t m_cnxTotal;
  uint32_t m_cnxAccepted;
  uint32_t m_cnxRejected;
  uint32_t m_receiveTotal;
  uint32_t m_receiveError;
  uint32_t m_receiveTimeout;
  uint32_t m_receiveSuccess;
  uint32_t m_receivedLastTime;
  uint32_t m_sendTotal;
  uint32_t m_sendError;
  uint32_t m_sendSuccess;
  uint32_t m_sendTimeout;
};


template<typename Domain> uint32_t& Server<Domain>::getNbCurrentThread(void)  { return m_nbCurrentThread;  }
template<typename Domain> uint32_t& Server<Domain>::getReceiveTotal(void)     { return m_receiveTotal;     }
template<typename Domain> uint32_t& Server<Domain>::getReceiveError(void)     { return m_receiveError;     }
template<typename Domain> uint32_t& Server<Domain>::getReceiveTimeout(void)   { return m_receiveTimeout;   }
template<typename Domain> uint32_t& Server<Domain>::getReceiveSuccess(void)   { return m_receiveSuccess;   }
template<typename Domain> uint32_t& Server<Domain>::getReceivedLastTime(void) { return m_receivedLastTime; }
template<typename Domain> uint32_t& Server<Domain>::getSendTotal(void)        { return m_sendTotal;        }
template<typename Domain> uint32_t& Server<Domain>::getSendSuccess(void)      { return m_sendSuccess;      }
template<typename Domain> uint32_t& Server<Domain>::getSendError(void)        { return m_sendError;        }
template<typename Domain> uint32_t& Server<Domain>::getSendTimeout(void)      { return m_sendTimeout;      }
template<typename Domain> uint32_t& Server<Domain>::getCnxTotal(void)         { return m_cnxTotal;         }
template<typename Domain> uint32_t& Server<Domain>::getCnxAccepted(void)      { return m_cnxAccepted;      }
template<typename Domain> uint32_t& Server<Domain>::getCnxRejected(void)      { return m_cnxRejected;      }

}}} // end namespaces

#endif // !NETWORK_BASE_SERVER_HH_
