#include "base/Client.hh"
#include "base/Client.hxx"
#include "network_types.hh"


namespace xtd {
namespace network {
namespace base {

template class Client<af_inet>;
template class Client<af_unix>;

}}} //end namespaces
