#ifndef XTD_CORE_LOG_FIELDS_HXX_
# define XTD_CORE_LOG_FIELDS_HXX_

namespace xtd {
namespace log {

template<typename T>
Fields<T>::Fields(void) :
  mc_functors({
      { "name",     [this](void) -> T& { return m_name;     } },
      { "module",   [this](void) -> T& { return m_module;   } },
      { "threadid", [this](void) -> T& { return m_threadid; } },
      { "slevel",   [this](void) -> T& { return m_slevel;   } },
      { "ilevel",   [this](void) -> T& { return m_ilevel;   } },
      { "message",  [this](void) -> T& { return m_message;  } },
      { "filename", [this](void) -> T& { return m_filename; } },
      { "line",     [this](void) -> T& { return m_line;     } },
      { "function", [this](void) -> T& { return m_function; } },
      { "time",     [this](void) -> T& { return m_time;     } },
      { "location", [this](void) -> T& { return m_location; } },
      { "fulllog",  [this](void) -> T& { return m_fulllog;  } },
      { "pid",      [this](void) -> T& { return m_pid;      } },
      { "ppid",     [this](void) -> T& { return m_ppid;     } }
    })
{
}

template<typename T>
bool
Fields<T>::set(const string& p_field, const T& p_val)
{
  auto c_func = mc_functors.find(p_field);
  if (mc_functors.end() == c_func)
    return false;
  T& l_val = c_func->second();
  l_val = p_val;
  return true;
}

template<typename T>
bool
Fields<T>::get(const string& p_field, T& p_val) const
{
  auto c_func = mc_functors.find(p_field);
  if (mc_functors.end() == c_func)
    return false;
  p_val = c_func->second();
  return true;
}

template<typename T>
bool
Fields<T>::exists(const string& p_field) const
{
  auto c_func = mc_functors.find(p_field);
  return (mc_functors.end() != c_func);
}

template<typename T>
T&
Fields<T>::get(const string& p_field) const
{
  auto c_func = mc_functors.find(p_field);
  return c_func->second();
}

}}

#endif // !XTD_CORE_LOG_FIELDS_HXX_
