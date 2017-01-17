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


class scoped_method
{
private:
  typedef std::function<void(void)> handler_t;

public:
  explicit scoped_method(handler_t p_handler) : m_handler(p_handler) { }
  scoped_method(const scoped_method&) = delete;
  ~scoped_method(void) { m_handler(); }

private:
  handler_t m_handler;
};

template<typename T>
class deque_id
{
public:
  void   push(const T& p_param);
  void   push_back(const T& p_param);
  bool   pop(T& p_param);
  bool   find(const T& p_param);
  size_t size(void);

private:
  std::mutex    m_mutex;
  std::deque<T> m_deque;
};






//converts boost::posix_time::ptime to std::time_t
std::time_t ptime_to_time_t(const boost::posix_time::ptime& t);

template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorbuf : public std::basic_streambuf<CharT, TraitsT>
{
public:
  vectorbuf(vector<CharT> &vec)
  {
    setg(&*vec.begin(), &*vec.begin(), &*vec.begin() + vec.size());
  }
};


}}} // end namespaces

# include "utils/Utils.hxx"

#endif // !NETWORK_UTILS_UTILS_HH_
