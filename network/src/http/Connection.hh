#ifndef NETWORK_HTTP_CONNECTION_HH_
# define NETWORK_HTTP_CONNECTION_HH_

# include "base/Connection.hh"

namespace xtd {
namespace network {
namespace http {

/**
 ** @details
 ** Le protocol http implémenté ici gère les version 1.0 et une sous-partie de la
 ** version 1.1 des spécifications données par le W3C.
 ** <h2>Format</h2>
 ** Un message HTTP est composé de deux parties :
 ** - un header : une suite de caractère ascii de taille variable terminant par
 **               une ligne vide (donc identifiable par la séquence d'octets
 **               "CR-LF-CR-LR")
 ** - une data : une suite de caractère ascii de taille variable, optionnelle et
 **              potentiellement encodée dans différents formats
 **
 ** Sans rentrer dans le détail du format du header, ce qui nous intéresse ici c'est
 ** que, lorsqu'une data est envoyée, il contient une directive @b Content-Length
 ** qui renseigne sur la taille en octet de la data.
 **
 ** A la réception, on lit des données par petits bouts jusqu'à trouver la fin du header
 ** (ligne vide), on extrait ensuite la taille de la donnée et, si elle présente et est
 ** non-nulle, on se met à lire jusqu'à avoir suffisamment d'octets.
 **
 ** Pour des raisons pratique, le parsing du header et la récupération de la taille de la
 ** data est déléguée à l'objet @ref xtd::network::http::Request
 **
 ** <h2> Séquence de dialogue </h2>
 **
 ** @image html http_flow.png
 */
template <typename Domain>
class Connection : public base::Connection<Domain>
{
private:
  static const uint32_t mcs_maxReadLength = 128;

public:
  explicit Connection(const utils::Config& p_configuration,
                      boost::asio::io_service&                p_ioService,
                      const string                       p_hostname,
                      const uint32_t                     p_port);
  virtual ~Connection(void);

private:
  void async_write(utils::sharedBuf_t p_outData,    utils::handler_t p_onSent);
  void async_read(utils::sharedBuf_t  p_inDataData, utils::handler_t p_onReceived);

public:
  void clear(void);
  void setClosedByServer(bool p_status);
  bool getClosedByServer(void) const;

private:
  void onSent(const boost::system::error_code           p_error,
              size_t                               /* p_bytesTransferred */,
              utils::handler_t                          p_onSent);

  void onHeaderReceived(const boost::system::error_code           p_error,
                        size_t                               /* p_bytesTransferred */,
                        utils::sharedBuf_t                        p_inData,
                        std::shared_ptr<boost::asio::streambuf> p_header,
                        utils::handler_t                          p_onReceived);

  void onDataReceived(const boost::system::error_code           p_error,
                      size_t                               /* p_bytesTransferred */,
                      utils::sharedBuf_t                        p_inData,
                      utils::sharedBuf_t                        p_data,
                      utils::handler_t                          p_onReceived);

  void receive_data(size_t        p_dataSize,
                    utils::sharedBuf_t p_inData,
                    utils::handler_t   p_onReceived);

  void do_receive_data(size_t        p_dataSize,
                       utils::sharedBuf_t p_inData,
                       utils::handler_t   p_onReceived);

private:
  bool m_isClosedByServer;
};

}}} // end namespaces


#endif // !NETWORK_HTTP_CONNECTION_HH_
