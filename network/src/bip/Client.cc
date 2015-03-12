#include "bip/Client.hh"
#include "bip/Client.hxx"
#include <objects/Doc.hh> // lib serializer

namespace xtd {
namespace network {
namespace bip {

template class Client<serializer::Doc, serializer::Doc, utils::af_inet>;
template class Client<serializer::Doc, serializer::Doc, utils::af_unix>;

}}}
