#ifndef COUNTERS_INSTANTFREQ_HH_
# define COUNTERS_INSTANTFREQ_HH_
#include <boost/date_time/posix_time/posix_time.hpp>
#include <deque>
#include "Value.hh"
#include "types.hh" // licore


namespace xtd {
namespace counters {

/** */

class InstantFreq : public Value<uint32_t>
{
  friend class Composed;

private:
  typedef std::deque<boost::posix_time::ptime> t_timevect;

public:
  InstantFreq(const string& p_name,
              uint32_t       p_timeDeltaMs = 1000);
  virtual ~InstantFreq(void) {}

public:
  void         tick(void);
  InstantFreq& operator=(uint32_t p_value);

protected:
  void update_safe(void);

private:
  bool expired_safe(const boost::posix_time::ptime& p_item,
                    const boost::posix_time::ptime& p_now) const;
  void shrink_safe(const boost::posix_time::ptime& p_now);

private:
  t_timevect         m_samples;
  const uint32_t m_timeDeltaMs;
};

}}

#endif // !COUNTERS_INSTANTFREQ_HH_
