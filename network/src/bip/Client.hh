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
template<class TRequest, class TResponse, typename TDomain>
class Client : public base::Client<TDomain>
{
public:
  typedef TRequest  TReq;
  typedef TResponse TRes;

private:
  typedef base::Client<TDomain>      TBase;
  typedef typename TBase::cnx_sptr_t cnx_sptr_t;
  typedef typename TBase::cnxstatus  cnxstatus;

public:
  Client(void);
  Client(const Client&) = delete;
  virtual ~Client(void);

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

  void compressCodec(compress_codec p_codec);

  compress_codec compressCodec(void) const;


  virtual bool shouldReceive(const TRequest&  p_request,
                             const bool       p_requestDebug);

private:
  cnx_sptr_t createCnx(string p_hostname, uint32_t p_port);
  void       do_receive(void);
  void       onSent(const boost::system::error_code p_error, bool p_shouldReceive);
  void       onReceived(const boost::system::error_code p_error,
                        sptr<vector<char>>              p_response);
private:
  /** Semaphore uitilise pour bloquer l'utilisateur soit dans le send (noget) soit dans le receive */
  boost::interprocess::interprocess_semaphore m_userSemaphore;
  cnxstatus                                   m_status;
  vector<char>                                m_response;
  boost::posix_time::ptime                    m_lastSend;
  compress_codec                              m_codec;
};

}}} //end namespaces

#endif // !NETWORK_BIPCLIENT_HH_
