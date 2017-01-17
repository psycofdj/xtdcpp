#ifndef NETWORK_FWD_HH_
# define NETWORK_FWD_HH_
# include "network_types.hh"

namespace xtd {
namespace network {

namespace utils {
class Config;
}

namespace http {
template<typename Domain> class Server;
class Request;
class Response;
}

namespace bip {
template<typename Request, typename Response, typename Domain> class Server;
template<typename Request, typename Response, typename Domain> class Client;
}

}}

#endif // !NETWORK_FWD_HH_
