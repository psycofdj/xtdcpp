#ifndef NETWORK_UTILS_UTILS_HH_
# define NETWORK_UTILS_UTILS_HH_

# include <vector>
# include <deque>
# include <streambuf>
# include <string>
# include <mutex>
# include <functional>
# include <ctime>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include "types.hh" // libcore

namespace xtd {
namespace network {
// fwd decl
namespace configuration { class CommConfiguration; }
namespace utils {

void do_sem_wait(boost::interprocess::interprocess_semaphore& p_sem);






//converts boost::posix_time::ptime to std::time_t
std::time_t ptime_to_time_t(const boost::posix_time::ptime& t);


}}} // end namespaces


#endif // !NETWORK_UTILS_UTILS_HH_
