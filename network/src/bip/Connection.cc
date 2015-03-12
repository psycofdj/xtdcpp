#include "bip/Connection.hh"
#include "bip/Connection.hxx"
#include "utils/CommTypeDefs.hh"


namespace xtd {
namespace network {

namespace bip {

template class Connection<utils::af_unix>;
template class Connection<utils::af_inet>;

}}}
