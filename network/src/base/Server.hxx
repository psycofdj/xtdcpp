#ifndef NETWORK_BASE_SERVER_HXX_
# define NETWORK_BASE_SERVER_HXX_

# include <iostream>
# include <iterator>
# include <string>
# include <boost/asio.hpp>
# include <boost/asio/ip/host_name.hpp>
# include <boost/thread/thread.hpp>
# include <boost/noncopyable.hpp>
# include <memory>
# include <boost/make_shared.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/interprocess/detail/atomic.hpp>
# include <log.hh> // libcore

# include "utils/Resolver.hh"
# include "base/Connection.hh"

namespace xtd {
namespace network {
namespace base {

/**
 ** On se permet les using namespace dans le hxx car on suppose
 ** ce header prive et l'instanciation explicite par Server.cc
 */

using namespace boost;


namespace ba     = boost::asio;
namespace bt     = boost::posix_time;
namespace bs     = boost::system;
namespace atomic = boost::interprocess::ipcdetail;

template<typename Domain>
Server<Domain>::Server(void) :
  m_conf(),
  m_dequeId(),
  m_ioService(),
  m_work(),
  m_acceptor(),
  m_resolver(),
  m_threadNb(0),
  m_threadGroup(),
  // counters
  m_nbCurrentThread(0),
  m_cnxTotal(0),
  m_cnxAccepted(0),
  m_cnxRejected(0),
  m_receiveTotal(0),
  m_receiveError(0),
  m_receiveTimeout(0),
  m_receiveSuccess(0),
  m_receivedLastTime(0),
  m_sendTotal(0),
  m_sendError(0),
  m_sendSuccess(0),
  m_sendTimeout(0)
{
}



template<typename Domain>
Server<Domain>::~Server(void)
{
  m_acceptor.reset();
  m_resolver.reset();
  m_work.reset();
  m_ioService.reset();
}



/**
 ** @details
 ** 1. mettre l'objet "work" en ecoute sur le boost::asio::io_service a pour
 **    effet de simuler un job qui ne termine jamais. Les threads
 **    restent bloquées dans leur appel à boost::io_service::run
 */
template<typename Domain>
void
Server<Domain>::initialize(const string&        p_host,
                           const uint32_t       p_port,
                           const utils::Config& p_conf,
                           const size_t         p_nbThread)
{
  m_conf     = p_conf;
  m_threadNb = p_nbThread;

  m_ioService.reset(new ba::io_service());
  // 1.
  m_work.reset(new ba::io_service::work(*m_ioService));
  m_resolver.reset(new utils::Resolver<Domain>(*m_ioService, m_conf.getDnsCacheTTL()));

  typename Domain::endpoint l_endpoint =
    m_resolver->resolve(p_host, lexical_cast<string>(p_port));

  m_acceptor.reset(new ba::basic_socket_acceptor<Domain>(*m_ioService, l_endpoint, p_conf.getReuseAddr()));
}

template<typename Domain>
const typename Domain::endpoint
Server<Domain>::getEndPoint(void) const
{
  return m_acceptor->local_endpoint();
}


/**
 ** @details
 ** 1. On lance le premier async_accept uniquement apres avoir enregistre
 **    les "workers" (cad les thread qui bind io_server::run) aupres de
 **    l'io_service
 */
template<typename Domain>
void
Server<Domain>::start(void)
{
  log::info("network.base.server", "starting Server threads",  HERE);

  //!\ WARNING /!\\ : +1 is needed to have a free slot to refuse a request
  for (size_t i = 0; i < m_threadNb+1; ++i)
    m_threadGroup.create_thread([this](void) {
        m_ioService->run();
      });

  for (size_t i = 0; i < m_threadNb; ++i)
    m_dequeId.push_back(i);

  // 1.
  accept();
}


template<typename Domain>
void
Server<Domain>::join(void)
{
  log::info("network.base.server", "joining Server thread",  HERE);
  m_threadGroup.join_all();
  log::info("network.base.server", "stopping network::base server...done",  HERE);
}


template<typename Domain>
void
Server<Domain>::run(void)
{
  start();
  join();
}


template<typename Domain>
void
Server<Domain>::stop(void)
{
  log::info("network.base.server", "stopping network::base server...",  HERE);
  m_ioService->stop();
}


/**
 ** @details
 ** createCnx va construire un objet spécialisé qui dérive de Connection.
 ** cet objet sera alors transmis en paramètre sur toutes les callbacks de la châine
 ** de traitement. Lorsque la dernière callback aura terminé, le compteur de référence
 ** du shared_ptr sera à 0 et l'objet sera automatiquement détruit
 */
template<typename Domain>
void
Server<Domain>::accept(void)
{
  log::debug("network.base.server", "accept : enterring, creating new cnx",  HERE);

  cnx_sptr_t l_conn = createCnx(m_resolver->getAddr(m_acceptor->local_endpoint()),
                                m_resolver->getPort(m_acceptor->local_endpoint()));
  l_conn->accept(m_acceptor, bind(&Server::onAccepted, this, _1, l_conn));

  log::debug("network.base.server", "accept : leaving",  HERE);
}


/**
 ** @details
 ** 1. On utilise ici un objet qui copie le fonctionnement des std::mutex::scoped_lock
 **    Au constructeur, on lui donne une fonction. Lorsque l'objet sera detruit,
 **    il executera cette fonction.
 **    Ici, on veut garantir que la methode accept doit appelée, quelque soit
 **    la façon dont sort la fontion.
 **
 ** 2. On donne le processID a la connexion par facilite, cela nous evite
 **    de trimpaller le parametre sur toutes les callbacks.
 **
 ** 3. On laisse la classe fille décider de ce qu'il convient de faire après
 **    avoir accepté la connexion entrante.
 **    Si la méthode afterAccept retourne false, cela signifie que
 **    l'on ne souhaite pas poursuivre la transaction, donc on libère
 **    le processID
 */
template<typename Domain>
void
Server<Domain>::onAccepted(const bs::error_code p_error,
                           cnx_sptr_t           p_conn)
{
  log::debug("network.base.server", "onAccepted (%s) : entering",  p_conn->info(), HERE);

  // 1.
  utils::scoped_method l_acceptOnExit(bind(&Server::accept, this));


  atomic::atomic_inc32(&m_cnxTotal);

  if (p_error)
  {
    atomic::atomic_inc32(&m_cnxRejected);
    log::err("network.base.server", "onAccepted (%s) : error '%s'",  p_conn->info(), p_error.message(), HERE);
    return;
  }

  atomic::atomic_inc32(&m_cnxAccepted);
  log::info("network.base.server", "onAccepted (%s) : connection accepted",  p_conn->info(), HERE);

  // 3.
  afterAccept(p_conn);

  log::debug("network.base.server", "onAccepted : leaving",  HERE);
}


template<typename Domain>
void
Server<Domain>::do_receive(cnx_sptr_t p_conn)
{
  log::debug("network.base.server", "do_receive (%s) : entering",  p_conn->info(), HERE);

  sptr<vector<char>> l_inBuffer = std::make_shared<vector<char>>();

  p_conn->receive(l_inBuffer,
                  bind(&Server::onReceived,
                       this,
                       _1,
                       p_conn,
                       l_inBuffer));

  log::debug("network.base.server", "do_receive (%s) : leaving",  p_conn->info(), HERE);
}


template <typename Domain>
void
Server<Domain>::onReceiveError(const bs::error_code p_error, cnx_sptr_t p_conn)
{
  log::err("network.base.server", "onReceiveError (%s) : %s",  p_conn->info(), p_error.message(), HERE);
  atomic::atomic_inc32(&m_receiveTotal);
  atomic::atomic_inc32(&m_receiveError);
}


template <typename Domain>
void
Server<Domain>::onReceiveTimeout(const bs::error_code p_error, cnx_sptr_t p_conn)
{
  log::info("network.base.server", "onReceiveTimeout (%s) : %s",  p_conn->info(), p_error.message(), HERE);
  atomic::atomic_inc32(&m_receiveTotal);
  atomic::atomic_inc32(&m_receiveTimeout);
}


/**
 ** @details
 ** 1. Dans la première implémentation, la libération du processID se faisait
 **    après l'envoi de la réponse du serveur dans le onSent. On a constater que
 **    selon le hasard du scheduling des threads par le noyeau, il était
 **    possible que le client ait le temps de faire son receive, son close et un autre
 **    reconnecte avant l'exécution du onSent. Cela aboutissait à une famine de processID
 **    et donc à une erreur.
 **
 **    On a donc décidé de libérer le processID après réception de la requête puisque,
 **    de toute façon, il ne sert à rien pour l'envoi de la réponse.
 **
 **    Le protocole pouvant décider de recycler la connexion pour faire plusieurs receive,
 **    elle peut contrôler la libération du processID en retournant false dans son
 **    afterReceive.
 */
template <typename Domain>
void
Server<Domain>::onReceived(const bs::error_code p_error,
                           cnx_sptr_t           p_conn,
                           sptr<vector<char>>   p_inBuffer)
{

  bt::ptime         l_beginTime = bt::microsec_clock::local_time();
  bt::time_duration l_diffTime;
  uint32_t          l_durationMs;
  uint32_t          l_processID;

  log::debug("network.base.server", "onReceived (%s) : entering",  p_conn->info(), HERE);

  if (p_error == ba::error::operation_aborted)
  {
    onReceiveTimeout(p_error, p_conn);
    return;
  }

  if (p_error)
  {
    onReceiveError(p_error, p_conn);
    return;
  }

  atomic::atomic_inc32(&m_receiveTotal);
  atomic::atomic_inc32(&m_receiveSuccess);

  if (false == m_dequeId.pop(l_processID))
  {
    atomic::atomic_inc32(&m_receiveError);
    log::crit("network.base.server", "onReceived (%s) : no processID available",  p_conn->info(), HERE);
    return;
  }

  p_conn->setProcessID(l_processID);
  atomic::atomic_inc32(&m_nbCurrentThread);

  afterReceive(p_conn, p_inBuffer);

  m_dequeId.push(l_processID);

  atomic::atomic_dec32(&m_nbCurrentThread);

  l_diffTime   = bt::microsec_clock::local_time() - l_beginTime;
  l_durationMs = l_diffTime.total_milliseconds();
  atomic::atomic_write32(&m_receivedLastTime, l_durationMs);
  log::debug("network.base.server", "onReceived (%s) : leaving",  p_conn->info(), HERE);
}


template<typename Domain>
void
Server<Domain>::do_send(cnx_sptr_t          p_conn,
                        const vector<char>& p_outData)
{
  log::debug("network.base.server", "do_send (%s) : entering",  p_conn->info(), HERE);

  p_conn->send(p_outData, bind(&Server::onSent, this, _1, p_conn));

  log::debug("network.base.server", "do_send (%s) : leaving",  p_conn->info(), HERE);
}


template<typename Domain>
void
Server<Domain>::onSent(const bs::error_code p_error,
                       cnx_sptr_t           p_conn)
{
  log::debug("network.base.server", "onSent (%s) : entering",  p_conn->info(), HERE);
  atomic::atomic_inc32(&m_sendTotal);

  if (p_error == ba::error::operation_aborted)
  {
    log::err("network.base.server", "onSent (%s) : receive canceled, probably due to timeout",  p_conn->info(), HERE);
    atomic::atomic_inc32(&m_sendTimeout);
    return;
  }

  if (p_error)
  {
    log::err("network.base.server", "onSent (%s) : error '%s'",  p_conn->info(), p_error.message(), HERE);
    atomic::atomic_inc32(&m_sendError);
    return;
  }

  afterSend(p_conn);
  atomic::atomic_inc32(&m_sendSuccess);
  log::debug("network.base.server", "onSent (%s) : leaving",  p_conn->info(), HERE);
}





}}} // end namespaces


#endif // !NETWORK_BASE_SERVER_HXX_
