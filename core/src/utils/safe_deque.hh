#ifndef CORE_UTILS_SAFE_DEQUE_HH_
# define CORE_UTILS_SAFE_DEQUE_HH_
# include <deque>
# include <mutex>

namespace xtd {
namespace utils {

template<typename T>
class safe_deque
{
public:
  safe_deque(void);
  safe_deque(const safe_deque&) = delete;

public:
  void   push_front(const T& p_param);
  void   push_back(const T& p_param);
  bool   pop(T& p_param);
  bool   find(const T& p_param);
  size_t size(void);

private:
  std::mutex    m_mutex;
  std::deque<T> m_deque;
};


}}

#include "utils/safe_deque.hxx"

#endif //! CORE_UTILS_SAFE_DEQUE_HH_
