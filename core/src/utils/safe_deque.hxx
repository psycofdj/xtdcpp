// IWYU pragma: private, include "utils/safe_deque.hh"
#ifndef CORE_UTILS_SAFE_DEQUE_HXX_
# define CORE_UTILS_SAFE_DEQUE_HXX_

namespace xtd {
namespace utils {


template<typename T>
safe_deque<T>::safe_deque(void) :
  m_mutex(),
  m_deque()
{
}

template<typename T>
void
safe_deque<T>::push_front(const T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);

  if (m_deque.end() != std::find(m_deque.begin(), m_deque.end(), p_param))
    return;
  m_deque.push_front(p_param);
}

template<typename T>
void
safe_deque<T>::push_back(const T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_deque.push_back(p_param);
}

template<typename T>
bool
safe_deque<T>::pop(T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  if (m_deque.size() == 0)
    return false;

  p_param = m_deque.front();
  m_deque.pop_front();

  return true;
}

template<typename T>
bool
safe_deque<T>::find(const T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  typename std::deque<T>::const_iterator l_it = m_deque.begin();
  for (; l_it !=  m_deque.end(); l_it++)
  {
    if (*l_it == p_param)
      return true;
  }
  return false;
}

template<typename T>
size_t
safe_deque<T>::size(void)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  return m_deque.size();
}

}}


#endif //! CORE_UTILS_SAFE_DEQUE_HXX_
