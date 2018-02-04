#ifndef COUNTERS_FREQ_HH_
# define COUNTERS_FREQ_HH_
# include <boost/date_time/posix_time/posix_time.hpp>
# include "Value.hh"
# include "types.hh" // libcore

namespace xtd {
namespace counters {

class Freq : public Value<uint32_t>
{
  friend class Composed;

public:
  Freq(const string& p_name);
  virtual ~Freq(void) {}

public:
  void  tick(void);
  Freq& operator=(const uint32_t& p_value);

protected:
  virtual void update_safe(void);

private:
  boost::posix_time::ptime m_beginTime;
  uint32_t             m_nbEvent;
};

}}

#endif // !COUNTERS_FREQ_HH_
