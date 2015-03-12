#include "http/Server.hh"
#include "http/Server.hxx"
#include "utils/CommTypeDefs.hh"

namespace xtd {
namespace network {

namespace http {

template class Server<utils::af_unix>;
template class Server<utils::af_inet>;

}}}
