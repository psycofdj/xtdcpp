#ifndef COUNTERS_PERF_HH_
# define COUNTERS_PERF_HH_


# include <vector>
# include "Composed.hh"
# include "AvgValue.hh"
# include "Freq.hh"
# include "InstantFreq.hh"



namespace xtd {
namespace counters {

/**
 ** @details
 ** On choisi l'heritage plutot que la composition pour garder une
 ** coherence sur la visibilite des methodes internes collect_safe
 ** update_safe (qu'on serait onbligé de mettre en public ou en friend
 ** pour faire de la composition).
 */
class Perf : public Composed
{
  friend class Composed;

private:
  typedef vector<boost::posix_time::ptime> t_timevect;

public:
  Perf(const string& p_name,
       uint32_t       p_nbThread,
       uint32_t       p_sampeSize   = AvgValue<uint32_t>::mcs_defaultSampleSize,
       uint32_t       p_deltaTimeMs = 1000);

public:
  void startChrono(uint32_t p_requestID);
  void stopChrono(uint32_t p_requestID);

private:
  AvgValue<uint32_t> m_avgRTT;
  Freq                   m_globalPerf;
  InstantFreq            m_instantPerf;
  t_timevect             m_startTimes;
};

}}


#endif // !COUNTERS_PERF_HH_
