#ifndef NETWORK_NETWORK_TYPES_HH_
# define NETWORK_NETWORK_TYPES_HH_

# include <functional>
# include <boost/asio.hpp>

namespace xtd {
namespace network {

typedef std::function<void(const boost::system::error_code)> t_handler;
typedef boost::asio::local::stream_protocol af_unix;
typedef boost::asio::ip::tcp                af_inet;


}} // end namespaces

#endif // !NETWORK_NETWORK_TYPES_HH_
