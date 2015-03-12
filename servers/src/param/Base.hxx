#ifndef SERVERS_PARAM_BASE_HXX_
# define SERVERS_PARAM_BASE_HXX_

# include <boost/algorithm/string/case_conv.hpp>
# include "param/Visitor.hh"
# include <boost/filesystem.hpp>


using namespace boost;

namespace xtd {
namespace servers {
namespace param {

Base&
Base::constraint(t_constraint p_constraint)
{
  m_constraints.push_back(p_constraint);
  return *this;
}

string
Base::getName(void) const
{
  return m_name;
}

void
Base::setName(const string& p_name)
{
  m_name = p_name;
}

bool
Base::getSync(void) const
{
  return m_sync;
}

void
Base::setSync(const bool& p_sync)
{
  m_sync = p_sync;
}

void
Base::setActionPath(const string& p_actionPath)
{
  m_actionPath = p_actionPath;
}

string
Base::getActionPath(void) const
{
  return m_actionPath;
}

void
Base::sync(void)
{
  if(m_sync)
  {
    string             l_value;
    string             l_path;

    if(true == toStr(l_value))
    {
      l_path = m_actionPath + "/" + m_name;
      ofstream l_file(l_path.c_str(), std::ios_base::out | std::ios_base::trunc);
      l_file << l_value;
      l_file.close();
    }
  }
}

uint32_t
Base::date() const
{
  tm l_tm = to_tm(m_timestamp);
  time_t l_date = mktime(&l_tm);
  return boost::lexical_cast<uint32_t>(l_date);
}

string
Base::log() const
{
  return m_log;
}

void
Base::setLog(const string p_log)
{
  m_log = p_log;
}

Base&
Base::listen(t_listener p_listener)
{
  m_listeners.push_back(p_listener);
  return *this;
}

template<typename T>
POD<T>::POD(const T& p_value) :
  Base(p_value)
{
}


template<typename T>
POD<T>::POD(const T& p_value, const string& p_name) :
  Base(p_value, p_name)
{
}


template<typename T>
bool
POD<T>::toStr(string& p_dst) const
{
  T l_value = T();
  return get(l_value) && as(l_value, p_dst);
}


template<typename T>
bool
POD<T>::fromStr(const string& p_src)
{
  T l_value = T();
  return to(p_src, l_value) && set(l_value);
}

template<typename T>
bool
POD<T>::verify(const string& p_src)
{
  T l_value = T();
  return to(p_src, l_value) && isAccepted(l_value);
}


template<typename T>
void
POD<T>::accept(Visitor& p_visitor) const
{
  p_visitor(*this);
}


template<typename T>
bool
POD<T>::as(const bool& p_src, string& p_dst) const
{
  if (p_src)
    p_dst = "1";
  else
    p_dst = "0";
  return true;
}


template<typename T>
template<typename V>
bool
POD<T>::as(const V& p_src, string& p_dst) const
{
  p_dst = lexical_cast<string>(p_src);
  return true;
}


template<typename T>
bool
POD<T>::to(const string& p_src, bool& p_dst) const
{
  string l_src = to_lower_copy(p_src);

  if ((l_src == "0") || (l_src == "off") || (l_src == "no") || (l_src == "false"))
  {
    p_dst = false;
    return true;
  }
  if ((l_src == "1") || (l_src == "on") || (l_src == "yes") || (l_src == "true"))
  {
    p_dst = true;
    return true;
  }
  return false;
}


template<typename T>
template<typename V>
bool
POD<T>::to(const string& p_src, V& p_dst) const
{
  try {
    p_dst = lexical_cast<V>(p_src);
    return true;
  } catch (bad_lexical_cast) {
    return false;
  }
}


}}}

#endif // !SERVERS_PARAMS_PARAM_HXX_
