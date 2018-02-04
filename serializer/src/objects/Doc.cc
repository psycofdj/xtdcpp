#include "objects/Doc.hh"
#include <limits>    // for numeric_limits
#include "types.hh"  // for uint32_t

namespace xtd {
namespace serializer {

Doc::Doc(void) :
  m_score(std::numeric_limits<uint32_t>::max()),
  m_debugScore(std::numeric_limits<uint32_t>::max())
{
}

}}
