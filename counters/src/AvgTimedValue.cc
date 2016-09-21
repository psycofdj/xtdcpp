#include "AvgTimedValue.hh"
#include <numeric>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <log.hh> // libcore



namespace bpt = boost::posix_time;

namespace xtd {
namespace counters {

AvgTimedValue::AvgTimedValue(const string& p_name,
                             uint32_t      p_nbThread,
                             uint32_t p_timeDeltaMs,
                             uint32_t p_thresholdMs) :
  Composed(p_name),
  m_samples(),
  m_startTimes(p_nbThread),
  m_reseted(p_nbThread, 0),
  m_globalPerf("perf"),
  m_instantPerf("instant.perf"),
  m_mean("RTT.moy"),
  m_min("RTT.min"),
  m_max("RTT.max"),
  m_overThreshold("RTT.threshold"),
  m_timeDeltaMs(p_timeDeltaMs),
  m_thresholdMs(p_thresholdMs),
  m_assert(false)
{
  addItem(m_globalPerf);
  addItem(m_instantPerf);
  addItem(m_mean);
  addItem(m_min);
  addItem(m_max);
  addItem(m_overThreshold);

  m_mean.NaN();
  m_min.NaN();
  m_max.NaN();
  m_overThreshold = 0;
  m_globalPerf    = 0;
  m_instantPerf   = 0;
}

void
AvgTimedValue::startChrono(uint32_t p_requestID)
{
  if(m_reseted[p_requestID] == 1)
  {
    log::crit("counters.avgtimedvalue", "AvgTimedValue chrono started but not stopped (ID:%d)!!! Cancel init start date. ", p_requestID, HERE);
  }
  else
  {
    m_startTimes[p_requestID] = bpt::microsec_clock::local_time();
  }

  m_reseted[p_requestID] = 1;
}

void
AvgTimedValue::stopChrono(uint32_t p_requestID)
{
  boost::mutex::scoped_lock l_lock(m_mutex);

  if (m_reseted[p_requestID] == 0)
  {
    log::crit("counters.avgtimedvalue", "AvgTimedValue chrono stopped but not started (ID:%d)!!! Value is discarded. ", p_requestID, HERE);
    return;
  }

  bpt::ptime         l_now      = bpt::microsec_clock::local_time();
  bpt::time_duration l_diffTime = l_now - m_startTimes[p_requestID];
  uint32_t           l_timeUs   = l_diffTime.total_microseconds();

  responsetime_safe(l_timeUs);
  m_reseted[p_requestID] = 0;
}

void
AvgTimedValue::addResponseTime(uint32_t p_timeUs)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  responsetime_safe(p_timeUs);
}


void
AvgTimedValue::responsetime_safe(uint32_t p_timeUs)
{
  bpt::ptime l_now = bpt::microsec_clock::local_time();

  if (true == thresdhold_safe(p_timeUs))
  {
    m_globalPerf.tick();
    m_instantPerf.tick();
    m_samples.push_front(std::make_pair(l_now, p_timeUs));
  }

}


bool
AvgTimedValue::thresdhold_safe(uint32_t p_timeUs)
{
  if ((m_thresholdMs != 0) && (p_timeUs > m_thresholdMs * 1000))
  {
    m_overThreshold++;
    log::crit("counters.avgtimedvalue", "AvgTimedValue over threshold (%d us / threshold : %d000 us)", p_timeUs, m_thresholdMs, HERE);

    if (true == m_assert)
    {
      // /!\ WARINING /!\ //
      // A core has been asked here to study context : must be removed after!
      exit(-3);
    }
    return false;
  }
  return true;
}

/**
 ** @details
 ** 1. On calcul le timestamp "frontiere" entre les elements qu'il faut
 **    garder et ceux qu'il faut supprimer
 ** 2. On trouve (en dichotomie) le premier element a supprimer
 ** 3. On supprime les echantillons a partir de cet element jusqu'a la fin
 */
void
AvgTimedValue::shrink_safe()
{
  // 1.
  bpt::ptime l_now     = bpt::microsec_clock::local_time();
  bpt::ptime l_maxTime = l_now - bpt::milliseconds(m_timeDeltaMs);

  // 2.
  t_samples::iterator c_elem  =
    std::lower_bound(m_samples.begin(),
                     m_samples.end(),
                     l_maxTime,
                     boost::bind(&AvgTimedValue::is_greater, _1, _2));

  // 3.
  m_samples.erase(c_elem, m_samples.end());
}



bool
AvgTimedValue::is_greater(t_elem&                         p_elem,
                          const boost::posix_time::ptime& p_maxTime)
{
  return p_elem.first > p_maxTime;
}


void
AvgTimedValue::update_safe(void)
{
  shrink_safe();

  if (m_samples.empty())
  {
    m_min.NaN();
    m_max.NaN();
    m_mean.NaN();
  }
  else
    compute_safe();
}

void
AvgTimedValue::compute_safe(void)
{
  uint64_t l_sum = 0;
  uint32_t l_min = std::numeric_limits<uint32_t>::max();
  uint32_t l_max = std::numeric_limits<uint32_t>::min();


  BOOST_FOREACH(const t_samples::value_type& c_sample, m_samples)
  {
    l_min = std::min(l_min, c_sample.second);
    l_max = std::max(l_max, c_sample.second);
    l_sum += c_sample.second;
  }


  m_mean = l_sum / m_samples.size();
  m_min  = l_min;
  m_max  = l_max;
}

void
AvgTimedValue::setAssert(const bool p_assert)
{
  m_assert = p_assert;
}

}}
