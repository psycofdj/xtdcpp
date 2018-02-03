#ifndef COUNTERS_AVGVALUE_HXX_
# define COUNTERS_AVGVALUE_HXX_
# include <boost/thread.hpp>
# include <numeric>
# include "AvgValue.hh"
# include "Composed.hh"
# include "types.hh" // libcore

namespace xtd {
namespace counters {


template<typename TType>
AvgValue<TType>::AvgValue(const string& p_name,
                          uint32_t      p_sampeSize) :
  Composed(p_name),
  m_samples(),
  m_mean("moy"),
  m_min("min"),
  m_max("max"),
  m_nbEvent(0),
  m_sampleSize(p_sampeSize)
{
  addItem(m_mean);
  addItem(m_min);
  addItem(m_max);

  m_mean.NaN();
  m_min.NaN();
  m_max.NaN();
}

template<typename TType>
AvgValue<TType>&
AvgValue<TType>::operator=(const TType& p_value)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  affect_safe(p_value);
  return *this;
}


template<typename TType>
AvgValue<TType>&
AvgValue<TType>::operator+(const AvgValue& p_avg)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  add_safe(p_avg);
  return *this;
}


template<typename TType>
void
AvgValue<TType>::add_safe(const AvgValue& p_avg)
{
  if (0 == m_nbEvent)
  {
    m_min  = p_avg.m_min.getValue();
    m_max  = p_avg.m_max.getValue();
    m_mean = p_avg.m_mean.getValue();
  }

  m_min = std::min(m_min.getValue(), p_avg.m_min.getValue());
  m_max = std::max(m_max.getValue(), p_avg.m_max.getValue());

  if (0 != (m_nbEvent + p_avg.m_nbEvent))
  {
    m_mean =
      ((p_avg.m_nbEvent * p_avg.m_mean.getValue()) +
       (      m_nbEvent *       m_mean.getValue()))  /
      (m_nbEvent + p_avg.m_nbEvent);

    m_nbEvent = m_nbEvent + p_avg.m_nbEvent;
  }
}


template<typename TType>
void
AvgValue<TType>::affect_safe(const TType& p_value)
{
  if (0 == m_nbEvent)
  {
    m_min = p_value;
    m_max = p_value;
  }

  m_nbEvent++;
  m_samples.push_back(p_value);

  if ((m_samples.size() > m_sampleSize) && (0 != m_samples.size()))
    m_samples.pop_front();

  m_min  = std::min(m_min.getValue(), p_value);
  m_max  = std::max(m_max.getValue(), p_value);
  m_mean = (std::accumulate(m_samples.begin(), m_samples.end(), 0) / m_samples.size());
}

}}

#endif // !COUNTERS_AVGVALUE_HXX_
