#ifndef NETWORK_HTTP_CLIENTPOOL_HHX_
# define NETWORK_HTTP_CLIENTPOOL_HHX_
# include "base/ClientPool.hh"
# include "http/Client.hh"

namespace xtd {
namespace network {
namespace http {

template<typename Domain>
class ClientPool : public base::ClientPool<http::Client<Domain>>
{ };

}}}


#endif // !NETWORK_HTTP_CLIENTPOOL_HHX_
