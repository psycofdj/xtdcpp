#include "log/Stream.hh"

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
Stream::append(const string& p_value)
{
  std::lock_guard<std::mutex> l_lock(ms_mutex);
  m_stream << p_value;
}

}}
