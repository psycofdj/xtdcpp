#include "http/Client.hh"
#include "http/Client.hxx"
#include "network_types.hh"

namespace xtd {
namespace network {
namespace http {

template class Client<af_inet>;
template class Client<af_unix>;

}}}
