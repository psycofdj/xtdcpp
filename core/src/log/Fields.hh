#ifndef XTD_CORE_LOG_FIELDS_HH_
# define XTD_CORE_LOG_FIELDS_HH_
# include "types.hh"

namespace xtd {
namespace log {


/**
 ** @brief Holds generic data for each log Record formattable field
 ** @details
 ** This object provides a convenient way to access fields data from either
 ** structure member or field name
 */
template<typename T>
class Fields
{
private:
  typedef Fields<T> this_type;

public:
  /**
   ** @brief Constructor
   */
  Fields(void);

public:
  /**
   ** @brief Get field value from name
   ** @param p_field field name
   ** @param p_value output value reference
   ** @return true if name is valid
   */
  bool get(const string& p_field, T& p_value) const;

  /**
   ** @brief Returns true when field name is valid
   ** @param p_field field name
   */
  bool exists(const string& p_field) const;

  /**
   ** @brief Return field value, @b Undefined behavior is field is unknown
   ** @param p_field field name
   ** @return field value
   */
  T& get(const string& p_field);

  /**
   ** @brief Sets value for given field
   ** @param p_field field name
   ** @param p_value new value to set
   ** @return true is given name is value
   */
  bool set(const string& p_field, const T& p_value);

public:
  T m_name;     ///< log logger name
  T m_module;   ///< log module name
  T m_threadid; ///< log thread identifier
  T m_slevel;   ///< log level, string version
  T m_ilevel;   ///< log level, underlying type version
  T m_message;  ///< log message
  T m_filename; ///< log location filename
  T m_line;     ///< log location line
  T m_function; ///< log location function
  T m_location; ///< log location
  T m_time;     ///< log timestamp
  T m_fulllog;  ///< full log message
  T m_pid;      ///< log process id
  T m_ppid;     ///< log parent process id

private:
  map<string, fn<T&(this_type&)> > m_functors;
};

}}

# include "log/Fields.hxx" // IWYU pragma: export

#endif // !XTD_CORE_LOG_FIELDS_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
