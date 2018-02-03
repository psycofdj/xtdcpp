#include "config/Grammar.hh"
#include <iterator>
#include "types.hh"
#include "config/Grammar.hxx" // IWYU pragma: keep


namespace xtd {
namespace config {
namespace impl {

template class Grammar<std::string::iterator>;
template class Grammar<std::string::const_iterator>;
template class Grammar<std::istream_iterator<char>>;

}}}
