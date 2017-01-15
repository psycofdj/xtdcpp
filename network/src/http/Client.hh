#ifndef NETWORK_HTTP_CLIENT_HH_
# define NETWORK_HTTP_CLIENT_HH_

# include "base/Client.hh"

namespace xtd {
namespace network {
namespace http {

class Request;
class Response;

template<typename TDomain = utils::af_inet>
class Client : public base::Client<TDomain>
{
private:
  typedef base::Client<TDomain>      TBase;
  typedef typename TBase::cnx_sptr_t cnx_sptr_t;
  typedef typename TBase::cnxstatus  cnxstatus;

public:
  Client(const utils::Config& p_conf);
  virtual ~Client(void);

public:
  /**
   ** @brief envoi d'une structure requête au server
   ** @param p_request la requête
   ** @return status::ok si tout va bien, status::error sinon
   ** @details
   ** Non bloquant, status::ok ne veut pas dire que le message est bien arrivé,
   ** simplement que l'objet étaient en étant d'envoyer une requête (pas
   ** déjà en attente de réponse par exemple).
   */
  status send(const Request& p_request);

  /**
   ** @brief reception de la structure réponse
   ** @param p_response structure à remplir avec la réponse du server
   ** @return status::ok si tout va bien, status::timeout en cas de timeout et staus::error en cas d'erreur.
   ** @details
   ** Cette méthode est bloquante, elle attend la réception effective de la réponse
   ** du serveur ou la survenue d'une erreur/timeout.
   */
  status receive(Response& p_response);

private:
  cnx_sptr_t createCnx(string p_hostname, uint32_t p_port);
  void       do_receive(void);
  void       onSent(const boost::system::error_code p_error);
  void       onReceived(const boost::system::error_code p_error,
                        utils::sharedBuf_t              p_response);
private:
  /** Semaphore uitilise pour bloquer l'utilisateur soit dans le send (noget) soit dans le receive */
  boost::interprocess::interprocess_semaphore m_userSemaphore;
  cnxstatus                                   m_status;
  utils::vectorBytes_t                        m_response;
  boost::posix_time::ptime                    m_lastSend;
};

}}} //end namespaces

#endif // !NETWORK_HTTPCLIENT_HH_
