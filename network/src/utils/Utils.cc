#include "utils/Utils.hh"
#include <errno.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/time.hpp>
#include <boost/date_time/time_system_counted.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

namespace xtd {
namespace network {
namespace utils {


void do_sem_wait(boost::interprocess::interprocess_semaphore& p_sem)
{
  try
  {
    p_sem.wait();
  }
  catch(boost::interprocess::interprocess_exception e)
  {
    if (e.get_native_error() == EINTR)
    {
      do_sem_wait(p_sem);
    }
  }
};



std::time_t ptime_to_time_t(const boost::posix_time::ptime& t)
{
  static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1) );
  const boost::posix_time::time_duration::sec_type ss((t - epoch).total_seconds() );
  return ss;
}

}}} // end namespaces
