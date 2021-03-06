#ifndef NETWORK_HTTP_CLIENT_HXX_
# define NETWORK_HTTP_CLIENT_HXX_
# include <boost/asio.hpp>
# include <boost/date_time/posix_time/posix_time_config.hpp>
# include <boost/date_time/posix_time/posix_time_types.hpp>
# include <boost/interprocess/creation_tags.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <boost/interprocess/sync/spin/condition.hpp>
# include <boost/iostreams/device/back_inserter.hpp>
# include <boost/iostreams/filtering_stream.hpp>
# include <boost/range/iterator_range_core.hpp>
# include <boost/system/error_code.hpp>
# include <utils/scoped_fn.hh>
# include "log/helpers.hh"  // libcore
# include "log/logtypes.hh" // libcore
# include "types.hh"        // libcore
# include "http/Client.hh"
# include "http/Connection.hh"
# include "http/Request.hh"
# include "http/Response.hh"
# include "utils/Utils.hh"

namespace atom = boost::interprocess::ipcdetail;

namespace xtd {
namespace network {
namespace http {


template<typename TDomain>
Client<TDomain>::Client(void) :
  TBase(),
  m_userSemaphore(0),
  m_status(cnxstatus::available),
  m_response()
{
}

template<typename TDomain>
Client<TDomain>::~Client(void)
{
}

template<typename TDomain>
typename Client<TDomain>::cnx_sptr_t
Client<TDomain>::createCnx(string p_hostname, uint32_t p_port)
{
  cnx_sptr_t l_result(new Connection<TDomain>(*this, TBase::m_ioService, p_hostname, p_port));
  return l_result;
}


template<typename TDomain>
status
Client<TDomain>::send(const Request& p_request)
{
  vector<char> l_buff;
  auto&                l_conn = TBase::m_connection;

  atom::atomic_inc32(&(this->m_sendTotal));

  if (m_status != cnxstatus::available)
  {
    log::err("network.http.client", "http client send (%s) : can't send multiple request without receive", TBase::m_connection->info(), HERE);
    atom::atomic_inc32(&(this->m_sendError));
    return status::error;
  }

  m_lastSend = boost::posix_time::microsec_clock::local_time();
  m_status   = cnxstatus::reserved;

  {
    boost::iostreams::filtering_ostream l_stream;
    l_stream.push(boost::iostreams::back_inserter(l_buff));
    p_request.write(l_stream);
    l_stream.flush();
  }

  l_conn->send(l_buff,
               std::bind(&Client::onSent,
                         this,
                         std::placeholders::_1));

  atom::atomic_inc32(&(this->m_sendSuccess));
  return status::ok;
}



template<typename TDomain>
void
Client<TDomain>::onSent(const boost::system::error_code p_error)
{
  log::debug("network.http.client", "http client onSent (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error == boost::asio::error::operation_aborted)
  {
    log::err("network.http.client", "http client onSent (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    m_userSemaphore.post();
    return;
  }

  if (p_error)
  {
    log::err("network.http.client", "http client onSent (%s) : send failed with error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    m_userSemaphore.post();
    return;
  }

  m_status = cnxstatus::sent;
  do_receive();
  log::debug("network.http.client", "http client onSent (%s) : leaving", TBase::m_connection->info(), HERE);
}






template<typename TDomain>
void
Client<TDomain>::do_receive(void)
{
  log::debug("network.http.client", "http client do_receive (%s) : entering", TBase::m_connection->info(), HERE);
  auto& l_conn = TBase::m_connection;

  sptr<vector<char>> l_res = std::make_shared<vector<char>>();
  l_conn->receive(l_res,
                  std::bind(&Client::onReceived,
                            this,
                            std::placeholders::_1,
                            l_res));
  log::debug("network.http.client", "http client do_receive (%s) : leaving", TBase::m_connection->info(), HERE);
}



template<typename TDomain>
void
Client<TDomain>::onReceived(const boost::system::error_code p_error,
                            sptr<vector<char>>              p_response)
{
  xtd::utils::scoped_fn l_semPos(std::bind(&boost::interprocess::interprocess_semaphore::post, std::ref(m_userSemaphore)));

  log::debug("network.http.client", "http client onReceived (%s) : entering", TBase::m_connection->info(), HERE);

  if (p_error ==  boost::asio::error::operation_aborted)
  {
    log::err("network.http.client", "http client onReceived (%s) : timeout detected (error '%s')", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::timeout;
    return;
  }

  if (p_error)
  {
    log::err("network.http.client", "http client onReceived (%s) : error '%s'", TBase::m_connection->info(), p_error.message(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  if (m_status != cnxstatus::sent)
  {
    log::err("network.http.client", "http client onReceived (%s) : request has invalid status '%d'", TBase::m_connection->info(), HERE);
    m_status = cnxstatus::error;
    return;
  }

  m_status = cnxstatus::received;
  m_response.swap(*p_response);

  boost::posix_time::time_duration l_diff =
    boost::posix_time::microsec_clock::local_time() - m_lastSend;

  TBase::m_lastRTTMs = l_diff.total_milliseconds();
  log::debug("network.http.client", "http onReceived (%s) : client received finished", TBase::m_connection->info(), HERE);
}

template<typename TDomain>
status
Client<TDomain>::receive(Response& p_response)
{
  log::debug("network.http.client", "http client receive (%s) : entering", TBase::m_connection->info(), HERE);
  atom::atomic_inc32(&(this->m_receiveTotal));

  if (m_status == cnxstatus::available)
  {
    log::err("network.http.client", "http client receive : unsent request", HERE);
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }

  utils::do_sem_wait(m_userSemaphore);

  if (m_status == cnxstatus::timeout)
  {
    log::err("network.http.client", "http client receive : timeout error", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_receiveTimeout));
    return status::timeout;
  }

  if (m_status != cnxstatus::received)
  {
    log::err("network.http.client", "http client receive : receive error (status '%d')", HERE);
    m_status = cnxstatus::available;
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }

  m_status = cnxstatus::available;

  boost::iostreams::filtering_istream l_fis;
  l_fis.push(boost::make_iterator_range(m_response));

  if (status::ok != p_response.read(l_fis))
  {
    log::crit("network.http.client", "http client receive (%s) : can't deserialize structure", TBase::m_connection->info(), HERE);
    atom::atomic_inc32(&(this->m_receiveError));
    return status::error;
  }

  log::debug("network.http.client", "http client receive (%s) : leaving", TBase::m_connection->info(), HERE);
  atom::atomic_inc32(&(this->m_receiveSuccess));
  return status::ok;
}

}}} //end namespaces

#endif // !NETWORK_HTTP_CLIENT_HH_
