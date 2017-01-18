#ifndef NETWORK_BASE_CLIENTPOOL_HXX_
# define NETWORK_BASE_CLIENTPOOL_HXX_
# include <boost/interprocess/detail/atomic.hpp>
# include <log.hh>   // libcore

namespace xtd {
namespace network {
namespace base {


template<typename TClient>
ClientPool<TClient>::PersistentClient::PersistentClient(const string&  p_hostname,
                                                        const uint32_t p_port):
  TClient(),
  m_hostname(p_hostname),
  m_port(p_port),
  m_isConnected(false),
  m_lastUsed(boost::posix_time::microsec_clock::universal_time())
{
}


template<typename TClient>
ClientPool<TClient>::PersistentClient::~PersistentClient(void)
{
}


template<typename TClient>
void
ClientPool<TClient>::PersistentClient::invalidate(void)
{
  log::err("network.base.clientpool", "invalidating client", HERE);
  m_lastUsed = boost::posix_time::ptime(boost::posix_time::min_date_time);
}


template<typename TClient>
void
ClientPool<TClient>::PersistentClient::async_connect(void)
{
  if (false == m_isConnected)
  {
    TClient::async_connect(m_hostname, m_port);
  }
}


template<typename TClient>
status
ClientPool<TClient>::PersistentClient::wait_async_connect(void)
{
  status l_status = status::ok;

  if (false == m_isConnected)
  {
    l_status = TClient::wait_async_connect();
    if (status::ok == l_status)
      m_isConnected = true;
    else
    {
      log::err("network.base.clientpool", "can't connect (%s:%d)", m_hostname, m_port, HERE);
      invalidate();
    }
  }

  return l_status;
}



/**
 ** @details
 ** 1. connect client if first send, invalidate if error
 ** 2. send request, invalidate if error
 */
template<typename TClient>
template<typename... Args>
status
ClientPool<TClient>::PersistentClient::send(Args&&... p_args)
{
  status l_status;

  // 1.
  if (status::ok != (l_status = wait_async_connect()))
    return l_status;

  // 2.
  l_status = TClient::send(p_args...);
  if (status::ok != l_status)
  {
    log::err("network.base.clientpool", "can't connect (%s:%d)", m_hostname, m_port, HERE);
    invalidate();
  }

  return l_status;
}

template<typename TClient>
template<typename... Args>
status
ClientPool<TClient>::PersistentClient::receive(Args&&... p_args)
{
  status l_status;

  l_status    = TClient::receive(p_args...);
  m_lastUsed  = boost::posix_time::microsec_clock::universal_time();

  if (status::ok != l_status)
  {
    log::err("network.base.clientpool", "can't receive", HERE);
    invalidate();
  }

  return l_status;
}


template<typename TClient>
bool
ClientPool<TClient>::PersistentClient::isObsolete(uint32_t p_ttlMs) const
{
  boost::posix_time::time_duration l_diffTime;
  uint32_t                     l_durationMs;

  l_diffTime   = boost::posix_time::microsec_clock::universal_time() - m_lastUsed;
  l_durationMs = l_diffTime.total_milliseconds();
  return l_durationMs > p_ttlMs;
}


/* --------------------------------------------------------- */


template<typename TClient>
ClientPool<TClient>::ClientPool(const string&  p_hostname,
                                const uint32_t p_port,
                                const uint32_t p_ttlMs):
  m_hostname(p_hostname),
  m_port(p_port),
  m_ttlMs(p_ttlMs),
  m_sendTotal(0),
  m_sendSuccess(0),
  m_sendError(0),
  m_sendTimeout(0),
  m_rcvTotal(0),
  m_rcvSuccess(0),
  m_rcvError(0),
  m_rcvTimeout(0),
  m_curNbClient(0),
  m_acquiredClients(0),
  m_recycleHit(0),
  m_recycleMiss(0)
{
}

/**
 ** @details
 ** 1. evacuation des clients perimés
 */
template<typename TClient>
typename ClientPool<TClient>::client_type
ClientPool<TClient>::acquire(void)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  client_type                 l_obj;

  // 1.
  m_available.erase(std::remove_if(m_available.begin(), m_available.end(), std::bind(&PersistentClient::isObsolete, std::placeholders::_1, m_ttlMs)), m_available.end());

  if (false == m_available.empty())
    m_recycleHit++;
  else
  {
    l_obj.reset(new PersistentClient(m_hostname, m_port));
    l_obj->async_connect();
    m_available.push_back(l_obj);
    m_recycleMiss++;
  }

  l_obj = m_available.front();
  m_available.pop_front();
  m_acquiredClients += 1;
  m_curNbClient      = m_acquiredClients + m_available.size();

  return l_obj;
}


template<typename TClient>
void
ClientPool<TClient>::release(client_type& p_client)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);

  m_available.push_front(p_client);
  m_acquiredClients  -= 1;
  m_curNbClient       = m_acquiredClients + m_available.size();
}

template<typename TClient>
template<typename... Args>
status
ClientPool<TClient>::send(client_type& p_client, Args&&... p_args)
{
  status l_status = p_client->send(p_args...);

  boost::interprocess::ipcdetail::atomic_inc32(&m_sendTotal);
  if (l_status == status::timeout)
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendTimeout);
  else if (l_status == status::error)
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendError);
  else
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendSuccess);
  return l_status;
}


template<typename TClient>
template<typename... Args>
status
ClientPool<TClient>::receive(client_type& p_client, Args&&... p_args)
{
  status l_status = p_client->receive(p_args...);

  boost::interprocess::ipcdetail::atomic_inc32(&m_rcvTotal);
  if (l_status == status::timeout)
    boost::interprocess::ipcdetail::atomic_inc32(&m_rcvTimeout);
  else if (l_status == status::error)
    boost::interprocess::ipcdetail::atomic_inc32(&m_rcvError);
  else
    boost::interprocess::ipcdetail::atomic_inc32(&m_rcvSuccess);
  return l_status;
}


}}}


#endif // !NETWORK_BASE_CLIENTPOOL_HXX_
