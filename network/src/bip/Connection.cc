#include "bip/Connection.hh"
#include "bip/Connection.hxx"
#include "network_types.hh"


namespace xtd {
namespace network {
namespace bip {

template class Connection<af_unix>;
template class Connection<af_inet>;

}}}
