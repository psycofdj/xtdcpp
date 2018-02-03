#ifndef SERVERS_PARAM_HANDLER_HH_
# define SERVERS_PARAM_HANDLER_HH_
# include <boost/bind.hpp>
# include <boost/core/ref.hpp>
# include <error.hh>
# include <map>
# include <memory>
# include <string>
# include <types.hh>
# include <utility>
# include "param/Base.hh"
# include "param/Pod.hh"

namespace xtd { namespace servers { namespace param { class Visitor; } } }
namespace xtd {
namespace servers {
namespace param {

xtd_error_class(param_error, error);

/**
 * \class Handler
 * \brief Param handler class
 *
 * Store declared parameter in an internal map throw add method
 * Provide get and set methods to modify already declared parameters
 */
class Handler
{
private:
  typedef Base::t_sptr               t_param_sptr;
  typedef map<string, t_param_sptr>  t_params;

public:
  /**
   * @brief Constructor
   * @details N/A
   *
   * @param p_actionPath the action path where to store persistent parameters (i.e. files containing values)
   */
  Handler(const string& p_actionPath);

private:
  template<typename T>
  struct setter {
    static void apply(T& p_dst, const T& p_src) {
      p_dst = p_src;
    }
  };

public:
  /**
   * @brief Add a new parameter to the Handler
   * @details N/A
   *
   * @param p_name the key name for the parameter
   * @param p_val the the variable to register
   * @param p_syncDisk the persistence parameter
   * @return a shared ptr to the base param object
   */
  template<typename T>
  t_param_sptr add(const string& p_name, const T& p_val, bool p_syncDisk = true);

  template<typename T>
  t_param_sptr bind(const string& p_name, T& p_val, bool p_syncDisk = true);

  /**
   * @brief Add a new parameter to the Handler
   * @details N/A
   *
   * @param p_name the key name for the parameter
   * @param p_param a shared ptr to an already existing parameter to register
   * @param p_syncDisk the persistence parameter
   * @return a shared ptr to the base param object
   */
  t_param_sptr add(const string& p_name, t_param_sptr p_param, bool p_syncDisk = true);

  /**
   * @brief Existency checking
   * @details N/A
   *
   * @param p_name the parameter to check
   * @return true if already declared
   */
  bool   exists(const string& p_name);

  /**
   * @brief Initialization
   * @details N/A
   */
  void   initialize(void);

  /**
   * @brief Add parameters to the Json visitor
   * @details call accept on each parameter
   *
   * @param p_visitor the right visitor
   */
  void accept(Visitor& p_visitor) const;

  /**
   * @brief Function to verify if given string value can be setted on parameter
   * @param p_name parameter value
   * @param p_src the new value
   * @return true if value can be setted
   * @details N/A
   */
  bool verify(const string& p_name, const string& p_src);

  /**
   * @brief Retreive parameter value from a string
   * @param p_name the parameter name
   * @param p_src the string value
   * @param p_log logging message
   * @return true if it succeeds
   * @details N/A
   */
  bool fromStr(const string& p_name, const string& p_src, const string& p_log);

  /**
   * @brief return an iterator on a find call on map
   * @details make a find with lowercase on given name
   *
   * @param p_name the parameter name to find in map
   *
   * @return iterator result of map search
   */
  t_params::iterator find(const string& p_name);

  /**
   * @brief Get the current value of a parameter
   * @details N/A
   *
   * @param p_name the parameter name
   * @param p_dst the variable to store the content of the current value
   * @return true if it succeeds
   */
  template<typename T>
  bool get(const string& p_name, T& p_dst);

  /**
   * @brief Set a parameter value
   * @details N/A
   *
   * @param p_name the parameter name
   * @param p_src the new value
   * @return true if it succeeds
   */
  template<typename T>
  bool set(const string& p_name, const T& p_src);

  /**
   * @brief Specify the setter to modify the parameter
   * @details N/A
   *
   * @param p_name the parameter name
   * @param p_handler the modification handler
   * @return true if it succeeds (i.e. parameter exists)
   */
  template<typename T>
  bool listen(const string& p_name, T p_handler);

  /**
   * @brief Add a specific constraint to match while trying to modify a parameter
   * @details Each time a modification is requested, the constraint will be checked
   * Many constraints can be added, they will be checked in declaration order
   * Opposite constraints must be avoided
   *
   * @param p_name the parameter name
   * @param p_handler the specific constraint
   * @return true if add succeeds
   */
  template<typename T>
  bool constraint(const string& p_name, T p_handler);

private:
  void sync(const string& p_name);

private:
  string m_actionPath;
  t_params    m_params;
};


template<typename T>
Handler::t_param_sptr
Handler::add(const string& p_name, const T& p_val, bool p_syncDisk)
{
  t_param_sptr l_param(new POD<T>(p_val, p_name));
  return add(p_name, l_param, p_syncDisk);
}



template<typename T>
Handler::t_param_sptr
Handler::bind(const string& p_name, T& p_val, bool p_syncDisk)
{
  Handler::t_param_sptr l_param;

  l_param = add(p_name, p_val, p_syncDisk);
  l_param->listen<T>(boost::bind(&Handler::setter<T>::apply, boost::ref(p_val), _2));

  return l_param;
}


template<typename T>
bool
Handler::get(const string& p_name, T& p_dst)
{
  t_params::iterator c_param = find(p_name);

  if (c_param == m_params.end())
    return false;
  return c_param->second->get(p_dst);
}


template<typename T>
bool
Handler::set(const string& p_name, const T& p_src)
{
  t_params::iterator c_param = find(p_name);

  if (c_param == m_params.end())
    return false;
  return c_param->second->set(p_src);
}


template<typename T>
bool
Handler::listen(const string& p_name, T p_handler)
{
  t_params::iterator c_param = find(p_name);

  if (c_param == m_params.end())
    return false;

  c_param->second->listen(p_handler);
  return true;
}


template<typename T>
bool
Handler::constraint(const string& p_name, T p_handler)
{
  t_params::iterator c_param = find(p_name);

  if (c_param == m_params.end())
    return false;

  c_param->second->constraint(p_handler);
  return true;
}

}}}

#endif // !SERVERS_APP_PARAMHANDLER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
