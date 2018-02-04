#include "types.hh" // libcore
#include "param/Pod.hh"
#include "param/Pod.hxx"

namespace xtd {
namespace servers {
namespace param {

template class POD<bool>;
template class POD<int>;
template class POD<uint8_t>;
template class POD<uint32_t>;
template class POD<uint64_t>;
template class POD<string>;

}}}
