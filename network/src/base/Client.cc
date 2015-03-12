#include "base/Client.hh"
#include "base/Client.hxx"
#include "utils/CommTypeDefs.hh"

namespace xtd {
namespace network {
namespace base {

template class Client<utils::af_inet>;
template class Client<utils::af_unix>;

}}} //end namespaces


