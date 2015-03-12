#include "http/Connection.hh"
#include "http/Connection.hxx"
#include "utils/CommTypeDefs.hh"


namespace xtd {
namespace network {
namespace http {

template class Connection<utils::af_inet>;
template class Connection<utils::af_unix>;

}}}
