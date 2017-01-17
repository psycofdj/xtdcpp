#ifndef NETWORK_BIP_SERVER_HH_
# define NETWORK_BIP_SERVER_HH_

#include "base/Server.hh"

namespace xtd {
namespace network {
namespace bip {

/**
 ** @param Domain : mode de connexion, utils::af_inet ou utils::af_unix
 ** @param TReq : Structure requête serialisable avec boost::serialization
 ** @param TRes : Structure réponse serialisable avec boost::serialization
 ** @details
 ** Serveur générique bip : gère la réception de structure TRequest et l'envoi
 ** de structure TResponse vers un client bip de même type.
 **
 ** Cet objet est destiné à être hérité des différents serveurs qui souhaitent
 ** communiquer en bip, ces derniers n'ont qu'a implémenter la méthode virtuelle
 ** pure Server::processObjectRequest pour calculer la réponse à envoyer à partir
 ** de la requête reçue
 */
template<typename TReq, typename TRes, typename Domain>
class Server : public base::Server<Domain>
{
private:
  typedef base::Server<Domain>       TBase;
  typedef typename TBase::cnx_sptr_t cnx_sptr_t;

public:
  /**
   ** @brief Constructeur
   */
  Server(void);
  virtual ~Server(void);

  /**
   ** @brief invoke base start method and print log
   */
  void start(void);

  /**
   ** @brief invoke base stop method and print log
   */
  void stop(void);

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

protected:
  /**
   ** @brief Calcul d'une réponse à partir d'une requête
   ** @details
   ** Cette méthode virtuelle pure est à définir dans les différents serveurs bip,
   ** c'est la partie metier qui calcul la réponse à envoyer à partir d'une requête reçue
   */
  virtual void processObjectRequest(uint32_t p_requestID,
                                    const TReq&   p_request,
                                    const bool    p_requestDebug,
                                    TRes&         p_response,
                                    bool&         p_responseDebug) = 0;

  virtual bool shouldReply(uint32_t    p_requestID,
                           const TReq& p_request,
                           const bool  p_requestDebug,
                           TRes&       p_response,
                           bool&       p_responseDebug);

  inline uint32_t& getReceiveEof(void);

protected:
  bool m_isPersistent;


private:
  cnx_sptr_t createCnx(string p_hostname, uint32_t p_port);
  void       afterAccept(cnx_sptr_t p_conn);
  void       afterSend(cnx_sptr_t   p_conn);
  void       afterReceive(cnx_sptr_t p_conn, sptr<vector<char>> p_inBuffer);
  void       onReceiveError(const boost::system::error_code p_error, cnx_sptr_t p_conn);
  void       onReceiveTimeout(const boost::system::error_code p_error, cnx_sptr_t p_conn);

private:
  //counter to monitor fake on receive error
  uint32_t m_receiveEof;
};

template<typename TReq, typename TRes, typename Domain>
uint32_t& Server<TReq, TRes, Domain>::getReceiveEof(void){ return m_receiveEof;}

}}} //end namespaces

#endif // !NETWORK_BIP_SERVER_HH_
