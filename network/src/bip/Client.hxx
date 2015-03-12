#ifndef NETWORK_BIP_CLIENT_HXX_
# define NETWORK_BIP_CLIENT_HXX_

# include <boost/interprocess/sync/lock_options.hpp>
# include <boost/interprocess/detail/atomic.hpp>
# include <boost/shared_ptr.hpp>
# include <boost/make_shared.hpp>
# include <boost/iostreams/filtering_stream.hpp>
# include <boost/iostreams/filter/zlib.hpp>
# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filter/bzip2.hpp>
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
  m_status(cnxstatus::available),
  m_response(),
  m_sendTotal(0),
  m_sendError(0),
  m_sendSuccess(0),
  m_sendTimeout(0),
  m_receiveTotal(0),
  m_receiveError(0),
  m_receiveTimeout(0),
  m_receiveSuccess(0),
  m_lastRTTMs(0)
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
  atom::atomic_inc32(&m_sendTotal);

  utils::vectorBytes_t l_buff;

  if (m_status != cnxstatus::available)
  {
    logger::err("network.bip.client", "bip client send (%s) : can't send multiple request without receive", TBase::m_connection->info(), HERE);
    atom::atomic_inc32(&m_sendError);
    return status::error;
  }


  if (status::ok != saveCompress<serializer::mode::bin>(TBase::m_conf, p_request, p_debug, l_buff))
  {
    logger::crit("network.bip.client", "Error while serializing request (%s)", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::error;
    atom::atomic_inc32(&m_sendError);
    return status::error;
  }

  m_lastSend = boost::posix_time::microsec_clock::local_time();
  m_status   = cnxstatus::reserved;

  bool l_shouldReceive = shouldReceive(p_request, p_debug);

  boost::shared_ptr<Connection<TDomain> > l_conn =
    boost::static_pointer_cast<Connection<TDomain> >(TBase::m_connection);

  l_conn->incCounter();

  TBase::m_connection->send(l_buff,
                            boost::bind(&Client::onSent,
                                        this,
                                        _1,
                                        l_shouldReceive));

  if (true == l_shouldReceive)
  {
    atom::atomic_inc32(&m_sendSuccess);
    return status::ok;
  }

  // Lock until the send have been done
  utils::do_sem_wait(m_userSemaphore);

  if (m_status == cnxstatus::sent)
  {
    m_status = cnxstatus::available;
    atom::atomic_inc32(&m_sendSuccess);
    return status::ok;
  }

  if (m_status == cnxstatus::timeout)
  {
    logger::err("network.bip.client", "bip client send : timeout error", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&m_sendTotal);
    return status::timeout;
  }

  logger::err("network.bip.client", "bip client send : send error", HERE);
  m_status = cnxstatus::available;
  atom::atomic_inc32(&m_sendError);
  return status::error;
}




template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::do_receive(void)
{
  logger::debug("network.bip.client", "bip client do_receive (%s) : entering", TBase::m_connection->info(), HERE);

  utils::sharedBuf_t l_res = boost::make_shared<utils::vectorBytes_t>();

  TBase::m_connection->receive(l_res,
                               boost::bind(&Client::onReceived,
                                           this,
                                           _1,
                                           l_res));

  logger::debug("network.bip.client", "bip client do_receive (%s) : leaving", TBase::m_connection->info(), HERE);
}

template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::onSent(const boost::system::error_code p_error, bool p_shouldReceive)
{
  logger::debug("network.bip.client", "bip client onSent (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error == boost::asio::error::operation_aborted)
  {
    logger::err("network.bip.client", "bip client onSent (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    m_userSemaphore.post();
    return;
  }

  if (p_error)
  {
    logger::err("network.bip.client", "bip client onSent (%s) : send failed with error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    m_userSemaphore.post();
    return;
  }

  m_status = cnxstatus::sent;

  if (true == p_shouldReceive)
  {
    do_receive();

    logger::debug("network.bip.client", "bip client onSent (%s) : leaving", TBase::m_connection->info(), HERE);
  }
  else
  {
    // Log before unlocking the send because the connection can be released before logging
    logger::debug("network.bip.client", "bip client onSent (%s) : leaving", TBase::m_connection->info(), HERE);

    // Send is done so unlock the send
    m_userSemaphore.post();
  }
}


template<class TReq, class TRes, typename TDomain>
void
Client<TReq, TRes, TDomain>::onReceived(const boost::system::error_code p_error,
                                        utils::sharedBuf_t              p_response)
{
  utils::scoped_method l_semPos(boost::bind(&boost::interprocess::interprocess_semaphore::post, boost::ref(m_userSemaphore)));

  logger::debug("network.bip.client", "bip client onReceived (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error ==  boost::asio::error::operation_aborted)
  {
    logger::err("network.bip.client", "bip client onReceived (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    return;
  }

  if (p_error)
  {
    logger::err("network.bip.client", "bip client onReceived (%s) : error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  if (m_status != cnxstatus::sent)
  {
    logger::err("network.bip.client", "bip client onReceived (%s) : request has invalid status '%d'", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  m_status = cnxstatus::received;
  m_response.swap(*p_response);

  boost::posix_time::time_duration l_diff =
    boost::posix_time::microsec_clock::local_time() - m_lastSend;

  m_lastRTTMs = l_diff.total_milliseconds();
  logger::debug("network.bip.client", "bip onReceived (%s) : client received finished", TBase::m_connection->info(), HERE);
}


template<class TReq, class TRes, typename TDomain>
status
Client<TReq, TRes, TDomain>::receive(TRes& p_response,
                                     bool& p_debug)
{
  logger::debug("network.bip.client", "bip client receive (%s) : entering", TBase::m_connection->info(), HERE);
  atom::atomic_inc32(&m_receiveTotal);

  if (m_status == cnxstatus::available)
  {
    logger::err("network.bip.client", "bip client receive : unsent request", HERE);
    atom::atomic_inc32(&m_receiveError);
    return status::error;
  }

  utils::do_sem_wait(m_userSemaphore);

  if (m_status == cnxstatus::timeout)
  {
    logger::err("network.bip.client", "bip client receive : timeout error", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&m_receiveTimeout);
    return status::timeout;
  }

  if (m_status != cnxstatus::received)
  {
    logger::err("network.bip.client", "bip client receive : receive error (status '%d')", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&m_receiveError);
    return status::error;
  }


  if (status::ok != loadCompress<serializer::mode::bin>(TBase::m_conf, m_response, p_response, p_debug))
  {
    logger::crit("network.bip.client", "bip client receive (%s) : can't deserialize structure", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&m_receiveError);
    return status::error;
  }

  atom::atomic_inc32(&m_receiveSuccess);
  m_status = cnxstatus::available;
  logger::debug("network.bip.client", "bip client receive (%s) : leaving", TBase::m_connection->info(), HERE);
  return status::ok;
}

template<typename TRequest, typename TResponse, typename TDomain>
bool
Client<TRequest, TResponse, TDomain>::shouldReceive(const TRequest&  /*p_request*/,
                                                    const bool   /*p_requestDebug*/)
{
  return true;
}


}}} //end namespaces

#endif // !NETWORK_BIP_CLIENT_HH_
