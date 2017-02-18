#include "http/ClientPool.hh"
#include <objects/Doc.hh> // libserializer

namespace xtd {
namespace network {
namespace http {

template class ClientPool<af_inet>;
template class ClientPool<af_unix>;

}}}
