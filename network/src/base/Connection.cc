#include "base/Connection.hh"
#include "base/Connection.hxx"

namespace xtd {
namespace network {
namespace base {

template <>
void
Connection<utils::af_inet>::setSocketOptions(void)
{
    m_socket.set_option(ba::ip::tcp::no_delay(m_conf.getNoDelay()));
    m_socket.set_option(ba::socket_base::reuse_address(m_conf.getReuseAddr()));
    m_socket.set_option(ba::socket_base::linger(m_conf.getLinger(),
                                                m_conf.getLingerTime()));
}

template <>
void
Connection<utils::af_inet>::readEndpoint(void)
{
  m_remoteAddr = m_socket.remote_endpoint().address().to_string();
  m_remotePort = m_socket.remote_endpoint().port();
  m_localAddr  = m_socket.local_endpoint().address().to_string();
  m_localPort  = m_socket.local_endpoint().port();
}

template <>
void
Connection<utils::af_unix>::setSocketOptions(void)
{
    m_socket.set_option(ba::socket_base::reuse_address(m_conf.getReuseAddr()));
    m_socket.set_option(ba::socket_base::linger(m_conf.getLinger(),
                                                m_conf.getLingerTime()));
}

/**
 ** @details
 ** On strip  le '\0' (cf Resolver.cc:af_unix)
 */
template <>
void
Connection<utils::af_unix>::readEndpoint(void)
{
  m_remotePort = 0;
  m_localPort = 0;
  m_remoteAddr = m_socket.remote_endpoint().path();

  //1.
  if (m_remoteAddr.size() > 1 && m_remoteAddr[0]=='\0')
   m_remoteAddr[0]=' ';
  m_localAddr  = m_socket.local_endpoint().path();
  if (m_localAddr.size() > 1 && m_localAddr[0]=='\0')
   m_localAddr[0]=' ';
}

template class Connection<utils::af_inet>;
template class Connection<utils::af_unix>;

}}}
