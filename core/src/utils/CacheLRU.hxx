#include "utils/CacheLRU.hh"

namespace xtd {
namespace utils {

namespace bpt = boost::posix_time;


template<typename TKey, typename TVal>
CacheLRU<TKey, TVal>::CacheLRU(uint32_t p_capacity, uint32_t p_ttlSec) :
  m_maxEntries(p_capacity),
  m_ttlSec(p_ttlSec)
{
}

/**
 ** @details
 ** 1. check and update stamp before returning value
 ** 2. simply remove from the map and list
 */
template<typename TKey, typename TVal>
bool
CacheLRU<TKey, TVal>::pop(const TKey& p_key, TVal& p_val)
{
  typename t_index::iterator c_index = m_index.find(p_key);
  if (c_index == m_index.end())
    return false;

  typename t_items::iterator c_it   = c_index->second;
  t_value&                   c_value = *c_it;

  // 1.
  if (true == checkUpdateStamp(std::get<2>(c_value)))
  {
    m_data.splice(m_data.begin(), m_data, c_it);
    p_val = std::get<1>(c_value);
    return true;
  }

  // 2.
  m_data.erase(c_it);
  m_index.erase(c_index);
  return false;
}

template<typename TKey, typename TVal>
void
CacheLRU<TKey, TVal>::push(const TKey& p_key, const TVal& p_val)
{
  bpt::ptime l_now = bpt::second_clock::universal_time();
  t_value    l_val(p_key, p_val, l_now);

  m_data.push_front(l_val);
  m_index.insert(std::make_pair(p_key, m_data.begin()));

  // check if it's time to remove the last element
  if (m_index.size() > m_maxEntries)
  {
    t_value c_last = *(m_data.rbegin());
    m_index.erase(std::get<0>(c_last));
    m_data.pop_back();
  }
}

template<typename TKey, typename TVal>
bool
CacheLRU<TKey, TVal>::checkUpdateStamp(bpt::ptime& p_stamp)
{
  bpt::ptime         l_now  = bpt::second_clock::universal_time();
  bpt::time_duration l_diff = l_now - p_stamp;

  if ((m_ttlSec == 0) || l_diff < bpt::seconds(m_ttlSec))
  {
    p_stamp = l_now;
    return true;
  }
  return false;
}

template<typename TKey, typename TVal>
size_t
CacheLRU<TKey, TVal>::size(void) const
{
  return m_index.size();
}

}} // end namespaces
