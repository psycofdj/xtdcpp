#ifndef XTD_CORE_LOG_FIELDS_HH_
# define XTD_CORE_LOG_FIELDS_HH_
# include "types.hh"

namespace xtd {
namespace log {

template<typename T>
struct Fields
{
public:
  Fields(void);

public:
  bool get(const string& p_field, T& p_value) const;
  bool exists(const string& p_field)          const;
  T&   get(const string& p_field)             const;
  bool set(const string& p_field, const T& p_value);

public:
  T m_name;
  T m_module;
  T m_threadid;
  T m_slevel;
  T m_ilevel;
  T m_message;
  T m_filename;
  T m_line;
  T m_function;
  T m_location;
  T m_time;
  T m_fulllog;
  T m_pid;
  T m_ppid;

private:
  map<string, fn<T&(void)> > mc_functors;
};

}}

# include "log/Fields.hxx"

#endif // !XTD_CORE_LOG_FIELDS_HH_
