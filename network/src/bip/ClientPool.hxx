#ifndef NETWORK_BIP_CLIENTPOOL_HXX_
# define NETWORK_BIP_CLIENTPOOL_HXX_

# include <boost/range/algorithm/find_if.hpp>
# include <boost/interprocess/detail/atomic.hpp>

namespace atom = boost::interprocess::ipcdetail;


namespace xtd {
namespace network {

namespace bip {

template<class TRequest, class TResponse, typename TDomain>
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::PersistentClient(const string&        p_hostname,
                                                                             const uint32_t       p_port,
                                                                             const utils::Config& p_conf):
  bip::Client<TRequest, TResponse, TDomain>(p_conf),
  m_hostname(p_hostname),
  m_port(p_port),
  m_isConnected(false),
  m_lastUsed(boost::posix_time::microsec_clock::universal_time())
{
}


template<class TRequest, class TResponse, typename TDomain>
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::~PersistentClient(void)
{
}


template<class TRequest, class TResponse, typename TDomain>
void
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::invalidate(void)
{
  core::logger::err("network.bip.clientpool", "bip PersistentClient : invalidating client", HERE);
  m_lastUsed = boost::posix_time::ptime(boost::posix_time::min_date_time);
}


template<class TRequest, class TResponse, typename TDomain>
void
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::async_connect(void)
{
  if (false == m_isConnected)
  {
    bip::Client<TRequest, TResponse, TDomain>::async_connect(m_hostname, m_port);
  }
}


template<class TRequest, class TResponse, typename TDomain>
status
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::wait_async_connect(void)
{
  status l_status = status::ok;

  if (false == m_isConnected)
  {
    l_status = bip::Client<TRequest, TResponse, TDomain>::wait_async_connect();
    if (status::ok == l_status)
      m_isConnected = true;
    else
    {
      core::logger::err("network.bip.clientpool", "bip PersistentClient : can't connect (%s:%d)", m_hostname, m_port, HERE);
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
template<class TRequest, class TResponse, typename TDomain>
status
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::send(const TRequest& p_request, bool  p_debug)
{
  status l_status;

  // 1.
  if (status::ok != (l_status = wait_async_connect()))
    return l_status;

  // 2.
  l_status = bip::Client<TRequest, TResponse, TDomain>::send(p_request, p_debug);
  if (status::ok != l_status)
  {
    core::logger::err("network.bip.clientpool", "bip PersistentClient : can't connect (%s:%d)", m_hostname, m_port, HERE);
    invalidate();
  }

  return l_status;
}


template<class TRequest, class TResponse, typename TDomain>
status
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::receive(TResponse& p_response,  bool& p_debug)
{
  status l_status;

  l_status    = bip::Client<TRequest, TResponse, TDomain>::receive(p_response, p_debug);
  m_lastUsed  = boost::posix_time::microsec_clock::universal_time();

  if (status::ok != l_status)
  {
    core::logger::err("network.bip.clientpool", "bip PersistentClient : can't receive", HERE);
    invalidate();
  }

  return l_status;
}


template<class TRequest, class TResponse, typename TDomain>
bool
ClientPool<TRequest, TResponse, TDomain>::PersistentClient::isObsolete(uint32_t p_ttlMs) const
{
  boost::posix_time::time_duration l_diffTime;
  uint32_t                     l_durationMs;

  l_diffTime   = boost::posix_time::microsec_clock::universal_time() - m_lastUsed;
  l_durationMs = l_diffTime.total_milliseconds();
  return l_durationMs > p_ttlMs;
}


/* --------------------------------------------------------- */


template<class TRequest, class TResponse, typename TDomain>
ClientPool<TRequest, TResponse, TDomain>::ClientPool(const string&        p_hostname,
                                                     const uint32_t       p_port,
                                                     const utils::Config& p_conf,
                                                     const uint32_t       p_ttlMs):
  m_hostname( p_hostname),
  m_port(p_port),
  m_ttlMs(p_ttlMs),
  m_conf(p_conf),
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
template<class TRequest, class TResponse, typename TDomain>
typename ClientPool<TRequest, TResponse, TDomain>::t_client_sptr
ClientPool<TRequest, TResponse, TDomain>::acquire(void)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  t_client_sptr             l_obj;

  // 1.
  m_available.erase(std::remove_if(m_available.begin(), m_available.end(), boost::bind(&PersistentClient::isObsolete, _1, m_ttlMs)), m_available.end());

  if (false == m_available.empty())
    m_recycleHit++;
  else
  {
    l_obj.reset(new PersistentClient(m_hostname, m_port, m_conf));
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


template<class TRequest, class TResponse, typename TDomain>
void
ClientPool<TRequest, TResponse, TDomain>::release(t_client_sptr& p_client)
{
  boost::mutex::scoped_lock l_lock(m_mutex);

  m_available.push_front(p_client);
  m_acquiredClients  -= 1;
  m_curNbClient       = m_acquiredClients + m_available.size();
}


template<class TRequest, class TResponse, typename TDomain>
status
ClientPool<TRequest, TResponse, TDomain>::send(t_client_sptr& p_client, const TRequest& p_request, bool p_debug)
{
  status l_status = p_client->send(p_request, p_debug);

  boost::interprocess::ipcdetail::atomic_inc32(&m_sendTotal);
  if (l_status == status::timeout)
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendTimeout);
  else if (l_status == status::error)
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendError);
  else
    boost::interprocess::ipcdetail::atomic_inc32(&m_sendSuccess);
  return l_status;
}


template<class TRequest, class TResponse, typename TDomain>
status
ClientPool<TRequest, TResponse, TDomain>::receive(t_client_sptr& p_client, TResponse& p_response,  bool& p_debug)
{
  status l_status = p_client->receive(p_response, p_debug);

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


#endif // !NETWORK_BIP_CLIENTPOOL_HXX_
