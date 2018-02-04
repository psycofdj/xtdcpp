#include "bip/ClientPool.hh"
#include "objects/Doc.hh" // IWYU pragma: keep // libserializer
#include "network_types.hh"

namespace xtd {
namespace network {
namespace bip {

template class ClientPool<serializer::Doc, serializer::Doc, af_inet>;
template class ClientPool<serializer::Doc, serializer::Doc, af_unix>;

}}}
