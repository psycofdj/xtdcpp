#ifndef NETWORK_COMMTYPEDEFS_HH_
# define NETWORK_COMMTYPEDEFS_HH_

# include <deque>
# include <boost/asio.hpp>
# include <boost/thread/thread.hpp>
# include <memory>
# include <boost/function.hpp>
# include <types.hh> //libcommon

namespace xtd {
namespace network {
namespace utils {

typedef boost::asio::deadline_timer                      deadLineTimer_t;
typedef std::shared_ptr<boost::asio::io_service>       ioServicePtr_t;
typedef std::shared_ptr<boost::asio::io_service::work> workPtr_t;
typedef std::shared_ptr<boost::asio::streambuf>        streambufPtr_t;
typedef map<boost::thread::id, size_t>                   t_IdNumberMap;
typedef std::pair<boost::thread::id, size_t>             t_IdNumberPair;

// definition of socket type
typedef boost::asio::local::stream_protocol af_unix;
typedef boost::asio::ip::tcp                af_inet;
typedef uint32_t                       requestId_t;

// containers definition
typedef vector<char>                 vectorBytes_t;
typedef vector<uint32_t>        vectorUint32_t;
typedef std::deque<requestId_t>           dequeId_t;
typedef std::shared_ptr<vectorBytes_t>  sharedBuf_t;
typedef std::shared_ptr<vectorUint32_t> sharedHeader_t;
typedef boost::function<void(const boost::system::error_code)> handler_t;

}}} // end namespaces

#endif // !NETWORK_COMMTYPEDEFS_HH_
