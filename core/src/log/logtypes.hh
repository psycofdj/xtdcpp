#ifndef XTD_CORE_LOG_LOGTYPES_HH_
# define XTD_CORE_LOG_LOGTYPES_HH_
# define HERE __PRETTY_FUNCTION__,__FILE__,__LINE__
# include "types.hh"
# include "error.hh"
# include <boost/date_time/posix_time/posix_time.hpp>
# include <syslog.h>

namespace xtd {

/**
 ** @brief Rich and flexible logging facilities
 */
namespace log {

xtd_error_class(log_error, error);

class Formatter;
class Appender;
class Logger;
class RootLogger;
class Record;
class FormattedRecord;

enum class level : int32_t {
  emerg   = LOG_EMERG,
  alert   = LOG_ALERT,
  crit    = LOG_CRIT,
  err     = LOG_ERR,
  warning = LOG_WARNING,
  notice  = LOG_NOTICE,
  info    = LOG_INFO,
  debug   = LOG_DEBUG
 };

struct Record
{
  string                   m_name;
  string                   m_module;
  level                    m_level;
  string                   m_format;
  boost::posix_time::ptime m_time;
  size_t                   m_threadID;
};

struct FormattedRecord : public Record
{
public:
  FormattedRecord(const Record& p_rec) :
    Record(p_rec),
    m_hasLoc(false)
  { }

public:
  bool   m_hasLoc;
  string m_fulllog;
  string m_message;
  string m_function;
  string m_filename;
  size_t m_line;
  pid_t  m_pid;
  pid_t  m_ppid;
};


}}

#endif // !XTD_CORE_LOG_LOGTYPES_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
