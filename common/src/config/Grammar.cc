#include "config/Grammar.hh"  // libcommon
#include "config/Grammar.hxx" // libcommon

namespace xtd {
namespace config {
namespace impl {

template class Grammar<std::string::iterator>;
template class Grammar<std::string::const_iterator>;
template class Grammar<std::istream_iterator<char>>;

}}}
