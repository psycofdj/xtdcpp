#ifndef NETWORK_UTILS_HXX_
# define NETWORK_UTILS_HXX_
# include <log.hh> // libcore

namespace xtd {
namespace network {
namespace utils {

template<typename T>
void
deque_id<T>::push(const T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);

  if (m_deque.end() != std::find(m_deque.begin(), m_deque.end(), p_param))
  {
    log::debug("network.utils", "double push detected on id '%d'", p_param, HERE);
    return;
  }

  m_deque.push_front(p_param);
}

template<typename T>
void
deque_id<T>::push_back(const T& p_param)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_deque.push_back(p_param);
}

template<typename T>
bool
deque_id<T>::pop(T& p_param)
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
deque_id<T>::find(const T& p_param)
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
deque_id<T>::size(void)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  return m_deque.size();
}


}}}


#endif // !NETWORK_UTILS_HXX_
