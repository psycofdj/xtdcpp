namespace xtd {
namespace log {
/**
 ** @page core_log Logging facility
 ** @brief This page shows how to use XTD's logging facility
 **
 **
 ** [TOC]
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Introduction {#introduction}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** XTD's logging facility privides a set of classes for flexible logging to files,
 ** syslog or other destinations.
 **
 ** It was modeled like the [log4cpp](http://log4cpp.sourceforge.net/) C++ library
 ** which is itself very close to the [Log4j](http://logging.apache.org/log4j/)
 ** Java library.
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Features {#features}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** The next sections describes the key differences between XTD's logging and
 ** log4cpp.
 **
 ** ## C++ Variadic template interface {#intro_variadic}
 **
 **  XTD's logging replaces log4cpp's stream-based and C-variadic interface by a
 **  modern C++ variadic template interface.
 **
 **  C-variadic only allows to pass POD (Plain Old Data) arguments  which leads
 **  to many ugly *c_str()* calls for strings.
 **
 **  On the other hand, stream-based interface breaks the readability of the
 **  whole message format, especially when the log record has many arguments.<br/>
 **
 **  The C++ modern variadic arguments interface allow any kind of argument type
 **  which must, at the end, be convertible to string.
 **
 **  That means that strings can be used as they are but also any other user
 **  type that implements the stream <i> operator@<@< </i>.
 **
 **  @code
 **  {
 **    // log4cpp
 **    std::string        l_name = "filename";
 **    log4cpp::Category& l_root = log4cpp::Category::getRoot();
 **
 **    l_root.error("error while opening file '%s'", l_name.c_str());
 **    l_root << log4cpp::Priority::ERROR
 **           << "error while opening file '"
 **           << l_name
 **           << "'";
 **  }
 **
 **  {
 **    // XTD's logging
 **    std::string  l_name = "filename";
 **    log::Logger& l_root = log::getRoot();
 **
 **    l_root.err("error while opening file '%s'", l_name);
 **  }
 **  @endcode
 **
 **
 ** ## Call site location {#intro_callsite}
 **
 ** XTD's logging has a native support of where the log record are generated in
 ** the code.
 **
 ** File name, line number and function name are given to the logger
 ** through a macro and these informations are stored in the log record.
 **
 ** Using the formatting layout, location can later be displayed or hidden
 ** depending on your needs.
 **
 ** A typical use-case is to activate location in the layout in a debugging process
 ** which is far more convenient than grepping the message format in the code.
 **
 ** @code
 ** log::Logger& l_root = log::getRoot();
 **
 ** l_root.err("error while opening file '%s'", l_name, HERE);
 ** @endcode
 **
 ** ## Flexible configuration loader {#intro_flexible}
 **
 ** In contrary to log4cpp, XTD's logging allows to register user-defined class
 ** to its configuration loading system.
 **
 ** ## Memory managment {#intro_memory}
 **
 ** log4cpp lacks a properly defined memory management model. Naked allocated
 ** objects are passed to loggers and appender and we have little details about
 ** their life-cycle.
 **
 ** XTD's logging makes a extensive use of shared pointers. User may shared instances
 ** of formatters and appender among different loggers without worrying about
 ** deleting objects
 **
 ** ## User friendly documentation {#intro_documentation}
 **
 ** Last but not least, XTD's logging provides a detailed API documentation where
 ** log4cpp often forces to either a try-and-fail or read-the-code learning process.
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Example {#simple_example}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** @include exemple1.cc
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # General design and concepts {#desing_and_concepts}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Main objects {#log_objects}
 **
 ** Several objects are involved in XTD's logging facility :
 ** - @ref RootLogger                   : @copybrief RootLogger
 ** - @ref Logger                       : @copybrief Logger
 ** - Different kinds of @ref Appender  : @copybrief Appender
 ** - Different kinds of @ref Formatter : @copybrief Formatter
 ** - **filters** functors              : Allow/Disallow log records
 ** - **format modifier** functors      : Modify log format depending on record content
 ** - @ref ConfLoader                   : @copybrief ConfLoader
 **
 ** ## Composition {#log_composition}
 **
 ** The following diagram shows how these objects interact with each others :
 ** @image html log_uml.png
 **
 ** ## Work flow {#log_flow}
 **
 ** The next diagram show the full work flow of a log record :
 ** - from user's call
 ** - to the effective output
 **
 ** @image html log_design.png
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Loggers {#loggers}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Logger {#loggers_logger}
 **
 ** @copydoc Logger
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## RootLogger {#loggers_rootlogger}
 **
 ** @copydoc RootLogger
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Appenders {#appenders}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Appender {#appenders_appender}
 **
 ** @copydoc Appender
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## SyslogAppender {#appenders_syslog_appender}
 **
 ** @copydoc SyslogAppender
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## StreamAppender {#appenders_stream_appender}
 **
 ** @copydoc StreamAppender
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## MemoryAppender {#appenders_memory_appender}
 **
 ** @copydoc MemoryAppender
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 ** # Formatters {#formatters}
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Formatter  {#formatters_formatter}
 **
 ** @copydoc Formatter
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## ColoredFormatter {#formatters_colored_formatter}
 **
 ** @copydoc ColoredFormatter
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 ** # Format and Styles Modifiers {#modifiers}
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## AutoWidth {#modifiers_auto_width}
 **
 ** @copydoc AutoWidth
 ** See @ref AutoWidth
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## StyleByLevel {#modifiers_syle_by_level}
 **
 ** @copydoc StyleByLevel
 ** See @ref StyleByLevel
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## StyleMatch {#modifiers_match_message}
 **
 ** @copydoc StyleMatch
 ** See @ref StyleMatch
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Configuration loader {#modifiers_config_loader}
 **
 ** @copydoc ConfLoader
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Helper functions {#helper_functions}
 **
 **
 ** - @ref level conversions :
 **   - @ref is_valid(const std::string&)   : @copybrief is_valid(const std::string&)
 **   - @ref is_valid(const std::uint32_t&) : @copybrief is_valid(const std::uint32_t&)
 **   - @ref from(const std::string&)       : @copybrief from(const std::string&)
 **   - @ref from(const std::uint32_t&)     : @copybrief from(const std::uint32_t&)
 **   - @ref to_value(level)                : @copybrief to_value(level)
 **   - @ref to_string(level)               : @copybrief to_string(level)
 **
 **
 **
 ** - logging functions :
 **   - @ref emerg   "log::emerg(module, format, args...)" :
 **      - @copybrief emerg
 **   - @ref alert   "log::alert(module, format, args...)" :
 **      - @copybrief alert
 **   - @ref crit    "log::crit(module, format, args...)" :
 **      - @copybrief crit
 **   - @ref err     "log::err(module, format, args...)" :
 **      - @copybrief err
 **   - @ref warning "log::warning(module, format, args...)" :
 **      - @copybrief warning
 **   - @ref info    "log::info(module, format, args...)" :
 **      - @copybrief info
 **   - @ref notice  "log::notice(module, format, args...)" :
 **      - @copybrief notice
 **   - @ref debug   "log::debug(module, format, args...)" :
 **      - @copybrief debug
 **
 **
 **
 ** - throw exception :
 **   - @ref raise "log::raise<Error>(module, format, args...)" : @copybrief raise
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:

//  LocalWords:  TOC
