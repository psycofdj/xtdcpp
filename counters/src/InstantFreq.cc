#include "InstantFreq.hh"
#include <boost/bind.hpp>
#include <boost/core/ref.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/thread.hpp>

namespace bpt = boost::posix_time;


namespace xtd {
namespace counters {

InstantFreq::InstantFreq(const string& p_name,
                         uint32_t  p_timeDeltaMs) :
  Value(p_name),
  m_samples(),
  m_timeDeltaMs(p_timeDeltaMs)
{
}

void
InstantFreq::tick(void)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  bpt::ptime                l_now = bpt::microsec_clock::local_time();

  m_samples.push_front(l_now);
  shrink_safe(l_now);
}

InstantFreq&
InstantFreq::operator=(uint32_t p_value)
{
  Value<uint32_t>::operator=(p_value);
  return *this;
}

void
InstantFreq::shrink_safe(const bpt::ptime& p_from)
{
  t_timevect::iterator c_elem =
    boost::find_if(m_samples,
                   boost::bind(&InstantFreq::expired_safe, this, _1, boost::ref(p_from)));
  m_samples.erase(c_elem, m_samples.end());
}

bool
InstantFreq::expired_safe(const bpt::ptime& p_item,
                          const bpt::ptime& p_now) const
{
  bpt::time_duration l_diffTime = p_now - p_item;
  return l_diffTime.total_milliseconds() > m_timeDeltaMs;
}

void
InstantFreq::update_safe(void)
{
  bpt::ptime l_now = bpt::microsec_clock::local_time();

  shrink_safe(l_now);
  m_value = (m_samples.size() * 1000) / m_timeDeltaMs;
}

}}
