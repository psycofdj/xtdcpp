#ifndef NETWORK_BIP_CONNECTION_HXX_
# define NETWORK_BIP_CONNECTION_HXX_


# include <boost/asio.hpp>
# include <log.hh> // libcore
# include "utils/Config.hh"


namespace xtd {
namespace network {
namespace bip {



namespace ba = boost::asio;
namespace bs = boost::system;

template <typename Domain>
Connection<Domain>::Connection(const utils::Config& p_configuration,
                               ba::io_service&      p_ioService,
                               const string         p_hostname,
                               const uint32_t       p_port) :
  TBase(p_configuration, p_ioService, p_hostname, p_port),
  m_header(mcs_headerSize),
  m_inboundData(),
  m_counter(0)
{
}




/**
 ** @details
 ** Le calcul du crc8 sur la donnee s'effectue sur les N derniers
 ** octects du message (N == mcs_maxDataCrcSize). On ne calcul
 ** pas le crc sur tout le message car ce calcul est assez gourmand.
 ** Dans le cas d'un message vide, le crc8 vaut 0xAA.
 */
template <typename Domain>
uint8_t
Connection<Domain>::computeDataCrc(const vector<char>& p_data)
{

  if (p_data.size() == 0)
    return 0xAA;

  size_t l_beginOffset;
  const char* l_pData = &p_data[0];

  if (p_data.size() <= mcs_maxDataCrcSize)
    l_beginOffset = 0;
  else
    l_beginOffset = p_data.size() - mcs_maxDataCrcSize;

  return calcCRC8(l_pData + l_beginOffset, p_data.size() - l_beginOffset);
}


template <typename Domain>
void
Connection<Domain>::async_write(sptr<vector<char>> p_outData, t_handler p_onSent)
{
  uint8_t                l_crc8       = computeDataCrc(*p_outData);
  sptr<vector<uint32_t>> l_headerBuff = std::make_shared<vector<uint32_t>>();
  sptr<vector<char>>     l_outBuff    = std::make_shared<vector<char>>();


  //build buffer to send
  l_outBuff->assign(p_outData->begin(), p_outData->end());
  l_outBuff->push_back(l_crc8);

  //get the size of the serialized data
  ba::const_buffer l_constBuf    = ba::buffer(*l_outBuff.get());
  uint32_t    l_messageSize = ba::buffer_size(l_constBuf);
  l_headerBuff->resize(mcs_headerSize);


  (*l_headerBuff)[0] = l_messageSize;
  (*l_headerBuff)[1] = m_counter;

  // Calculate CRC32 for data
  uint32_t l_crc = calcCRC(&((*l_headerBuff)[0]), sizeof(uint32_t) * (mcs_headerSize - 1));

  (*l_headerBuff)[mcs_headerSize - 1] = l_crc;

  log::debug("network.bip.cnx", "bip cnx async_write (%s) : Header created (%d)", TBase::info(), m_counter, HERE);

  // construct vector containing header (data size) and data
  vector<ba::const_buffer> l_buffers;

  l_buffers.push_back(ba::buffer(*(l_headerBuff.get())));
  l_buffers.push_back(l_constBuf);

  // send it through the socket
  ba::async_write(this->m_socket,
                  l_buffers,
                  std::bind(&Connection::onSent,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            l_headerBuff,
                            l_outBuff,
                            p_onSent));
}


template <typename Domain>
void
Connection<Domain>::onSent(bs::error_code                     p_error,
                           size_t                          /* p_bytesTransferred */,
                           sptr<vector<uint32_t>>             p_outHeader,
                           sptr<vector<char>>                 p_outData,
                           t_handler                          p_onSent)
{
  p_onSent(p_error);
  p_outData.reset();
  p_outHeader.reset();
}


template <typename Domain>
void
Connection<Domain>::async_read(sptr<vector<char>> p_inData, t_handler p_onReceived)
{
  log::debug("network.bip.cnx", "bip cnx async_read (%s) : enterring (%d)", TBase::info(), m_counter, HERE);

  // async read makes sure to receive all the data
  ba::async_read(this->m_socket,
                 ba::buffer(m_header),
                 std::bind(&Connection::onHeaderReceived,
                           this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           p_inData,
                           p_onReceived));

  log::debug("network.bip.cnx", "bip cnx async_read (%s) : leaving (%d)", TBase::info(), m_counter, HERE);
}

template <typename Domain>
void
Connection<Domain>::onHeaderReceived(const bs::error_code& p_error,
                                     size_t             /* p_bytesTransferred */,
                                     sptr<vector<char>>    p_inData,
                                     t_handler             p_onReceived)
{
  if (p_error)
  {
    log::err("network.bip.cnx", "bip cnx onHeaderReceived (%s) : header receive error %s", TBase::info(), p_error.message(), HERE);
    p_onReceived(p_error);
    return;
  }

  uint32_t l_crc = calcCRC(&m_header[0], sizeof(uint32_t) * (mcs_headerSize - 1));

  if (l_crc  != m_header[mcs_headerSize - 1])
  {
    log::err("network.bip.cnx", "bip cnx onHeaderReceived (%s) : bad header CRC error (0x%hhX != 0x%hhX)", TBase::info(), m_header[mcs_headerSize - 1], l_crc, HERE);
    p_onReceived(ba::error::invalid_argument);
    return;
  }

  m_counter = m_header[1];
  m_inboundData.resize(m_header[0]);

  log::debug("network.bip.cnx", "bip cnx onHeaderReceived (%s) : header received (id : %d)", TBase::info(), m_counter, HERE);
  receive_data(p_inData, p_onReceived);
}


template <typename Domain>
void
Connection<Domain>::receive_data(sptr<vector<char>> p_inData,
                                 t_handler          p_onReceived)
{
  TBase::m_strand.post(std::bind(&Connection::do_receive_data,
                                 this,
                                 p_inData,
                                 p_onReceived));
}


template <typename Domain>
void
Connection<Domain>::do_receive_data(sptr<vector<char>> p_inData,
                                    t_handler          p_onReceived)
{
  ba::async_read(TBase::m_socket,
                 ba::buffer(m_inboundData),
                 std::bind(&Connection::onDataReceived,
                           this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           p_inData,
                           p_onReceived));
}



template <typename Domain>
void
Connection<Domain>::onDataReceived(const bs::error_code&    p_error,
                                   size_t                /* p_bytesTransferred */,
                                   sptr<vector<char>>       p_inData,
                                   t_handler                p_onReceived)
{
  if (p_error)
  {
    log::err("network.bip.cnx", "bip cnx onDataReceived (%s) : data receive error %s (id: %d) ", TBase::info(), p_error.message(), m_counter, HERE);
    p_onReceived(p_error);
    return;
  }

  uint8_t l_dataCrc = m_inboundData.back(); m_inboundData.pop_back();
  uint8_t l_crc8    = computeDataCrc(m_inboundData);

  if (l_dataCrc != l_crc8)
  {
    log::err("network.bip.cnx", "bip cnx onDataReceived (%s) : bad data crc error (0x%hhX != 0x%hhX) (id : %d)", TBase::info(), l_dataCrc, l_crc8, m_counter, HERE);
    p_onReceived(ba::error::invalid_argument);
    return;
  }

  log::debug("network.bip.cnx", "bip cnx onDataReceived (%s) : Data received (id : %d)", TBase::info(), m_counter, HERE);

  p_inData->swap(m_inboundData);
  p_onReceived(p_error);
}


template <typename Domain>
void
Connection<Domain>::incCounter(void)
{
  m_counter++;
}


}}} // end namespaces

#endif // !NETWORK_BIP_CONNECTION_HXX_
