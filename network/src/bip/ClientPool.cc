#include "bip/ClientPool.hh"
#include <objects/Doc.hh> // libserializer

namespace xtd {
namespace network {
namespace bip {

template class ClientPool<serializer::Doc, serializer::Doc, af_inet>;
template class ClientPool<serializer::Doc, serializer::Doc, af_unix>;

}}}
