#ifndef SERVERS_PARAM_BASE_HH_
# define SERVERS_PARAM_BASE_HH_

# include <boost/bind.hpp>
# include <boost/any.hpp>
# include <boost/function.hpp>
# include <boost/foreach.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <ctime>
# include <types.hh> // libcore
# include <log.hh>   // libcore


namespace xtd {
namespace servers {
namespace param {

class Visitor;

/**
 * \class Base
 * \brief Param base class
 *
 * provide generic methods to handle generic parameters
 */
class Base
{
public:
  /**
   * @brief Generic cast
   * @details N/A
   *
   * @param p_val generic instance reference
   * @tparam T generic type
   * @return specific type
   */
  template<typename T>
  static T cast(const boost::any& p_val);

public:
  typedef std::shared_ptr<Base> t_sptr;

private:
  typedef boost::function<void(const boost::any&, const boost::any&)> t_listener;
  typedef boost::function<bool(const boost::any&)>                    t_constraint;
  typedef vector<t_listener>                                          t_listeners;
  typedef vector<t_constraint>                                        t_constraints;

public:
  template<typename T>
  Base(const T& p_value);
  template<typename T>
  Base(const T& p_value, const string& p_name);

public:
  virtual bool fromStr(const string& p_src) = 0;
  virtual bool verify(const string& p_src)  = 0;
  virtual bool toStr(string& p_dst) const   = 0;
  virtual void accept(Visitor& p_visitor) const  = 0;

public:

  /**
   * @brief Get parameter name
   * @details N/A
   * @return associated name
   */
  string getName(void) const;

  /**
   * @brief Set parameter name
   * @details N/A
   *
   * @param p_name requested name
   */
  void        setName(const string& p_name);

  /**
   * @brief Get sync property
   * @details [long description]
   * @return true if the parameter needs to be synchronized on the filesystem else false
   */
  bool        getSync(void) const;

  /**
   * @brief Set sync property
   * @details N/A
   *
   * @param p_sync requested sync property
   */
  void        setSync(const bool& p_sync);

  /**
   * @brief Specify filesystem path to synchronize the parameter
   * @details N/A
   *
   * @param p_actionPath filesystem path
   */
  void        setActionPath(const string& p_actionPath);

  /**
   * @brief Get the filesystem path
   * @details N/A
   * @return the filesystem path
   */
  string getActionPath(void) const;

  /**
   * @brief Process synchronization action
   * @details N/A
   */
  void        sync(void);

  /**
   * @brief Get last parameter change date
   * @details N/A
   *
   * @return the last parameter change
   */
  uint32_t date(void) const;

  /**
   * @brief Add a specific constraint the parameter must conform to
   * @details Each time a modification is requested, the constraint will be checked
   * Many constraints can be added, they will be checked in declaration order
   * Opposite constraints must be avoided
   *
   * @param p_constraint the constraint
   * @return a self reference
   */
  Base& constraint(t_constraint p_constraint);

  /**
   * @brief Specify the setter to modify the parameter
   * @details This function will be called each time a call to set is done
   *
   * @param p_listener a modification function
   * @return a self reference
   */
  Base& listen(t_listener p_listener);

  /**
   * @brief Add a specific constraint the parameter must conform to
   * @details Each time a modification is requested, the constraint will be checked
   * Many constraints can be added, they will be checked in declaration order
   * Opposite constraints must be avoided
   *
   * @param p_constraint the constraint
   * @return a self reference
   */
  template<typename T>
  Base& constraint(boost::function<bool(const T&)> p_constraint);

  /**
   * @brief Specify the setter to modify the parameter
   * @details This function will be called each time a call to set is done
   * @param p_constraint a modification function
   * @return a self reference
   */
  template<typename T>
  Base& listen(boost::function<void(const T&, const T&)> p_constraint);

  /**
   * @brief Tells if value is accepted by parameter
   * @details N/A
   *
   * @param p_src the new value
   * @return true if value is accepted
   */
  template<typename T>
  bool isAccepted(const T& p_src);

  /**
   * @brief Function to set the parameter
   * @details N/A
   *
   * @param p_src the new value
   * @return true if setting succeeds
   */
  template<typename T>
  bool set(const T& p_src);

  /**
   * @brief Function to set log associated to parameter
   * @details N/A
   *
   * @param p_log
   */
  void setLog(const string p_log);

  /**
   * @brief Function to get the parameter
   * @details N/A
   *
   * @param p_dst the current value
   * @return true if getting succeeds
   */
  template<typename T>
  bool get(T& p_dst) const;

  /**
   * @brief Function to get log associated to parameter
   * @details N/A
   *
   * @return log associated with parameter
   */
  string log() const;

private:
  boost::any               m_value;
  boost::any               m_defaultValue;
  t_constraints            m_constraints;
  t_listeners              m_listeners;
  string              m_name;
  bool                     m_sync;
  string              m_actionPath;
  boost::posix_time::ptime m_timestamp;
  string              m_log;
};


/**
 * \class POD
 * \brief Templated param class
 *
 * handle type specific operation of a parameter (Base inheritence)
 */
template<typename T>
class POD : public Base
{
public:
  POD(const T& p_value);
  POD(const T& p_value, const string& p_name);

public:
  /**
   * @brief Initialize the POD from a string
   * @param p_src The string to be casted
   * @return true if the string casting and the value setting succeeds
   * @details N/A
   */
  bool fromStr(const string& p_src);

  /**
   * @brief Function to verify if given string value can be setted on parameter
   * @param p_src the new value
   * @return true if value can be setted
   * @details N/A   */
  bool verify(const string& p_src);

  /**
   * @brief Get a string from the POD representing the current value
   * @param p_dst The string containing the current value
   * @return true if the value getting and string building succeeds
   * @details N/A
   */
  bool toStr(string& p_dst)  const;

  /**
   * @brief Register this parameter to a visitor
   * @param p_visitor the right visitor
   * @details N/A
   */
  void accept(Visitor& p_visitor)  const;

private:

  bool as(const bool& p_src, string& p_dst) const;

  bool to(const string& p_src, bool& p_dst) const;

  template<typename V>
  bool as(const V& p_src, string& p_dst) const;

  template<typename V>
  bool to(const string& p_src, V& p_dst) const;
};



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

#endif // !KESERVERS_PARAM_BASE_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
