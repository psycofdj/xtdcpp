
#ifndef COUNTERS_AVGTIMEDVALUE_HH_
# define COUNTERS_AVGTIMEDVALUE_HH_

# include <deque>
# include <memory>
# include <types.hh> // libommon
# include "Value.hh"
# include "Composed.hh"
# include "Freq.hh"
# include "InstantFreq.hh"



namespace xtd {
namespace counters {


class AvgTimedValue : public Composed
{
  friend class Composed;

protected:
  typedef std::pair<boost::posix_time::ptime, uint32_t> t_elem;
  typedef std::deque<t_elem>                                t_samples;
  typedef std::deque< boost::posix_time::ptime >            t_timevect;

public:
  typedef std::shared_ptr<AvgTimedValue> t_sptr;

public:
  AvgTimedValue(const string& p_name,
                uint32_t      p_nbThread,
                uint32_t      p_timeDeltaMs = 5 * 60 * 1000,
                uint32_t      p_thresholdMs = 0);

  virtual ~AvgTimedValue(void) {};

public:
  void startChrono(uint32_t p_requestID);
  void stopChrono(uint32_t p_requestID);
  void addResponseTime(uint32_t p_timeUs);
  void setAssert(const bool p_assert);

protected:
  void update_safe(void);

private:
  static bool is_greater(t_elem& p_elem, const boost::posix_time::ptime& p_maxTime);
  void        shrink_safe(void);
  void        compute_safe(void);
  bool        thresdhold_safe(uint32_t p_timeUs);
  void        responsetime_safe(uint32_t p_timeUs);

public:
  t_samples                 m_samples;
  t_timevect                m_startTimes;
  vector<uint32_t> m_reseted;
  Freq                      m_globalPerf;
  InstantFreq               m_instantPerf;
  Value32                   m_mean;
  Value32                   m_min;
  Value32                   m_max;
  Value32                   m_overThreshold;
  const uint32_t        m_timeDeltaMs;
  const uint32_t        m_thresholdMs;
  bool                      m_assert;
};

}}

#endif // !COUNTERS_AVGTIMEDVALUE_HH_
