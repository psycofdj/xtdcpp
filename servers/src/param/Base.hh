#ifndef SERVERS_PARAM_BASE_HH_
# define SERVERS_PARAM_BASE_HH_
#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/function.hpp>
#include "types.hh"

namespace boost { template <typename Signature> class function; }
namespace xtd { namespace servers { namespace param { class Visitor; } } }
namespace xtd {
namespace servers {
namespace param {

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
  string                   m_name;
  bool                     m_sync;
  string                   m_actionPath;
  boost::posix_time::ptime m_timestamp;
  string                   m_log;
};


}}}

#include "param/Base.hxx" // IWYU pragma: export

#endif // !KESERVERS_PARAM_BASE_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
