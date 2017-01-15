#ifndef NETWORK_BIP_CLIENT_HXX_
# define NETWORK_BIP_CLIENT_HXX_

# include <boost/interprocess/sync/lock_options.hpp>
# include <boost/interprocess/detail/atomic.hpp>
# include <memory>
# include <boost/make_shared.hpp>
# include <boost/iostreams/filtering_stream.hpp>
# include <boost/iostreams/filter/zlib.hpp>
# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filter/bzip2.hpp>
# include <log.hh> // libcore
# include "bip/Connection.hh"
# include "bip/tools.hh"

namespace atom = boost::interprocess::ipcdetail;

namespace xtd {
namespace network {
namespace bip {


template<class TReq, class TRes, typename TDomain>
Client<TReq, TRes, TDomain>::Client(const utils::Config& p_conf) :
  TBase(p_conf),
  m_userSemaphore(0),
  m_status(TBase::cnxstatus::available),
  m_response()
{
}

template<class TReq, class TRes, typename TDomain>
Client<TReq, TRes, TDomain>::~Client(void)
{
}

template<class TReq, class TRes, typename TDomain>
typename Client<TReq, TRes, TDomain>::cnx_sptr_t
Client<TReq, TRes, TDomain>::createCnx(string p_hostname, uint32_t p_port)
{
  cnx_sptr_t l_result(new Connection<TDomain>(TBase::m_conf, TBase::m_ioService, p_hostname, p_port));
  return l_result;
}


template<class TReq, class TRes, typename TDomain>
status
Client<TReq, TRes, TDomain>::send(const TReq& p_request,
                                  bool        p_debug)
{
  atom::atomic_inc32(&(this->m_sendTotal));


  if (m_status != cnxstatus::available)
  {
    log::err("network.bip.client", "bip client send (%s) : can't send multiple request without receive", TBase::m_connection->info(), HERE);
    atom::atomic_inc32(&(this->m_sendError));
    return status::error;
  }

  utils::vectorBytes_t l_buff;

  if (status::ok != saveCompress<serializer::mode::bin>(TBase::m_conf, p_request, p_debug, l_buff))
  {
    log::crit("network.bip.client", "Error while serializing request (%s)", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::error;
    atom::atomic_inc32(&(this->m_sendError));
    return status::error;
  }

  m_lastSend = boost::posix_time::microsec_clock::local_time();
  m_status   = cnxstatus::reserved;

  bool                                  l_shouldReceive = shouldReceive(p_request, p_debug);
  std::shared_ptr<Connection<TDomain> > l_conn =
    std::static_pointer_cast<Connection<TDomain> >(TBase::m_connection);

  l_conn->incCounter();
  l_conn->send(l_buff,
               std::bind(&Client::onSent,
                         this,
                         std::placeholders::_1,
                         l_shouldReceive));

  if (true == l_shouldReceive)
  {
    atom::atomic_inc32(&(this->m_sendSuccess));
    return status::ok;
  }

  // Lock until the send have been done
  utils::do_sem_wait(m_userSemaphore);

  if (m_status == cnxstatus::sent)
  {
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_sendSuccess));
    return status::ok;
  }

  if (m_status == cnxstatus::timeout)
  {
    log::err("network.bip.client", "bip client send : timeout error", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_sendTotal));
    return status::timeout;
  }

  log::err("network.bip.client", "bip client send : send error", HERE);
  m_status = cnxstatus::available;
  atom::atomic_inc32(&(this->m_sendError));
  return status::error;
}




template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::do_receive(void)
{
  log::debug("network.bip.client", "bip client do_receive (%s) : entering", TBase::m_connection->info(), HERE);

  utils::sharedBuf_t l_res = std::make_shared<utils::vectorBytes_t>();

  TBase::m_connection->receive(l_res,
                               std::bind(&Client::onReceived,
                                         this,
                                         std::placeholders::_1,
                                         l_res));

  log::debug("network.bip.client", "bip client do_receive (%s) : leaving", TBase::m_connection->info(), HERE);
}

template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::onSent(const boost::system::error_code p_error, bool p_shouldReceive)
{
  log::debug("network.bip.client", "bip client onSent (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error == boost::asio::error::operation_aborted)
  {
    log::err("network.bip.client", "bip client onSent (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    m_userSemaphore.post();
    return;
  }

  if (p_error)
  {
    log::err("network.bip.client", "bip client onSent (%s) : send failed with error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    m_userSemaphore.post();
    return;
  }

  m_status = cnxstatus::sent;

  if (true == p_shouldReceive)
  {
    do_receive();

    log::debug("network.bip.client", "bip client onSent (%s) : leaving", TBase::m_connection->info(), HERE);
  }
  else
  {
    // Log before unlocking the send because the connection can be released before logging
    log::debug("network.bip.client", "bip client onSent (%s) : leaving", TBase::m_connection->info(), HERE);

    // Send is done so unlock the send
    m_userSemaphore.post();
  }
}


template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::onReceived(const boost::system::error_code p_error,
                                        utils::sharedBuf_t              p_response)
{
  utils::scoped_method l_semPos(std::bind(&boost::interprocess::interprocess_semaphore::post, std::ref(m_userSemaphore)));

  log::debug("network.bip.client", "bip client onReceived (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error ==  boost::asio::error::operation_aborted)
  {
    log::err("network.bip.client", "bip client onReceived (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    return;
  }

  if (p_error)
  {
    log::err("network.bip.client", "bip client onReceived (%s) : error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  if (m_status != cnxstatus::sent)
  {
    log::err("network.bip.client", "bip client onReceived (%s) : request has invalid status '%d'", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  m_status = cnxstatus::received;
  m_response.swap(*p_response);

  boost::posix_time::time_duration l_diff =
    boost::posix_time::microsec_clock::local_time() - m_lastSend;

  this->m_lastRTTMs = l_diff.total_milliseconds();
  log::debug("network.bip.client", "bip onReceived (%s) : client received finished", TBase::m_connection->info(), HERE);
}


template<class TReq, class TRes, typename TDomain>
status
Client<TReq, TRes, TDomain>::receive(TRes& p_response,
                                     bool& p_debug)
{
  log::debug("network.bip.client", "bip client receive (%s) : entering", TBase::m_connection->info(), HERE);
  atom::atomic_inc32(&(this->m_receiveTotal));

  if (m_status == cnxstatus::available)
  {
    log::err("network.bip.client", "bip client receive : unsent request", HERE);
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }

  utils::do_sem_wait(m_userSemaphore);

  if (m_status == cnxstatus::timeout)
  {
    log::err("network.bip.client", "bip client receive : timeout error", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_receiveTimeout));
    return status::timeout;
  }

  if (m_status != cnxstatus::received)
  {
    log::err("network.bip.client", "bip client receive : receive error (status '%d')", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }


  if (status::ok != loadCompress<serializer::mode::bin>(TBase::m_conf, m_response, p_response, p_debug))
  {
    log::crit("network.bip.client", "bip client receive (%s) : can't deserialize structure", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }

  atom::atomic_inc32(&(this->m_receiveSuccess));
  m_status = cnxstatus::available;
  log::debug("network.bip.client", "bip client receive (%s) : leaving", TBase::m_connection->info(), HERE);
  return status::ok;
}

template<typename TRequest, typename TResponse, typename TDomain>
bool
Client<TRequest, TResponse, TDomain>::shouldReceive(const TRequest&  /* p_request */,
                                                    const bool       /* p_requestDebug */)
{
  return true;
}


}}} //end namespaces

#endif // !NETWORK_BIP_CLIENT_HH_
