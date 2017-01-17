#ifndef NETWORK_HTTP_CONNECTION_HXX_
# define NETWORK_HTTP_CONNECTION_HXX_
# include <boost/regex.hpp>
# include <boost/iostreams/filtering_stream.hpp>
# include <log.hh> // libcore
# include "http/Message.hh"



namespace xtd {
namespace network {
namespace http {

namespace ba = boost::asio;
namespace bs = boost::system;


template <typename Domain>
Connection<Domain>::Connection(const utils::Config& p_configuration,
                               ba::io_service&      p_ioService,
                               const string         p_hostname,
                               const uint32_t       p_port) :
  base::Connection<Domain>(p_configuration, p_ioService, p_hostname, p_port),
  m_isClosedByServer(false)
{
}

template <typename Domain>
Connection<Domain>::~Connection(void)
{
}


template <typename Domain>
void
Connection<Domain>::async_write(sptr<vector<char>> p_outData, t_handler p_onSent)
{
  ba::async_write(this->m_socket,
                  ba::buffer(*p_outData),
                  std::bind(&Connection::onSent,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            p_onSent));
}

template <typename Domain>
void
Connection<Domain>::onSent(const bs::error_code    p_error,
                           size_t               /* p_bytesTransferred */,
                           t_handler               p_onSent)
{
  p_onSent(p_error);
}




template <typename Domain>
void
Connection<Domain>::async_read(sptr<vector<char>> p_inData, t_handler p_onReceived)
{
  sptr<ba::streambuf> l_header = std::make_shared<ba::streambuf>();

  ba::async_read_until(this->m_socket,
                       *l_header,
                       "\r\n\r\n",
                       std::bind(&Connection::onHeaderReceived,
                                 this,
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 p_inData,
                                 l_header,
                                 p_onReceived));
}


/**
 ** @details
 ** 1. Copie du header dans le conteneur utilisateur
 */
template <typename Domain>
void
Connection<Domain>::onHeaderReceived(const bs::error_code p_error,
                                     size_t               p_bytesTransferred,
                                     sptr<vector<char>>   p_inData,
                                     sptr<ba::streambuf>  p_header,
                                     t_handler            p_onReceived)
{
  if (p_error)
  {
    p_onReceived(p_error);
    return;
  }

  // 1.
  ba::streambuf::const_buffers_type l_buffs = p_header->data();
  copy(ba::buffers_begin(l_buffs),
       ba::buffers_end(l_buffs),
       back_inserter(*p_inData));

  // 1.
  boost::iostreams::filtering_istream l_fis;
  l_fis.push(boost::make_iterator_range(*p_inData));

  Message l_req;
  size_t  l_dataSize;

  if ((status::error == l_req.readHead(l_fis)) ||
      (status::error == l_req.getDataSize(l_dataSize)))
  {
    log::err("network.http.cnx", "http cnx (%s) : malformated http header", this->info(), HERE);
    p_onReceived(ba::error::not_found);
    return;
  }

  log::info("network.http.cnx", "http cnx (%s) : header OK", this->info(), HERE);

  size_t l_suppBytes = ba::buffer_size(l_buffs) - p_bytesTransferred;

  if (l_suppBytes >= l_dataSize)
    p_onReceived(p_error);
  else
    receive_data(l_dataSize - l_suppBytes, p_inData, p_onReceived);
}



template <typename Domain>
void
Connection<Domain>::receive_data(size_t             p_dataSize,
                                 sptr<vector<char>> p_inData,
                                 t_handler          p_onReceived)
{
  this->m_strand.post(std::bind(&Connection::do_receive_data,
                                this,
                                p_dataSize,
                                p_inData,
                                p_onReceived));
}


template <typename Domain>
void
Connection<Domain>::do_receive_data(size_t             p_dataSize,
                                    sptr<vector<char>> p_inData,
                                    t_handler          p_onReceived)
{
  sptr<vector<char>> l_data = std::make_shared<vector<char>>();

  l_data->resize(p_dataSize, 0);
  ba::async_read(this->m_socket,
                 ba::buffer(*l_data),
                 std::bind(&Connection::onDataReceived,
                           this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           p_inData,
                           l_data,
                           p_onReceived));

}

template <typename Domain>
void
Connection<Domain>::onDataReceived(const bs::error_code    p_error,
                                   size_t               /* p_bytesTransferred */,
                                   sptr<vector<char>>      p_inData,
                                   sptr<vector<char>>      p_data,
                                   t_handler               p_onReceived)
{
  if (p_error)
  {
    p_onReceived(p_error);
    return;
  }

  copy(p_data->begin(), p_data->end(), back_inserter(*p_inData));
  p_onReceived(p_error);
}

template <typename Domain>
void
Connection<Domain>::clear(void)
{
  m_isClosedByServer = false;
}

template <typename Domain>
void
Connection<Domain>::setClosedByServer(bool p_status)
{
  m_isClosedByServer = p_status;
}

template <typename Domain>
bool
Connection<Domain>::getClosedByServer(void) const
{
  return m_isClosedByServer;
}


}}} // end namespaces

#endif // !NETWORK_HTTP_CONNECTION_HXX_
