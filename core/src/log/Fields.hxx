#ifndef XTD_CORE_LOG_FIELDS_HXX_
# define XTD_CORE_LOG_FIELDS_HXX_

namespace xtd {
namespace log {

template<typename T>
Fields<T>::Fields(void) :
  m_functors({
      { "name",     [](this_type& p_obj) -> T& { return p_obj.m_name;     } },
      { "module",   [](this_type& p_obj) -> T& { return p_obj.m_module;   } },
      { "threadid", [](this_type& p_obj) -> T& { return p_obj.m_threadid; } },
      { "slevel",   [](this_type& p_obj) -> T& { return p_obj.m_slevel;   } },
      { "ilevel",   [](this_type& p_obj) -> T& { return p_obj.m_ilevel;   } },
      { "message",  [](this_type& p_obj) -> T& { return p_obj.m_message;  } },
      { "filename", [](this_type& p_obj) -> T& { return p_obj.m_filename; } },
      { "line",     [](this_type& p_obj) -> T& { return p_obj.m_line;     } },
      { "function", [](this_type& p_obj) -> T& { return p_obj.m_function; } },
      { "time",     [](this_type& p_obj) -> T& { return p_obj.m_time;     } },
      { "location", [](this_type& p_obj) -> T& { return p_obj.m_location; } },
      { "fulllog",  [](this_type& p_obj) -> T& { return p_obj.m_fulllog;  } },
      { "pid",      [](this_type& p_obj) -> T& { return p_obj.m_pid;      } },
      { "ppid",     [](this_type& p_obj) -> T& { return p_obj.m_ppid;     } }
    })
{
}


template<typename T>
bool
Fields<T>::exists(const string& p_field) const
{
  auto c_func = m_functors.find(p_field);
  return (m_functors.end() != c_func);
}

template<typename T>
T&
Fields<T>::get(const string& p_field)
{
  auto c_func = m_functors.find(p_field);
  return c_func->second(*this);
}

template<typename T>
bool
Fields<T>::set(const string& p_field, const T& p_val)
{
  if (false == exists(p_field))
    return false;
  get(p_field) = p_val;
  return true;
}

/**
 ** @details
 ** 1. We allow const_cast since member reference is copied to result value
 **    and therefore not modified.
 */
template<typename T>
bool
Fields<T>::get(const string& p_field, T& p_val) const
{
  auto c_func = m_functors.find(p_field);
  if (m_functors.end() == c_func)
    return false;
  p_val = c_func->second(const_cast<this_type&>(*this));
  return true;
}

}}

#endif // !XTD_CORE_LOG_FIELDS_HXX_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
