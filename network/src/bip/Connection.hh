#ifndef NETWORK_BIP_CONNECTION_HH_
# define NETWORK_BIP_CONNECTION_HH_

# include <boost/crc.hpp>
# include "utils/CommTypeDefs.hh"
# include "base/Connection.hh"

namespace xtd {
namespace network {
namespace utils { class Config; }
namespace bip {

/**
 ** @details
 ** <h2> Format </h2>
 ** Un message bip est composé de deux parties :
 ** - un  header     : une suite de Connection::mcs_headerSize  uint32_t (donc taille fixe)
 ** - une data       : une suite de taille variable de uint8
 ** - un crc de data : un crc en uint8 calculé sur les Connection::mcs_maxDataCrcSize derniers
 **                    octects de la partie donnée.
 **
 ** Le header embarque 3 informations :
 ** - la taille de la partie donnée (comptabilise le crc finale) en nombre d'octect
 ** - un identifiant de requête croissant (s'incrémente à chaque requete)
 ** - un crc de header calculé sur les (Connection::mcs_headerSize - 1) unit32 du header
 **
 ** A la réception :
 ** - on commence par lire le header
 ** - on vérifie l'integrité du header en comparant le crc de header reçu
 **   et le crc du header re-calculé
 ** - on extrait la taille du message N attendu a partir du header, on
 **   on va lire sur la socket ces N octects.
 **
 ** A la réception de la partie donnée :
 ** - on vérifie l'intégrité du message en comparant le crc de donnée contenu dans dernier
 **   octet du message au crc recalculé.
 **
 ** <h2> Séquence de dialogue </h2>
 **
 ** @image html bip_flow.png
 */
template <typename Domain>
class Connection : public  base::Connection<Domain>
{
private:
  typedef base::Connection<Domain> TBase;

private:
  static const uint32_t mcs_headerSize     = 3;
  static const uint32_t mcs_maxDataCrcSize = 5;

public:
  explicit Connection(const utils::Config& p_configuration,
                      boost::asio::io_service&                p_ioService,
                      const string                       p_hostname,
                      const uint32_t                      p_port);


public:
  void incCounter(void);

private:
  uint8_t computeDataCrc(const utils::vectorBytes_t& p_data);

  /**
   * \brief calculates the CRC32
   * @param p_buffer the buffer to calculate the CRC
   * @param p_byteCount number of bytes to process
   * @return uint32_t  the CRC
   */
  inline uint32_t calcCRC(void const* p_buffer, size_t p_byteCount);

  /**
   * \brief calculates the CRC8
   * @param p_buffer the buffer to calculate the CRC
   * @param p_byteCount number of bytes to process
   * @return uint8_t the CRC
   */
  inline uint8_t calcCRC8(void const* p_buffer, size_t p_byteCount);



  void async_write(utils::sharedBuf_t p_outData, utils::handler_t p_onSent);
  void async_read(utils::sharedBuf_t p_inData,   utils::handler_t p_onReceived);



  void onSent(boost::system::error_code                p_error,
              size_t                              /* p_bytesTransferred */,
              std::shared_ptr<utils::vectorUint32_t> p_outHeader,
              utils::sharedBuf_t                       p_outData,
              utils::handler_t                         p_onSent);



  void onHeaderReceived(const boost::system::error_code& p_error,
                        size_t                      /* p_bytesTransferred */,
                        utils::sharedBuf_t               p_inData,
                        utils::handler_t                 p_onReceived);


  void receive_data(utils::sharedBuf_t p_inData,
                    utils::handler_t   p_onReceived);

  void do_receive_data(utils::sharedBuf_t p_inData,
                       utils::handler_t   p_onReceived);


  void onDataReceived(const boost::system::error_code& p_error,
                      size_t                      /* p_bytesTransferred */,
                      utils::sharedBuf_t               p_inData,
                      utils::handler_t                 p_onReceived);


private:
  utils::vectorUint32_t m_header;
  utils::vectorBytes_t  m_inboundData;
  uint32_t          m_counter;
};


template <typename Domain>
uint32_t
Connection<Domain>::calcCRC(void const* p_buffer, size_t p_byteCount)
{
  boost::crc_32_type l_result;
  l_result.process_bytes(p_buffer, p_byteCount);
  return l_result.checksum();
}


template <typename Domain>
uint8_t
Connection<Domain>::calcCRC8(void const* p_buffer, size_t p_byteCount)
{
  boost::crc_optimal<8, 0x9B, 0, 0, false, false> crc_8;
  crc_8.process_bytes(p_buffer, p_byteCount);
  return crc_8.checksum();
}


}}} //end namespaces


#endif // !NETWORK_BIP_CONNECTION_HXX_
