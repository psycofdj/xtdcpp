#include "bip/Client.hh"
#include "bip/Client.hxx"
#include <objects/Doc.hh> // lib serializer
#include "network_types.hh"

namespace xtd {
namespace network {
namespace bip {

template class Client<serializer::Doc, serializer::Doc, af_inet>;
template class Client<serializer::Doc, serializer::Doc, af_unix>;

}}}
