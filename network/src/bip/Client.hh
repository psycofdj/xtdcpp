#ifndef NETWORK_BIP_CLIENT_HH_
# define NETWORK_BIP_CLIENT_HH_

# include "base/Client.hh"

namespace xtd {
namespace network {

namespace bip {

/**
 ** @param TDomain : mode de connexion, utils::af_inet ou utils::af_unix
 ** @param TRequest Structure requète serialisable avec boost::serialization
 ** @param TResponse Structure réponse serialisable avec boost::serialization
 ** @details
 ** Client générique bip : gère l'envoi de structure TRequest et la réception de
 ** structure TResponse vers un serveur bip de même type.
 **
 ** Selon la configuration transmise au constructeur, les données pourront etre
 ** compressées avant l'envoi et décompressée à la réception.
 **
 ** La méthode send de cet objet est non bloquante et déclenche, en interne, la
 ** réception de la réponse du serveur. La méthode receive, elle, est bloquante
 ** jusqu'à la réception effective de la réponse. Cette approche permet à un
 ** utilisateur, qui aurait plusieurs client connectés vers plusieurs serveurs,
 ** d'envoyer ses requètes et de réceptionner en parallèle ses réponses pour au
 ** final aller au rythme du serveur le plus lent et pas subir la somme de tous
 ** les temps de réponse.
 **
 ** Thread safety :
 ** - même instance : non
 ** - instances différentes : oui
 */
template<class TRequest, class TResponse, typename TDomain = utils::af_inet>
class Client : public base::Client<TDomain>
{
public:
  typedef TRequest  TReq;
  typedef TResponse TRes;

private:
  typedef base::Client<TDomain>      TBase;
  typedef typename TBase::cnx_sptr_t cnx_sptr_t;

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

public:
  Client(const utils::Config& p_conf);
  virtual ~Client(void);

public:
  inline const uint32_t& getSendTotal(void) const       { return m_sendTotal; }
  inline const uint32_t& getSendSuccess(void) const     { return m_sendSuccess; }
  inline const uint32_t& getSendTimeout(void) const     { return m_sendTimeout; }
  inline const uint32_t& getSendError(void) const       { return m_sendError; }
  inline const uint32_t& getReceiveTotal(void) const    { return m_receiveTotal; }
  inline const uint32_t& getReceiveSuccess(void) const  { return m_receiveSuccess; }
  inline const uint32_t& getReceiveTimeout(void) const  { return m_receiveTimeout; }
  inline const uint32_t& getReceiveError(void) const    { return m_receiveError; }
  inline const uint32_t& getLastRTTMs(void) const       { return m_lastRTTMs; }

public:
  /**
   ** @brief envoi d'une structure requête au server
   ** @param p_request la requête
   ** @param p_debug si vrai, envoyer aussi la partie débug de la structure
   ** @return status::ok si tout va bien, status::error sinon
   ** @details
   ** Non bloquant, status::ok ne veut pas dire que le message est bien arrivé,
   ** simplement que l'objet étaient en étant d'envoyer une requête (pas
   ** déjà en attente de réponse par exemple).
   */
  status send(const TRequest& p_request, bool  p_debug);

  /**
   ** @brief reception de la structure réponse
   ** @param p_response structure à remplir avec la réponse du server
   ** @param p_debug sortira à la valeur true si le serveur a envoyé la partie débug de la structure
   ** @return status::ok si tout va bien, status::timeout en cas de timeout et staus::error en cas d'erreur.
   ** @details
   ** Cette méthode est bloquante, elle attend la réception effective de la réponse
   ** du serveur ou la survenue d'une erreur/timeout.
   */
  status receive(TResponse& p_response,  bool& p_debug);

  virtual bool shouldReceive(const TRequest&  p_request,
                             const bool       p_requestDebug);

private:
  cnx_sptr_t createCnx(string p_hostname, uint32_t p_port);
  void       do_receive(void);
  void       onSent(const boost::system::error_code p_error, bool p_shouldReceive);
  void       onReceived(const boost::system::error_code p_error,
                        utils::sharedBuf_t              p_response);
private:
  /** Semaphore uitilise pour bloquer l'utilisateur soit dans le send (noget) soit dans le receive */
  boost::interprocess::interprocess_semaphore m_userSemaphore;
  cnxstatus                                   m_status;
  utils::vectorBytes_t                        m_response;
  boost::posix_time::ptime                    m_lastSend;
  uint32_t                                    m_sendTotal;
  uint32_t                                    m_sendError;
  uint32_t                                    m_sendSuccess;
  uint32_t                                    m_sendTimeout;
  uint32_t                                    m_receiveTotal;
  uint32_t                                    m_receiveError;
  uint32_t                                    m_receiveTimeout;
  uint32_t                                    m_receiveSuccess;
  uint32_t                                    m_lastRTTMs;
};

}}} //end namespaces

#endif // !NETWORK_BIPCLIENT_HH_
