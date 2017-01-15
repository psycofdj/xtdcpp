#include "http/Client.hh"
#include "http/Client.hxx"

namespace xtd {
namespace network {
namespace http {

template class Client<utils::af_inet>;
template class Client<utils::af_unix>;

}}}
