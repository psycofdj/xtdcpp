// IWYU pragma: private, include "param/Base.hh"
#ifndef SERVERS_PARAM_BASE_HXX_
# define SERVERS_PARAM_BASE_HXX_
# include <boost/any.hpp>
# include <boost/bind.hpp>
# include <boost/date_time/posix_time/posix_time_types.hpp>
# include <boost/foreach.hpp>
# include <boost/function.hpp>
# include <typeinfo>
# include "log/helpers.hh"
# include "log/logtypes.hh"
# include "types.hh"

namespace xtd { namespace servers { namespace param { class Base; } } }
namespace xtd {
namespace servers {
namespace param {

using namespace boost;

template<typename T>
T
Base::cast(const boost::any& p_val)
{
  return boost::any_cast<T>(p_val);
}


template<typename T>
Base::Base(const T& p_value) :
  m_value(p_value),
  m_defaultValue(p_value),
  m_constraints(),
  m_listeners(),
  m_name(),
  m_sync(true),
  m_actionPath(""),
  m_timestamp(boost::posix_time::microsec_clock::local_time()),
  m_log("no log")
{
}


template<typename T>
Base::Base(const T& p_value, const string& p_name) :
  m_value(p_value),
  m_defaultValue(p_value),
  m_constraints(),
  m_listeners(),
  m_name(p_name),
  m_sync(true),
  m_actionPath(""),
  m_timestamp(boost::posix_time::microsec_clock::local_time()),
  m_log("no log")
{
}


template<typename T>
Base&
Base::constraint(boost::function<bool(const T&)> p_constraint)
{
  m_constraints.push_back(boost::bind(p_constraint, boost::bind(cast<T>, _1)));
  return *this;
}



template<typename T>
bool
Base::get(T& p_dst) const
{
  try {
    p_dst = boost::any_cast<T>(m_value);
    return true;
  }
  catch (boost::bad_any_cast l_error) {
    log::crit("servers.param.base", "unable to cast param '%s' as destination type (%s)", m_name, l_error.what(), HERE);
    return false;
  }
}


template<typename T>
bool
Base::isAccepted(const T& p_src)
{
  // Verify type
  if (typeid(T) != m_defaultValue.type())
    return false;

  // Verify constraints
  BOOST_FOREACH(const t_constraint& c_cond, m_constraints)
  {
    if (false == c_cond(p_src))
      return false;
  }

  return true;
}

template<typename T>
bool
Base::set(const T& p_src)
{
  T l_value;

  if (false == isAccepted(p_src))
    return false;

  // Update timestamp last parameter access
  m_timestamp = boost::posix_time::microsec_clock::local_time();

  // If new value is different, update value and trace parameter change
  if (true == get(l_value) && (l_value != p_src))
  {
    string l_oldStr, l_newStr;
    boost::any  l_old = m_value;

    toStr(l_oldStr);
    m_value = p_src;
    toStr(l_newStr);

    log::crit("servers.param.base", "parameter '%s' has been changed from '%s' to '%s'", m_name, l_oldStr, l_newStr, HERE);

    for (auto& c_callback : m_listeners)
      c_callback(l_old, m_value);
  }

  return true;
}

template<typename T>
Base&
Base::listen(boost::function<void(const T&, const T&)> p_constraint)
{
  m_listeners.push_back(boost::bind(p_constraint,
                                    boost::bind(cast<T>, _1),
                                    boost::bind(cast<T>, _2)));
  return *this;
}

}}}

#endif // !SERVERS_PARAMS_PARAM_HXX_
