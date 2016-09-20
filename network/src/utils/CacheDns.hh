#ifndef NETWORK_CACHEDNS_HH_
# define NETWORK_CACHEDNS_HH_

# include <string>
# include <vector>
# include <map>
# include <utility>
# include <list>

# include <boost/thread/thread.hpp>
# include <boost/function.hpp>
# include <memory>
# include <boost/noncopyable.hpp>
# include <boost/bind.hpp>
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsequence-point"
# include <boost/unordered_map.hpp>
# pragma GCC diagnostic pop
# include <boost/make_shared.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>

# include "utils/CommTypeDefs.hh"
# include "utils/Utils.hh"

namespace xtd {
namespace network {
namespace utils {

const uint32_t CACHE_CAPACITY_MAX = 200;
const uint32_t CACHE_TTL_MAX = 1800; // 30 minutes

struct CacheEntry
{
  explicit CacheEntry(const string& p_ipAddress,
                      uint32_t       p_timeStamp) :
    m_value(p_ipAddress),
    m_stamp(p_timeStamp)
  {
  }
  string m_value; //IP address
  std::time_t m_stamp; //time stamp
};

/// typedef for URL/Entry(IP address) pair
typedef std::pair< string, std::shared_ptr<CacheEntry> > EntryPair;
/// typedef for Cache list
typedef std::list< std::shared_ptr<EntryPair> > CacheList;
/// typedef for URL-indexed map (aka hash map) into the CacheList
typedef boost::unordered_map< string, CacheList::iterator > CacheMap;



/**
 ** @brief Cache local de resolution de dns
 ** @details
 ** Cet objet sert de cache local aux entrees resolues par le Resolver
 ** Rien de compliqué, une paire de <data, timestamp> et un paramètre de TTL
 */
class CacheDns : boost::noncopyable
{
public:
  /**
   ** @brief Cache DNS Constructor specifying capacity and entry timetolive
   ** @param p_capacity : max number of entries in the cache
   ** @param p_ttl : maximum time to live of an entry in the cache in seconds
   */
  explicit CacheDns(uint32_t p_capacity, uint32_t p_ttl);

  /**
   * \brief pop the element from the pool
   * @param p_url url of the web site for each we want to retrieve the ip address
   * @param[out] p_ipAddr ip address corresponding to the url
   * @return true if url found in cache and ip address is retrieved
   */
  bool popElem(const string& p_url, string& p_ipAddr);

  /**
   * \brief push the element in the pool
   * @param p_url url of the web site for each we want to add the corresponding the ip address
   * @param p_ipAddr ip address corresponding to the url that we want to add
   */
  void pushElem(const string& p_url,const string& p_ipAddr);

  /// Returns the number of elements present in the cache dns
  inline size_t size(void) const
  {
    return m_entries;
  }

private:
  /**
   ** @brief create a timestamp for an element in the pool
   ** @param p_time timestamp created
   */
  void createTimeStamp(std::time_t& p_time);

  /**
   * \brief checks if timestamp is good and updates if necessary
   * @param[out] p_stamp timestamp checked/updated
   * @param[out] p_dnsTimeoutSeconds durée de vite de l'entrée
   * @return true if timestamp is valid and updated
   */
  bool checkUpdateStamp(std::time_t& p_stamp, uint32_t p_dnsTimeoutSeconds);

  /**
   * \brief moves element to the front of the cache (most recent)
   * @param p_elem cache entry to move to the front of the cache
   */
  void moveElementFrontLst(const std::shared_ptr<EntryPair> p_elem);

  /**
   * \brief checks if the cache is full(maximum capacity reached) and evicts the oldest element from the cache
   */
  void checkAndResize(void);


private:
  /// Cache LRU list
  CacheList m_cacheList;
  /// Cache map
  CacheMap m_cacheMap;
  /// Number of entries in the cache list
  uint32_t m_entries;
  /// Number maximum of entries in the cache list
  uint32_t m_maxEntries;
  /// Time to live of the elements/entries in the cache list - in seconds
  uint32_t m_ttl;

};

}}} // end namespaces

#endif // !NETWORK_CACHEDNS_HH_
