#include "base/Server.hh"
#include "base/Server.hxx"
#include "utils/CommTypeDefs.hh"


namespace xtd {
namespace network {
namespace base {

template class Server<utils::af_inet>;
template class Server<utils::af_unix>;

}}} //end namespaces
