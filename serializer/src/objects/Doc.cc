#include "objects/Doc.hh"

namespace xtd {
namespace serializer {

Doc::Doc(void) :
  m_score(std::numeric_limits<uint32_t>::max()),
  m_debugScore(std::numeric_limits<uint32_t>::max())
{
}

}}
