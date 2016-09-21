#ifndef COUNTERS_AVGVALUE_HH_
# define COUNTERS_AVGVALUE_HH_

# include <deque>
# include <types.hh> // libommon
# include "Value.hh"
# include "Composed.hh"


namespace xtd {
namespace counters {

template<typename TType>
class AvgValue : public Composed
{
  friend class Composed;

public:
  typedef std::shared_ptr<AvgValue<TType> > t_sptr;

protected:
  typedef std::deque<TType> t_samples;

public:
  static const uint32_t mcs_defaultSampleSize = 1000;

public:
  AvgValue(const string& p_name,
           uint32_t       p_sampeSize = mcs_defaultSampleSize);
  virtual ~AvgValue(void) {};

public:
  AvgValue& operator=(const TType& p_value);
  AvgValue& operator+(const AvgValue& p_value);

protected:
  void affect_safe(const TType& p_value);
  void add_safe(const AvgValue& p_value);

protected:
  t_samples    m_samples;
  Value<TType> m_mean;
  Value<TType> m_min;
  Value<TType> m_max;
  uint32_t m_nbEvent;

  const uint32_t m_sampleSize;
};

typedef AvgValue<uint32_t> AvgValue32;
typedef AvgValue<uint64_t> AvgValue64;

}}

#endif // !COUNTERS_AVGVALUE_HH_
