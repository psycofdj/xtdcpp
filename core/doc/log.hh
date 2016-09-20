namespace xtd {
namespace log {

/**
 ** @page core_log Using xtd logging facility
 **
 ** # Table of content
 ** # Introduction
 ** - why another log4cpp ?
 **   - replace ostream interface by variadic template args
 **   - more customization
 **   - native support of location in log records
 **   - more userfiendly documentation
 **
 ** # Simple example
 ** @include exemple1.cc
 ** # Reference
 ** ## General design and concepts
 **
 ** Several objects are involved in XTD's logging facility :
 ** - @ref RootLogger : @copybrief RootLogger
 ** - @ref Logger : @copybrief Logger
 ** - Different kinds of @ref Appender : @copybrief Appender
 ** - Different kinds of @ref Formatter : @copybrief Formatter
 ** - **filters** functors : Allow/Disallow log records
 ** - **format modifier** functors : Modify log format depending on record content
 ** - A @ref ConfLoader : Creates and initialize all theses object from config
 **
 **
 ** The following diagram shows how theses objects interact with each other :
 ** @image html log_uml.png
 **
 ** The next diagram show the full work flow of a log record, from user's call to
 ** to effective output of the log
 ** @image html log_design.png
 **
 ** ## Loggers
 **
 ** ### Logger
 ** @copydoc Logger
 **
 ** ### RootLogger
 ** @copydoc RootLogger
 **
 ** ## Appenders
 ** ### StreamAppender
 ** ### SyslogAppender
 ** ### MemoryAppender
 **
 **
 ** ## Formatters
 ** ### Formatter
 ** ### ColoredFormatter
 **
 **
 ** ## Format Modifiers
 ** ### AutoWidth
 ** ### StyleByLevel
 ** ### MatchMessage
 ** ## Configuration loader
 ** # FAQ
 */

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
