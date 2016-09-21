namespace xtd {
namespace log {

/**
 ** @page core_log Using xtd logging facility
 **
 ** [TOC]
 **
 ** # Introduction {#introduction}
 ** - why another log4cpp ?
 **   - replace ostream interface by variadic template args
 **   - more customization
 **   - native support of location in log records
 **   - more userfiendly documentation
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Simple example {#simple_example}
 **
 ** @include exemple1.cc
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # API Reference {#api_reference}
 **
 **
 ** ## General design and concepts {#desing_and_concepts}
 **
 ** Several objects are involved in XTD's logging facility :
 ** - @ref RootLogger                   : @copybrief RootLogger
 ** - @ref Logger                       : @copybrief Logger
 ** - Different kinds of @ref Appender  : @copybrief Appender
 ** - Different kinds of @ref Formatter : @copybrief Formatter
 ** - **filters** functors              : Allow/Disallow log records
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
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Loggers {#loggers}
 **
 ** ### Logger {#logger}
 **
 ** @copydoc Logger
 **
 ** ### RootLogger {#rootlogger}
 **
 ** @copydoc RootLogger
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Appenders {#appenders}
 **
 ** ### Appender {#appender}
 **
 ** @copydoc Appender
 **
 ** ### StreamAppender {#stream_appender}
 **
 ** @copydoc SyslogAppender
 **
 ** ### SyslogAppender {#syslog_appender}
 **
 ** @copydoc MemoryAppender
 **
 ** ### MemoryAppender {#memory_appender}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 ** ## Formatters {#formatters}
 ** ### Formatter  {#formatter}
 ** ### ColoredFormatter {#colored_formatter}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 ** ## Format Modifiers {#format_modifiers}
 ** ### AutoWidth {#auto_width}
 ** ### StyleByLevel {#syle_by_level}
 ** ### MatchMessage {#match_message}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 ** ## Configuration loader {#config_loader}
 ** # FAQ {#faq}
 */

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
