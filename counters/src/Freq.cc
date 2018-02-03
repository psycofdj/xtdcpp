#include "Freq.hh"
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time.hpp>
#include <boost/interprocess/sync/spin/condition.hpp>

namespace xtd {
namespace counters {

namespace bpt = boost::posix_time;
namespace atom = boost::interprocess::ipcdetail;

Freq::Freq(const string& p_name) :
  Value(p_name),
  m_beginTime(bpt::microsec_clock::local_time()),
  m_nbEvent(0)
{
}

Freq&
Freq::operator=(const uint32_t& p_value)
{
  Value<uint32_t>::operator=(p_value);
  return *this;
}

void
Freq::tick(void)
{
  atom::atomic_inc32(&m_nbEvent);
}


void
Freq::update_safe(void)
{
  bpt::time_duration l_diffTime = bpt::microsec_clock::local_time() - m_beginTime;

  if (l_diffTime.total_milliseconds() == 0)
    return;
  m_value = (m_nbEvent * 1000) / l_diffTime.total_milliseconds();
}


}}
