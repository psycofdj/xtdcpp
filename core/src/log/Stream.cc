#include "log/Stream.hh"
#include "types.hh"

namespace xtd {
namespace log {

std::mutex Stream::ms_mutex;

Stream::Stream(std::ostream& p_stream) :
  m_stream(p_stream)
{
}

Stream::Stream(const sptr<std::ostream>& p_stream) :
  m_ref(p_stream),
  m_stream(*m_ref)
{
}

void
Stream::flush(void)
{
  m_stream.flush();
}

}}
