#include "utils/CacheDns.hh"


namespace xtd {
namespace network {
namespace utils {

namespace bpt = boost::posix_time;

CacheDns::CacheDns(uint32_t p_capacity, uint32_t p_ttl) :
  m_entries(0),
  m_maxEntries(p_capacity),
  m_ttl(p_ttl)
{
}

bool
CacheDns::popElem(const string& p_url,
                  string&       p_ipAddr)
{
  bool ret = false;
  p_ipAddr = ""; // the string should be empty
  //look-up map
  CacheMap::iterator it = m_cacheMap.find(p_url);
  if (it != m_cacheMap.end())
  {
    //found it
    boost::shared_ptr<EntryPair> ep (*(*it).second);
    if (ep)
    {
      boost::shared_ptr<CacheEntry> cacheEntry(ep->second);
      if (cacheEntry)
      {
        //check and update stamp before returning value
        if (checkUpdateStamp(cacheEntry->m_stamp, m_ttl))
        {
          moveElementFrontLst(ep);
          p_ipAddr = cacheEntry->m_value;
          ret = true;
        }
        else
        {
          //simply remove from the map and list
          m_cacheList.erase((*it).second);
          m_cacheMap.erase(p_url);
          m_entries--;
        }
      }
    }
  }
  return ret;
}

void
CacheDns::pushElem(const string& p_url,
                   const string& p_ipAddr)
{
  // create new entry
  //timestamp the element
  std::time_t l_timestamp;
  createTimeStamp(l_timestamp);

  boost::shared_ptr<CacheEntry> l_entry = boost::make_shared<CacheEntry>(p_ipAddr, l_timestamp);

  boost::shared_ptr<EntryPair> l_ep =
    boost::make_shared<EntryPair>(std::make_pair(p_url, l_entry));

  // push it to the front;
  m_cacheList.push_front(l_ep);
  // add it to the cache map
  m_cacheMap[p_url] = m_cacheList.begin();
  // increase count of entries
  m_entries++;
  //if needed resize list and erase element from map
  checkAndResize();
}


void
CacheDns::createTimeStamp(std::time_t& p_timeStamp)
{
  const boost::posix_time::ptime l_time(boost::posix_time::second_clock::universal_time());
  p_timeStamp = ptime_to_time_t(l_time);
}

bool
CacheDns::checkUpdateStamp(std::time_t& p_stamp, uint32_t p_dnsTimeoutSeconds)
{
  bool        l_ret = false;
  std::time_t l_currentTm;

  createTimeStamp(l_currentTm);
  // check elapsed time
  std::time_t l_diff = l_currentTm - p_stamp;
  //if it's within a valid interval update the timestamp and return true
  if (l_diff < p_dnsTimeoutSeconds)
  {
    p_stamp = l_currentTm; // new timestamp
    l_ret = true;
  }
  return l_ret;
}

void
CacheDns::moveElementFrontLst(const boost::shared_ptr<EntryPair> p_elem)
{
  //finds the element
  CacheList::iterator c_ite = std::find(m_cacheList.begin(),m_cacheList.end(), p_elem);

  if (c_ite != m_cacheList.end())
  {
    //insert the found element at the beginning
    m_cacheList.splice(m_cacheList.begin(), m_cacheList, c_ite);
  }
}

void
CacheDns::checkAndResize(void)
{
  // check if it's time to remove the last element
  if ( m_entries > m_maxEntries )
  {
    // erase from the map the last cache list element
    m_cacheMap.erase( m_cacheList.back()->first );
    // erase it from the list
    m_cacheList.pop_back();
    // decrease count
    m_entries--;
  }
}

}}} // end namespaces

