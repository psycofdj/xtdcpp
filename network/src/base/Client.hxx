#ifndef NETWORK_CLIENT_HXX_
# define NETWORK_CLIENT_HXX_

# include <types.hh> // libcore
# include <log.hh>   // libcore
# include "utils/scoped_fn.hh"
# include <boost/interprocess/detail/atomic.hpp>
# include "base/Connection.hh"
# include "base/ThreadManager.hh"


namespace atom = boost::interprocess::ipcdetail;

namespace xtd {
namespace network {
namespace base {

/**
 ** On se permet les using namespace dans le hxx car on suppose
 ** ce header prive et l'instanciation explicite par Client.cc
 */

namespace ba = boost::asio;
namespace bs = boost::system;



template <typename Domain>
Client<Domain>::Client(void) :
  Config(),
  m_threadManager(ThreadManager::get()),
  m_resolver(),
  m_hostname(),
  m_port(),
  m_semaphoreConnect(0),
  m_ioService(m_threadManager.getIoService()),
  m_connection(),
  m_connectStatus(status::error),
  m_cnxTotal(0),
  m_cnxSuccess(0),
  m_cnxTimeout(0),
  m_cnxError(0)
{
  m_resolver.reset(new utils::Resolver<Domain>(m_ioService, getDnsCacheTTL()));
}

template <typename Domain>
Client<Domain>::~Client(void)
{
}


/**
 ** @details
 ** delegue la gestion d'erreurs au onConnect (notament alrady conected)
 **
 */
template <typename Domain>
status
Client<Domain>::connect(const string&       p_hostname,
                        const uint32_t p_port)
{
  async_connect(p_hostname, p_port);

  return wait_async_connect();
}

template <typename Domain>
void
Client<Domain>::async_connect(const string&  p_hostname,
                              const uint32_t p_port)
{
  log::debug("network.base.client", "async_connect (%s:%d) : entering", p_hostname, p_port, HERE);

  atom::atomic_inc32(&m_cnxTotal);

  m_hostname = p_hostname;
  m_port     = p_port;

  m_connection = createCnx(m_hostname, m_port);
  m_connection->connect(m_resolver,
                        std::bind(&Client::onConnected, this, std::placeholders::_1));

  log::debug("network.base.client", "async_connect (%s:%d) : leaving", p_hostname, p_port, HERE);
}

template <typename Domain>
status
Client<Domain>::wait_async_connect(void)
{
  utils::do_sem_wait(m_semaphoreConnect);

  if (m_connectStatus == status::error)
    atom::atomic_inc32(&m_cnxError);
  else if (m_connectStatus == status::timeout)
    atom::atomic_inc32(&m_cnxTimeout);
  else
    atom::atomic_inc32(&m_cnxSuccess);

  return m_connectStatus;
}

template <typename Domain>
status
Client<Domain>::reconnect(void)
{
  close();

  return connect(m_hostname, m_port);
}

template <typename Domain>
void
Client<Domain>::onConnected(const bs::error_code p_error)
{
  // On libere la semaphore juste a la sortie de la fonction pour garantir l'exitence de la connection
  xtd::utils::scoped_fn l_semPos(std::bind(&boost::interprocess::interprocess_semaphore::post, std::ref(m_semaphoreConnect)));

  log::info("network.base.client", "onConnected (%s) : entering", m_connection->info(), HERE);

  if (p_error == ba::error::operation_aborted)
  {
    log::err("network.base.client", "onConnected (%s) : connection canceled", m_connection->info(), HERE);
    m_connectStatus = status::timeout;
    return;
  }

  if (p_error)
  {
    log::err("network.base.client", "onConnected (%s) : connection failed with error '%s'", m_connection->info(), p_error.message(), HERE);
    m_connectStatus = status::error;
    return;
  }

  m_connectStatus = status::ok;

  log::info("network.base.client", "onConnected (%s) : leaving", m_connection->info(), HERE);
}


template <typename Domain>
void
Client<Domain>::close(void)
{
  log::debug("network.base.client", "close (%s) : entering", m_connection->info(), HERE);
  m_connection->close();
  m_connection.reset();
  log::debug("network.base.client", "close : leaving", HERE);
}


}}} // end namespace


#endif // !NETWORK_CLIENT_HXX_
