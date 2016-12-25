#ifndef XTD_CORE_LOG_FWD_HH_
# define XTD_CORE_LOG_FWD_HH_
# include "types.hh"

namespace xtd {
namespace log {

class Stream;
class Appender;
class SyslogAppender;
class MemoryAppender;
class StreamAppender;
class Formatter;
class ColoredFormatter;
class ConfLoader;
class Logger;
class RootLogger;

template<typename T> class Fields;

}}

#endif // !XTD_CORE_LOG_FWD_HH_
