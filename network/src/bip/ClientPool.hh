#ifndef NETWORK_BIP_CLIENTPOOL_HHX_
# define NETWORK_BIP_CLIENTPOOL_HHX_
# include "base/ClientPool.hh"
# include "bip/Client.hh"

namespace xtd {
namespace network {
namespace bip {

template<typename TReq, typename TRes, typename Domain>
class ClientPool : public base::ClientPool<bip::Client<TReq, TRes, Domain>>
{ };

}}}


#endif // !NETWORK_BIP_CLIENTPOOL_HHX_
