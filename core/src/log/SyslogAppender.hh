#ifndef CORE_LOG_SYSLOGAPPENDER_HH_
# define CORE_LOG_SYSLOGAPPENDER_HH_
# include <mutex>
# include "log/Appender.hh"
# include "mixins/cloneable.hh"

class TestSyslogAppender;

namespace xtd {
namespace log {

/**
 ** @brief Forward log record to system syslog facility
 ** @details
 **
 ** This object processes given log record to the installed system syslog.
 ** Constructor takes the same paramters than the underlying C-function openlog
 ** which is well documented at ```man 3 openlog```.
 **
 ** In addition static methods helps you getting option and facility integer values
 ** from corresponding string literals :
 ** - @ref SyslogAppender::to_facility "to_facility(string, int&)" :
 **   - @copybrief SyslogAppender::to_facility
 ** - @ref SyslogAppender::to_option "to_option(string, int&)" :
 **   - @copybrief SyslogAppender::to_option
 **
 **
 ** ### Important note
 **
 ** Because of the global nature of system syslog, a program can't initialize
 ** multiple instances of syslog with different parameters. This object implement
 ** a reference count mechanism that ensures that ```openlog``` is called when the
 ** first instance is create, and ```closelog``` is called when the last instance
 ** is destroyed.
 **
 ** As a consequence, only the parameters given to the first instance of
 ** SyslogAppender will be used to initialize syslog.
 **
 **
 ** ### Configuration
 **
 ** For a given name **@<name@>**, the @ref SyslogAppender::create function considers
 ** the following property keys :
 ** - *log.appender.@<name@>.identity* : (optional) give the syslog identity parameter.
 **   If the property is unset, program name will be used.
 **
 ** - *log.appender.@<name@>.facility* : (mandatory) exactly one of the valid syslog
 **   facility names. ex. *LOG_USER*.
 **
 ** - *log.appender.@<name@>.options* : (optional) comma separated list of valid
 **   syslog option names. ex. *LOG_PID, LOG_NOWAIT*
 **
 **
 ** @code
 ** map<string,string> l_props({
 **   { "log.appender.A1.identity", "my-name" },
 **   { "log.appender.A1.facility", "LOG_LOCAL0" },
 **   { "log.appender.A1.options",  "LOG_PID, LOG_NOWAIT" }});
 **
 ** auto l_a1 = SyslogAppender::create("A1", l_props);
 **
 ** log::getRoot().addAppender(l_a1);
 ** @endcode
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** Constructor and destructor uses a mutex-based locking that prevents from
 ** unwanted multiple openlog and closelog calls.
 **
 ** The underlying syslog call isn't protected but POSIX specification states
 ** that concurrent calls to syslog is thread safe.
 */
class SyslogAppender : public Appender
{
  friend class ConfLoader;
  friend class ::TestSyslogAppender;

public:
  /**
   ** @brief Constructor, calls openlog if first created instance
   ** @param p_identity syslog identity. Literal "%(binname)" to use program name
   ** @param p_opts syslog option mask, same as option parameter of C function openlog
   ** @param p_facility syslog facility, same as facility parameter of C function openlog
   ** @param p_formatter log record formatter, default is Formatter
   ** @details
   ** This constructor ensures openlog is called only once. Only the first
   ** SyslogAppender object will initialized syslog with its parameters.
   */
  SyslogAppender(const string&          p_identity,
                 int32_t                p_opts,
                 int32_t                p_facility,
                 const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());

  /**
   ** @brief Destructor, calls closelog if last used instance
   */
  virtual ~SyslogAppender(void);

public:
  /**
   ** @brief Converts facility string literals to their corresponding syslog value
   ** @param p_value facility name, like "LOG_LOCAL0"
   ** @param p_result integer result reference
   ** @retval true p_value is a valid facility name
   ** @retval false p_value is not a valid facility name
   */
  static bool to_facility(const string& p_value, uint32_t& p_result);

  /**
   ** @brief Converts option string literals to their corresponding syslog value
   ** @param p_value option name, like "LOG_PID"
   ** @param p_result integer result reference
   ** @retval true p_value is a valid option name
   ** @retval false p_value is not a valid option  name
   */
  static bool to_option(const string& p_value, uint32_t& p_result);

public:
  /**
   ** @brief Create a new SyslogAppender instance from given configuration map
   ** @param p_name Appender name
   ** @param p_properties configuration map
   ** @details
   ** Full configuration specification described in @ref SyslogAppender detailed
   ** section.
   */
  static sptr<Appender> create(const string&             p_name,
                               const map<string,string>& p_properties);

protected:
  /**
   ** @brief Actually deliver given record to syslog
   ** @param p_rec log record to send
   */
  void print(const FormattedRecord& p_rec) const override;

private:
  int32_t            m_opts;
  int32_t            m_facility;
  static uint32_t    ms_instances;
  static std::mutex  ms_mutex;
};

}}

#endif // !CORE_LOG_SYSLOGAPPENDER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:

//  LocalWords:  closelog
