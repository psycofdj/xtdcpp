#ifndef COUNTERS_COUNTERMANAGER_HH_
# define COUNTERS_COUNTERMANAGER_HH_

# include <boost/thread.hpp>
# include <boost/thread/mutex.hpp>
# include <boost/filesystem.hpp>
# include <boost/property_tree/ptree.hpp>
# include <types.hh> // libcommon
# include "counters_fwd.hh"
# include "Base.hh"


namespace xtd {
namespace counters {

enum class exec_status : uint32_t { starting = 0, running = 1, error = 2, ok = 3 };

class CounterManager
{

public:
  typedef std::shared_ptr<CounterManager>                    t_sptr;
  typedef std::multimap<boost::filesystem::path, Base::t_sptr> t_counters;

public:
  CounterManager(const uint32_t p_deplay, const string& p_path);
  virtual ~CounterManager(void);

public:
  status           start(void);
  void             stop(void);
  void             toJson(boost::property_tree::ptree& p_dst);
  string           getSnmpPath(void) const;
  status           add(Base::t_sptr p_counter, const string& p_path);
  const t_counters getCounters(void);
  void             writeOnDisk(void) const;

private:
  void refresh(void) const;

private:
  uint32_t      m_refreshDelay;
  string        m_snmpPath;
  t_counters    m_counters;
  bool          m_isRunning;
  boost::thread m_thread;
  boost::mutex  m_writeMutex;
};

}}

#endif /* COUNTERS_COUNTERMANAGER_HH_ */
