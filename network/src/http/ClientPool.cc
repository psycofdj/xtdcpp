#include "http/ClientPool.hh"
#include "network_types.hh"

namespace xtd {
namespace network {
namespace http {

template class ClientPool<af_inet>;
template class ClientPool<af_unix>;

}}}
