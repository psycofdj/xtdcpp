#ifndef NETWORK_NETWORK_TYPES_HH_
# define NETWORK_NETWORK_TYPES_HH_

# include <functional>
# include <boost/asio.hpp>

namespace xtd {
namespace network {

typedef std::function<void(const boost::system::error_code)> t_handler;
typedef boost::asio::local::stream_protocol af_unix;
typedef boost::asio::ip::tcp                af_inet;


enum class socket_opt : uint32_t {
  nodelay   = 1,
  reuseaddr = 2,
  keepalive = 4,
  linger    = 8
 };

enum class compress_codec  : uint32_t {
  none  = 0,
  zlib  = 1,
  gzip  = 2,
  bzip2 = 3
 };


}} // end namespaces

#endif // !NETWORK_NETWORK_TYPES_HH_
