#ifndef NETWORK_CACHEDNS_HH_
# define NETWORK_CACHEDNS_HH_

# include <tuple>
# include <list>
# include <boost/date_time/posix_time/posix_time.hpp>
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsequence-point"
#  include <boost/unordered_map.hpp>
# pragma GCC diagnostic pop

namespace xtd {
namespace utils {


template<typename TKey, typename TVal>
class CacheLRU
{
private:
  /// typedef for URL/Entry(IP address) pair
  typedef std::tuple<TKey, TVal, boost::posix_time::ptime>       t_value;
  typedef std::list<t_value>                                     t_items;
  typedef boost::unordered_map<TKey, typename t_items::iterator> t_index;

public:
  /**
   ** @brief specifying capacity and entry timetolive
   ** @param p_capacity max number of entries
   ** @param p_ttlSec maximum time to live of an entry in seconds
   ** @details
   ** TTL = 0 means no expire
   */
  explicit CacheLRU(uint32_t p_capacity = 200, uint32_t p_ttlSec = 1800);

  /**
   ** @brief Non copyable object
   */
  CacheLRU(const CacheLRU&) = delete;

  /**
   ** @brief pop the element from the pool
   ** @param p_key element key
   ** @param[out] p_val popped element value
   ** @return true if key found
   */
  bool pop(const TKey& p_key, TVal& p_val);

  /**
   ** @brief push the element in the pool
   ** @param p_key element key
   ** @param p_val element value
   */
  void push(const TKey& p_key, const TVal& p_val);

public:
  /**
   ** @brief Returns the number of elements present in container
   */
  inline size_t size(void) const;

private:
  /**
   ** @brief checks if timestamp is good and updates if necessary
   ** @param[out] p_stamp timestamp checked/updated
   ** @return true if timestamp is valid and updated
   */
  bool checkUpdateStamp(boost::posix_time::ptime& p_stamp);


private:
  t_items   m_data;       /// Holds internal data
  t_index   m_index;      /// Holds intex to internal data
  uint32_t  m_maxEntries; /// Number maximum of entries
  uint32_t  m_ttlSec;     /// Time to live of the elements/entries in seconds
};


}} // end namespaces

#include "utils/CacheLRU.hxx" // IWYU pragma: export

#endif // !NETWORK_CACHEDNS_HH_

// Local Variables:
// ispell-local-dictionary: "en"
// End:
