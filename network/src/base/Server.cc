#include "base/Server.hh"
#include "base/Server.hxx"
#include "network_types.hh"



namespace xtd {
namespace network {
namespace base {

template class Server<af_inet>;
template class Server<af_unix>;

}}} //end namespaces
