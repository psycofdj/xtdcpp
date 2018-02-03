#include "Perf.hh"
#include <ext/alloc_traits.h>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time.hpp>

namespace xtd {
namespace counters {

namespace bpt = boost::posix_time;


Perf::Perf(const string& p_name,
           uint32_t      p_nbThread,
           uint32_t      p_sampeSize,
           uint32_t      p_deltaTimeMs) :
  Composed(p_name),
  m_avgRTT("RTT", p_sampeSize),
  m_globalPerf("perf"),
  m_instantPerf("instant.perf", p_deltaTimeMs),
  m_startTimes(p_nbThread)
{
  addItem(m_avgRTT);
  addItem(m_globalPerf);
  addItem(m_instantPerf);
}


void
Perf::startChrono(uint32_t p_requestID)
{
  m_startTimes[p_requestID] = bpt::microsec_clock::local_time();
}


void
Perf::stopChrono(uint32_t p_requestID)
{
  bpt::time_duration l_diffTime =
    bpt::microsec_clock::local_time() - m_startTimes[p_requestID];

  m_globalPerf.tick();
  m_instantPerf.tick();
  m_avgRTT = l_diffTime.total_milliseconds();
}

}}
