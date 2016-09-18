#ifndef NETWORK_BASE_CONNECTION_HXX_
# define NETWORK_BASE_CONNECTION_HXX_

# include <types.hh> // libcommon
# include <logger.hh>   // libcommon
# include <boost/format.hpp>
# include <memory>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/algorithm/string/replace.hpp>

namespace xtd {
namespace network {
namespace base {


/**
 ** @details
 ** Header privé (utilisé uniquement par base/Connection.cc), on se permet de
 ** jouer avec les namespaces.
 */
using namespace boost;
using namespace xtd;

namespace       ba = boost::asio;
namespace       bs = boost::system;



template <typename Domain>
Connection<Domain>::Connection(const utils::Config& p_conf,
                               ba::io_service&                         p_ioService,
                               const string                       p_hostname,
                               const uint32_t                     p_port) :
  m_conf(p_conf),
  m_ioService(p_ioService),
  m_strand(p_ioService),
  m_socket(p_ioService),
  m_hostname(p_hostname),
  m_port(p_port),
  m_remoteAddr(),
  m_remotePort(0),
  m_localAddr(),
  m_localPort(0),
  m_processID(0)
{
}



template <typename Domain>
Connection<Domain>::~Connection(void)
{
  close();
}


/**
 ** @details
 ** 1. on ne log pas "info()", la donnée n'est pas encore disponible
 */
template <typename Domain>
void
Connection<Domain>::accept(std::shared_ptr<ba::basic_socket_acceptor<Domain> > p_acceptor,
                           utils::handler_t                                      p_onAccepted)
{
  logger::debug("network.base.cnx", "cnx accept : entering", HERE);

  m_strand.post(bind(&Connection::do_accept,
                     this->shared_from_this(),
                     p_acceptor,
                     p_onAccepted));

  logger::debug("network.base.cnx", "cnx accept : leaving", HERE);
}



/**
 ** @details
 ** 1. on ne log pas "info()", la donnee n'est pas encore disponible
 */
template <typename Domain>
void
Connection<Domain>::do_accept(std::shared_ptr<ba::basic_socket_acceptor<Domain> > p_acceptor,
                              utils::handler_t                                      p_onAccepted)
{
  logger::debug("network.base.cnx", "cnx do_accept : entering", HERE);

  m_localAddr = m_hostname;
  m_localPort = m_port;

  p_acceptor->async_accept(m_socket,
                           bind(&Connection::onAccepted,
                                this->shared_from_this(),
                                _1,
                                p_acceptor,
                                p_onAccepted));

  logger::debug("network.base.cnx", "cnx do_accept : leaving", HERE);
}



/**
 ** @details
 ** 1. on ne log pas "info()", la donnee n'est pas encore disponible
 */
template <typename Domain>
void
Connection<Domain>::onAccepted(bs::error_code                                           p_error,
                               std::shared_ptr<ba::basic_socket_acceptor<Domain> > /* p_acceptor */,
                               utils::handler_t                                         p_onAccepted)
{
  logger::debug("network.base.cnx", "cnx onAccepted : entering", HERE);

  // 1. Set socket informations
  status l_ret = setSocketInfo();

  // 2. Post handler on strand

  if (p_error)
    m_strand.post(bind(p_onAccepted, p_error));
  else if (status::ok != l_ret)
    m_strand.post(bind(p_onAccepted, ba::error::bad_descriptor));
  else
    m_strand.post(bind(p_onAccepted, p_error));

  logger::debug("network.base.cnx", "cnx onAccepted (%s) : leaving", info(), HERE);
}


/**
 ** @details
 ** 1. on ne log pas "info()", la donnee n'est pas encore disponible
 */
template <typename Domain>
void
Connection<Domain>::connect(std::shared_ptr<utils::Resolver<Domain> > p_resolver,
                            utils::handler_t                            p_onConnected)
{
  logger::debug("network.base.cnx", "cnx connect (%s:%d): entering", m_hostname, m_port, HERE);

  m_strand.post(bind(&Connection::do_connect,
                     this->shared_from_this(),
                     p_resolver,
                     p_onConnected));

  logger::debug("network.base.cnx", "cnx connect (%s:%d): leaving", m_hostname, m_port, HERE);
}


/**
 ** @details
 ** 1. on ne log pas "info()", la donnee n'est pas encore disponible
 */
template <typename Domain>
void
Connection<Domain>::do_connect(std::shared_ptr<utils::Resolver<Domain> > p_resolver,
                               utils::handler_t                            p_onConnected)
{
  logger::debug("network.base.cnx", "cnx do_connect (%s:%d): entering", m_hostname, m_port, HERE);

  m_remoteAddr = m_hostname;
  m_remotePort = m_port;

  typename Domain::endpoint                 l_endpoint = p_resolver->resolve(m_hostname, boost::lexical_cast<string>(m_port));
  std::shared_ptr<utils::deadLineTimer_t> l_timer(new utils::deadLineTimer_t(m_ioService));


  l_timer->expires_from_now(posix_time::milliseconds(m_conf.getConnectTimeoutMs()));
  l_timer->async_wait(m_strand.wrap(bind(&Connection::connectTimeout,
                                         this->shared_from_this(),
                                         _1)));

  m_socket.async_connect(l_endpoint,
                         m_strand.wrap(bind(&Connection::onConnected,
                                            this->shared_from_this(),
                                            _1,
                                            l_timer,
                                            p_onConnected)));

  logger::debug("network.base.cnx", "cnx do_connect (%s:%d): leaving", m_hostname, m_port, HERE);
}


/**
 ** @details
 **
 ** Une p_error == ba::error::operation_aborted signifie que
 ** l'async_wait du timer à été annulé (p_time->cancel).
 **
 ** Dans le cas contraire, cette callback cancel tous les evenements en
 ** attente. (Ici normalement un seul, le async_connect). Cela aura pour
 ** effet d'appeler sa callback avec l'erreur ba::error::operation_aborted
 */
template <typename Domain>
void
Connection<Domain>::connectTimeout(const bs::error_code p_error)
{
  logger::debug("network.base.cnx", "cnx connectTimeout : entering", HERE);

  if (p_error != ba::error::operation_aborted)
  {
    logger::err("network.base.cnx", "cnx connectTimeout : timeout detected", HERE);
    cancel();
  }

  logger::debug("network.base.cnx", "cnx connectTimeout : leaving", HERE);
}




/**
 ** @details
 ** 1. p_error == ba::error::operation_aborted voudrait dire que le async_connect
 **    s'est fait cancel par la callback du timeout de connexion.
 **    Dans le cas contraire, la première chose à faire c'est d'annuler le timer.
 **
 ** 2. Si on a pas d'erreur, on tente initialiser les informations de la socket
 **    (appel à setSocketInfo). Cet appel nepeut se faire qu'une fois la connexion
 **    établie.
 **    Si cet appel échoue, on pirate un type d'erreur boost::asio pour le signifier
 **    à l'appelant.
 **
 **    Dans tous les cas, on appelle p_onConnected.
 */
template <typename Domain>
void
Connection<Domain>::onConnected(const bs::error_code                      p_error,
                                std::shared_ptr<utils::deadLineTimer_t> p_timer,
                                utils::handler_t                          p_onConnected)
{
  logger::debug("network.base.cnx", "cnx onConnected : entering", HERE);

  // 1. Cancel timer
  if (p_error != ba::error::operation_aborted)
  {
    logger::info("network.base.cnx", "cnx onConnected : no abort detected, cancelling timer", HERE);
    p_timer->cancel();
  }

  // 2. Set socket informations
  status l_ret = setSocketInfo();

  // 3. Post handler on strand
  if (p_error)
    m_strand.post(bind(p_onConnected, p_error));
  else if (status::ok != l_ret)
    m_strand.post(bind(p_onConnected, ba::error::bad_descriptor));
  else
    m_strand.post(bind(p_onConnected, p_error));

  logger::debug("network.base.cnx", "cnx onConnected (%s) : leaving", HERE);
}



/**
 ** @details
 ** 1. L'objet se charge de garantir la durée de vie de la donnée à envoyer.
 **    Pour cela, on copie le buffer d'entrée dans une sharedBuff_t que l'on va
 **    trimballer jusqu'à ce que l'envoi soit terminée.
 */
template <typename Domain>
void
Connection<Domain>::send(const utils::vectorBytes_t& p_outData,
                         utils::handler_t            p_onSent)
{
  logger::debug("network.base.cnx", "cnx send (%s) : entering", info(), HERE);

  // 1.
  utils::sharedBuf_t l_outBuff = std::make_shared<utils::vectorBytes_t>();

  l_outBuff->assign(p_outData.begin(), p_outData.end());

  m_strand.post(bind(&Connection::do_send,
                     this->shared_from_this(),
                     l_outBuff,
                     p_onSent));

  logger::debug("network.base.cnx", "cnx send (%s) : leaving", info(), HERE);
}



/**
 **
 */
template <typename Domain>
void
Connection<Domain>::do_send(utils::sharedBuf_t p_outData,
                            utils::handler_t   p_onSent)
{
  logger::debug("network.base.cnx", "cnx do_send (%s) : entering", info(), HERE);

  std::shared_ptr<utils::deadLineTimer_t> l_timer(new utils::deadLineTimer_t(m_ioService));

  l_timer->expires_from_now(posix_time::milliseconds(m_conf.getSendTimeoutMs()));
  l_timer->async_wait(m_strand.wrap(bind(&Connection::sendTimeout,
                                         this->shared_from_this(),
                                         _1)));

  utils::handler_t l_handler =
    bind(&Connection::onSent,
         this->shared_from_this(),
         _1,
         p_outData,
         l_timer,
         p_onSent);

  async_write(p_outData, l_handler);

  logger::debug("network.base.cnx", "cnx do_send (%s) : leaving", info(), HERE);
}


template <typename Domain>
void
Connection<Domain>::sendTimeout(const bs::error_code  p_error)
{
  logger::debug("network.base.cnx", "cnx sendTimeout (%s) : entering", info(), HERE);

  if (p_error != ba::error::operation_aborted)
  {
    logger::err("network.base.cnx", "cnx sendTimeout (%s) : timeout detected", info(), HERE);
    cancel();
  }

  logger::debug("network.base.cnx", "cnx sendTimeout (%s) : leaving", info(), HERE);
}


template <typename Domain>
void
Connection<Domain>::onSent(const bs::error_code                      p_error,
                           utils::sharedBuf_t                        p_outData,
                           std::shared_ptr<utils::deadLineTimer_t> p_timer,
                           utils::handler_t                          p_onSent)
{
  logger::debug("network.base.cnx", "cnx onSent (%s) : entering", info(), HERE);

  if (p_error != ba::error::operation_aborted)
  {
    logger::info("network.base.cnx", "cnx onSent (%s) : no abort detected, cancelling timer", info(), HERE);
    p_timer->cancel();
  }

  m_strand.post(bind(p_onSent, p_error));
  p_outData.reset();
  logger::debug("network.base.cnx", "cnx onSent (%s) : leaving", info(), HERE);
}




template <typename Domain>
void
Connection<Domain>::receive(utils::sharedBuf_t p_inData,
                            utils::handler_t   p_onReceived)
{
  logger::debug("network.base.cnx", "cnx receive (%s) : entering", info(), HERE);

  m_strand.post(bind(&Connection::do_receive,
                     this->shared_from_this(),
                     p_inData,
                     p_onReceived));

  logger::debug("network.base.cnx", "cnx receive (%s) : leaving", info(), HERE);
}



template <typename Domain>
void
Connection<Domain>::do_receive(utils::sharedBuf_t p_inData,
                               utils::handler_t   p_onReceived)
{
  logger::debug("network.base.cnx", "cnx do_receive (%s) : entering", info(), HERE);


  std::shared_ptr<utils::deadLineTimer_t> l_timer(new utils::deadLineTimer_t(m_ioService));

  l_timer->expires_from_now(posix_time::milliseconds(m_conf.getReceiveTimeoutMs()));
  l_timer->async_wait(m_strand.wrap(bind(&Connection::receiveTimeout,
                                         this->shared_from_this(),
                                         _1)));

  utils::handler_t l_handler
    = m_strand.wrap(bind(&Connection::onReceived,
                         this->shared_from_this(),
                         _1,
                         l_timer,
                         p_onReceived));

  async_read(p_inData, l_handler);

  logger::debug("network.base.cnx", "cnx do_receive (%s) : leaving", info(), HERE);
}




template <typename Domain>
void
Connection<Domain>::receiveTimeout(const bs::error_code p_error)
{
  logger::debug("network.base.cnx", "cnx receiveTimeout (%s) : entering", info(), HERE);

  if (p_error != ba::error::operation_aborted)
  {
    logger::debug("network.base.cnx", "cnx receiveTimeout (%s) : timeout detected", info(), HERE);
    cancel();
  }

  logger::debug("network.base.cnx", "cnx receiveTimeout (%s) : leaving", info(), HERE);
}




template <typename Domain>
void
Connection<Domain>::onReceived(const bs::error_code                      p_error,
                               std::shared_ptr<utils::deadLineTimer_t> p_timer,
                               utils::handler_t                          p_onReceived)
{
  logger::debug("network.base.cnx", "cnx onReceived (%s) : entering", info(), HERE);

  if (p_error != ba::error::operation_aborted)
  {
    logger::info("network.base.cnx", "cnx onReceived (%s) : no abort detected, cancelling timer", info(), HERE);
    p_timer->cancel();
  }

  m_strand.post(bind(p_onReceived, p_error));
  logger::debug("network.base.cnx", "cnx onReceived (%s) : leaving", info(), HERE);
}


template <typename Domain>
void
Connection<Domain>::cancel(void)
{
  bs::error_code l_error;
  m_socket.cancel(l_error);

  if (l_error)
    logger::err("network.base.cnx", "cnx (%s) : error on cancel : %s", info(), l_error.message(), HERE);
}


template <typename Domain>
void
Connection<Domain>::close(void)
{
  if (false == m_socket.is_open())
    return;

  bs::error_code l_error;

  m_socket.shutdown(ba::ip::tcp::socket::shutdown_both, l_error);
  if (l_error)
    logger::err("network.base.cnx", "cnx (%s) : error on shutdown : %s", info(), l_error.message(), HERE);

  m_socket.close(l_error);
  if (l_error)
    logger::err("network.base.cnx", "cnx (%s) : error on close : %s", info(), l_error.message(), HERE);
}


template <typename Domain>
const string&
Connection<Domain>::info(void) const
{
  return m_info;
}


template <typename Domain>
status
Connection<Domain>::setSocketInfo(void)
{
  status l_status = status::ok;

  if (m_socket.is_open())
  {
    try {
      setSocketOptions();
      readEndpoint();
    }
    catch (const boost::system::system_error& l_error)
    {
      logger::err("network.base.cnx", "setSocketInfo() : error while setting socket option '%s'", l_error.what(), HERE);
      l_status = status::error;
    }
  }
  else
  {
    logger::err("network.base.cnx", "setSocketInfo() : socket is yet closed!", HERE);
    l_status = status::error;
  }

  m_info = format::vargs("%s:%d -> %s:%d",
                         m_localAddr,
                         m_localPort,
                         m_remoteAddr,
                         m_remotePort);

  return l_status;
}





}}} // end namespaces

#endif // !NETWORK_BASE_CONNECTION_HXX_
