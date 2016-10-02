#ifndef XTD_CORE_LOG_LOGTYPES_HH_
# define XTD_CORE_LOG_LOGTYPES_HH_
# include "types.hh"
# include "error.hh"
# include <boost/date_time/posix_time/posix_time.hpp>
# include <syslog.h>


/**
 ** @brief Handy macro giving location information of call site.
 ** @details
 ** Meant to be used as last argument of xtd::log's logging function. When
 ** given, created log record will have *location* information usable in
 ** @ref xtd::log::Formatter layouts.
 */
# define HERE __PRETTY_FUNCTION__,__FILE__,__LINE__


namespace xtd {

/**
 ** @brief Rich and flexible logging facilities
 */
namespace log {


/**
 ** @class xtd::log::log_error
 ** @brief thrown by xtd::log objects on configuration error
 */

/**
 ** @brief Defines class log_error
 */
xtd_error_class(log_error, error);


/**
 ** @brief Log record severity, value are mapped on syslog values
 */
enum class level : int32_t {
  emerg   = LOG_EMERG,   ///< system is unusable
  alert   = LOG_ALERT,   ///< action must be taken immediatly
  crit    = LOG_CRIT,    ///< critical conditions
  err     = LOG_ERR,     ///< error conditions
  warning = LOG_WARNING, ///< warning conditions
  notice  = LOG_NOTICE,  ///< normal, but significant, condition
  info    = LOG_INFO,    ///< informational message
  debug   = LOG_DEBUG    ///< debug-level message
 };



/**
 ** @brief Holds data of a log record
 */
struct Record
{
  Record(void):
    m_name(),
    m_module(),
    m_level(level::crit),
    m_format(),
    m_time(boost::posix_time::microsec_clock::local_time()),
    m_threadID(0),
    m_pid(0),
    m_ppid(0)
  { }

public:
  string                   m_name;     ///< Logger name that emitted the record
  string                   m_module;   ///< Module name
  level                    m_level;    ///< Log severity
  string                   m_format;   ///< Message format
  boost::posix_time::ptime m_time;     ///< Timestamp of record
  size_t                   m_threadID; ///< Thread identifier that emitted the record
  pid_t                    m_pid;      ///< Process id
  pid_t                    m_ppid;     ///< Parent process id
};


/**
 ** @brief Holds data of a log record formatted by a @ref Formatter
 */
struct FormattedRecord : public Record
{
public:
  /**
   ** @brief Construction from Parent class
   ** @param p_rec Parent class object
   */
  FormattedRecord(const Record& p_rec) :
    Record(p_rec),
    m_hasLoc(false)
  { }

  /**
   ** @brief Construction from Parent class
   */
  FormattedRecord(void) :
    FormattedRecord(Record())
  { }

public:
  bool   m_hasLoc;   ///< True if record was emitted with location informations, See @ref HERE
  string m_fulllog;  ///< Final formatted log message string
  string m_message;  ///< Message formatted with log arguments
  string m_function; ///< Function name
  string m_filename; ///< File name
  size_t m_line;     ///< File line
};


}}

#endif // !XTD_CORE_LOG_LOGTYPES_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
