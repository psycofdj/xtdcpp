#include "http/Connection.hh"
#include "http/Connection.hxx"
#include "network_types.hh"



namespace xtd {
namespace network {
namespace http {

template class Connection<af_inet>;
template class Connection<af_unix>;

}}}
