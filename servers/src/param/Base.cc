#include "param/Base.hh"
#include <time.h>
#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/lexical_cast.hpp>

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

}}}
