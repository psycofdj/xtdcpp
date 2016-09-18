#include "log/Stream.hh"

namespace xtd {
namespace log {

Stream::Stream(std::ostream& p_stream) :
  m_stream(p_stream),
  m_mutex()
{
}

void
Stream::append(const string& p_value)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_stream << p_value;
}

}}
