#ifndef NETWORK_BASE_CONNECTION_HH_
# define NETWORK_BASE_CONNECTION_HH_


# include <boost/asio.hpp>
# include <types.hh> // libcommon
# include "utils/Config.hh"
# include "utils/CommTypeDefs.hh"
# include "utils/Resolver.hh"

namespace xtd {
namespace network {
namespace base {


/**
 ** @brief Base class from which all connections should derive
 ** @param Domain type de connexion @ref utils::af_inet ou utils::af_unix
 ** @details
 **
 ** Le premier rôle de cet objet est de mutualiser le pilotage de la socket par
 ** le client et le serveur.
 **
 ** Est à la charge de cet objet :
 ** - définir les primitives asynchrones de base connect, accept, send, receive et close
 ** - gérer tous les contrôles d'erreur
 ** - gérer les timeouts
 **
 ** La gestion des d'erreur et des timeouts se fait en cascadant une callback locale
 ** entre le déclanchement de la callback asynchrone par le boost::asio::io_service
 ** et la callback du client/server
 **
 ** Pour permettre l'implementation de tout types de prototole, cet objet
 ** n'écrit, ni ne lit directement sur la socket. Il délègue cettte tâche aux
 ** méthodes virtuelles pures @ref async_write et @ref async_read.
 **
 ** Ces méthodes qui implémentées dans une classe fille pour chacun des protocoles.
 ** C'est typiquement là que l'on codera la création d'un header de taille fixe
 ** (ou à délimiteur fixe) qui contiendra la taille du message à lire (partie data)
 **
 ** - Thread safety :
 **   - plusieurs instances : oui
 **   - meme instance       : non
 **
 ** - Constructeur protegé, n'a pas de sens d'utiliser cette classe
 **   telle quelle à cause des virtuelles pures
 **
 ** - Bien que les primitives accept et connect ne soient pas utilisée à la fois
 **   dans le client et le server, leur implémentation a été déportée dans cet
 **   objet pour deux raisons :
 **   - pour garantir la mise sur le strand
 **   - garantir l'initialisation correct des informations de socket (appel
 **     à setSocketInfo qu'il faut effectuer seulement après ouverture
 **     effective de la socket)
 */
template <typename Domain>
class Connection : public boost::enable_shared_from_this<Connection<Domain> >,
                   public boost::noncopyable
{
public:
  /** @brief shared_ptr sur this */
  typedef boost::shared_ptr<Connection<Domain> > cnx_sptr_t;

protected:
  // 1.
  explicit Connection(const utils::Config& p_configuration,
                      boost::asio::io_service&                p_ioService,
                      const string                            p_hostname,
                      const uint32_t                          p_port);

public:
  /** @brief descruteur */
  virtual ~Connection(void);

public:
  inline void         setProcessID(uint32_t p_procesID);
  inline uint32_t getProcessID(void) const;

public:
  /**
   ** @brief ouverture aynchrone de socket (server only)
   ** @details
   ** Appel a p_onAccepted lorsque l'appel aura ete execute (qu'il y ait erreur
   ** ou non)
   **
   ** L'acceptor est passe en parametre car c'est chez lui qu'il faut
   ** enregistré l'évenement.
   */
  void accept(boost::shared_ptr<boost::asio::basic_socket_acceptor<Domain> > p_acceptor,
              utils::handler_t                                               p_onAccepted);

  /**
   ** @brief connexion aynchrone de socket (client only)
   ** @details
   ** Appel a p_onConnected lorsque l'appel aura ete execute (qu'il y ait erreur
   ** ou non)
   */
  void connect(boost::shared_ptr<utils::Resolver<Domain> > p_resolver,
               utils::handler_t                            p_onConnected);

  /**
   ** @brief envoi asynchrone d'un message sur la socket
   ** @details
   ** Appel a p_onSent lorsque l'appel aura ete execute (qu'il y ait erreur
   ** ou non)
   */
  void send(const utils::vectorBytes_t& p_outData,
            utils::handler_t            p_onSent);

  /**
   ** @brief reception asynchrone d'un message de la socket
   ** @details
   ** Appel a p_onReceived lorsque l'appel aura ete execute (qu'il y ait erreur
   ** ou non)
   */
  void receive(utils::sharedBuf_t p_inData,
               utils::handler_t   p_onReceived);

  /**
   ** @brief fermeture de la socket et annule ses operations de lecture ecriture
   */
  void close(void);

  /**
   ** @brief annule toutes les operations asynchrones en cours de la socket
   */
  void cancel(void);

  /**
   ** @brief information de connexion (pour les logs)
   */
  const string& info(void) const;



private:
  void do_accept(boost::shared_ptr<boost::asio::basic_socket_acceptor<Domain> > p_acceptor,
                 utils::handler_t                                               p_onAccepted);
  /**
   ** @details
   ** L'acceptor est gardé jusqu'à l'exécution de la callback pour
   ** garantir sa durée de vie dans le cas où un utilisateur peu clairvoyant
   ** ait décidé d'instancier un acceptor pour chaque connection...)
   */
  void onAccepted(boost::system::error_code                                      p_error,
                  boost::shared_ptr<boost::asio::basic_socket_acceptor<Domain> > p_acceptor,
                  utils::handler_t                                               p_onAccepted);


  /* connextion  */

  /**
   ** @details
   ** 1. parametres p_host et p_port en copie pour garantir la duree
   **    de vie des donnees et eviter les dangling references
   */
  void do_connect(boost::shared_ptr<utils::Resolver<Domain> > p_resolver,
                  utils::handler_t                            p_onConnected);
  void connectTimeout(const boost::system::error_code p_error);
  void onConnected(const boost::system::error_code           p_error,
                   boost::shared_ptr<utils::deadLineTimer_t> p_timer,
                   utils::handler_t                          p_onConnected);

  /* envoi */
  void do_send(utils::sharedBuf_t p_outData,
               utils::handler_t   p_onSent);
  void sendTimeout(const boost::system::error_code p_error);
  void onSent(const boost::system::error_code           p_error,
              utils::sharedBuf_t                        p_outData,
              boost::shared_ptr<utils::deadLineTimer_t> p_timer,
              utils::handler_t                          p_onSent);


  /* reception */
  void do_receive(utils::sharedBuf_t p_inData,
                  utils::handler_t   p_onReceived);
  void receiveTimeout(const boost::system::error_code p_error);
  void onReceived(const boost::system::error_code           p_error,
                  boost::shared_ptr<utils::deadLineTimer_t> p_timer,
                  utils::handler_t                          p_onReceived);

  /**
   ** @details
   ** Mets à jour les options de la socket (keepalive, linger.. etc) et
   ** extrait les informations sur la point de connexion distant (host/port).
   **
   ** Cette routine doit être appelée APRES s'être connecté à la machine
   ** distante, c'est à dire lorsque la socket à été ouverte.
   */
  status setSocketInfo(void);

  void setSocketOptions(void);
  void readEndpoint(void);

protected:
  virtual void async_write(utils::sharedBuf_t p_outData,    utils::handler_t p_onSent)     = 0;
  virtual void async_read(utils::sharedBuf_t  p_inDataData, utils::handler_t p_onReceived) = 0;

protected:
  utils::Config         m_conf;
  boost::asio::io_service&                 m_ioService;
  boost::asio::strand                      m_strand;
  boost::asio::basic_stream_socket<Domain> m_socket;

private:
  string   m_hostname;
  uint32_t m_port;
  string   m_remoteAddr;
  uint32_t m_remotePort;
  string   m_localAddr;
  uint32_t m_localPort;
  uint32_t m_processID;
  string   m_info;
};


template <typename Domain>
void
Connection<Domain>::setProcessID(uint32_t p_processID)
{
  m_processID = p_processID;
}

template <typename Domain>
uint32_t
Connection<Domain>::getProcessID(void) const
{
  return m_processID;
}

}}} // end namespaces


#endif /* NETWORK_BASE_CONNECTION_HH_ */
