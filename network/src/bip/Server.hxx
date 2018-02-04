#ifndef NETWORK_BIP_SERVER_HXX_
# define NETWORK_BIP_SERVER_HXX_
# include <boost/asio.hpp>
# include <boost/interprocess/sync/spin/condition.hpp>
# include <boost/system/error_code.hpp>
# include "types.hh"        // libcore
# include "log/helpers.hh"  // libcore
# include "log/logtypes.hh" // libcore
# include "serializer.hh"   // libserializer
# include "bip/Connection.hh"
# include "bip/Server.hh"
# include "bip/compress.hxx"
# include "network_types.hh"

namespace xtd {
namespace network {
namespace bip {

template<typename TReq, typename TRes, typename Domain>
Server<TReq, TRes, Domain>::Server(void) :
  TBase(),
  m_isPersistent(false),
  m_receiveEof(0),
  m_codec(compress_codec::none)
{
}

template<typename TReq, typename TRes, typename Domain>
Server<TReq, TRes, Domain>::~Server(void)
{
}


template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::compressCodec(compress_codec p_codec)
{
  m_codec = p_codec;
}

template<typename TReq, typename TRes, typename Domain>
compress_codec
Server<TReq, TRes, Domain>::compressCodec(void) const
{
  return m_codec;
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::start(void)
{
  TBase::start();
  log::crit("network.base.client", "bip server started", HERE);
}

template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::stop(void)
{
  log::info("network.base.client", "stopping network::bip server...", HERE);
  TBase::stop();
  log::info("network.base.client", "stopping network::bip server...done", HERE);
}


template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::initialize(const string&  p_host,
                                       const uint32_t p_port,
                                       const size_t   p_nbThread)
{
  TBase::initialize(p_host, p_port, p_nbThread);
}

template<typename TReq, typename TRes, typename Domain>
bool
Server<TReq, TRes, Domain>::shouldReply(uint32_t /*p_requestID*/,
                                        const TReq&  /*p_request*/,
                                        const bool   /*p_requestDebug*/,
                                        TRes&        /*p_response*/,
                                        bool&        /*p_responseDebug*/)
{
  return true;
}

template<typename TReq, typename TRes, typename Domain>
typename Server<TReq, TRes, Domain>::cnx_sptr_t
Server<TReq, TRes, Domain>::createCnx(string p_hostname, uint32_t p_port)
{
  return cnx_sptr_t(new Connection<Domain>(*this, *this->m_ioService, p_hostname, p_port));
}


template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::afterAccept(cnx_sptr_t p_conn)
{
  this->do_receive(p_conn);
}


template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::afterSend(cnx_sptr_t p_conn)
{
  if (true == m_isPersistent)
    this->do_receive(p_conn);
}


template <typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::onReceiveError(const boost::system::error_code p_error, cnx_sptr_t p_conn)
{
  sptr<Connection<Domain> > l_conn =
    std::static_pointer_cast<Connection<Domain> >(p_conn);
  if (p_error == boost::asio::error::eof){
    boost::interprocess::ipcdetail::atomic_inc32(&m_receiveEof);
    if (true == m_isPersistent){
      log::info("network.base.client", "onReceivedError (%s) : closed by client", p_conn->info(), HERE);
    }
  }
  else  TBase::onReceiveError(p_error, p_conn);
}


template <typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::onReceiveTimeout(const boost::system::error_code p_error, cnx_sptr_t p_conn)
{
  sptr<Connection<Domain> > l_conn = std::static_pointer_cast<Connection<Domain> >(p_conn);

  if (m_isPersistent)
    log::info("network.base.client", "onReceivedTimeout (%s) : client did not recycle cnx before server timeout", p_conn->info(), HERE);
  else
    log::err("network.base.client", "onReceivedTimeout (%s) : receive aborted, no answer from client before timeout", p_conn->info(), HERE);
  TBase::onReceiveTimeout(p_error, p_conn);
}


/**
 ** @details
 ** On ne retourne vrai que si on veut que la classe mere
 ** garde le processID en cours en vu du traitement d'une nouvelle
 ** requete.
 */
template<typename TReq, typename TRes, typename Domain>
void
Server<TReq, TRes, Domain>::afterReceive(cnx_sptr_t         p_conn,
                                         sptr<vector<char>> p_inBuffer)
{
  vector<char> l_resBuff;
  TReq         l_req;
  TRes         l_res;
  bool         l_reqDebug = false;
  bool         l_resDebug = false;


  if (status::ok != load<serializer::mode::bin>(m_codec, *p_inBuffer, l_req, l_reqDebug))
  {
    log::crit("network.base.client", "Error while unserializing request (%s)", p_conn->info(), HERE);
    return;
  }

  processObjectRequest(p_conn->getProcessID(), l_req, l_reqDebug, l_res, l_resDebug);

  if (status::ok != save<serializer::mode::bin>(m_codec, l_res, l_resDebug, l_resBuff))
  {
    log::crit("network.base.client", "Error while serializing request (%s)", p_conn->info(), HERE);
    return;
  }

  if (true == shouldReply(p_conn->getProcessID(), l_req, l_reqDebug, l_res, l_resDebug))
  {
    this->do_send(p_conn, l_resBuff);
  }
}

}}} //end namespaces

#endif // !NETWORK_BIP_SERVER_HXX_
