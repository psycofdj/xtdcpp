#ifndef XTD_CORE_LOG_HELPERS_HH_
# define XTD_CORE_LOG_HELPERS_HH_
# include "log/logtypes.hh"
# include "log/fwd.hh"

namespace xtd {
namespace log {

/**
 ** @brief Converts given @ref level to underlying value
 ** @param p_level @ref level
 ** @return underlying value, compatible to syslog
 */
std::uint32_t to_value(level p_level);

/**
 ** @brief Converts given @ref level to string label
 ** @param p_level @ref level
 ** @return label string
 */
std::string to_string(level p_level);

/**
 ** @brief Tells if given string is a valid @ref level label
 ** @param p_level @ref level label string
 ** @return true is given label is valid
 */
bool is_valid(const std::string& p_level);

/**
 ** @brief Tells if given integer is a valid @ref level underlying value
 ** @param p_level @ref level underlying value
 ** @return true is given value is valid
 */
bool is_valid(const std::uint32_t& p_level);

/**
 ** @brief Converts given underlying value to @ref level
 ** @param p_level underlying @ref level value
 ** @return corresponding @ref level, @ref level::crit if value is invalid
 */
level from(const std::uint32_t& p_level);

/**
 ** @brief Converts given label to @ref level
 ** @param p_level @ref level label
 ** @return corresponding @ref level, @ref level::crit if label is invalid
 */
level from(const std::string& p_level);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::emerg severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void emerg(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::emerg severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void alert(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::crit severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void crit(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::err severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void err(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::warning severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void warning(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::info severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void info(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::notice severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void notice(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Logs to global @ref RootLogger with @ref level::debug severity
 ** @tparam Args Variadic argument types
 ** @param p_module module to send log to
 ** @param p_format log message format
 ** @param p_args arguments to apply to message format
 */
template<typename ... Args>
void debug(const string& p_module, const string& p_format, Args... p_args);

/**
 ** @brief Throws given exception class with formatted message and logs with @ref level::crit
 ** @tparam TError Exception class to throw
 ** @tparam Args Variadic arguments type
 ** @param p_module module to log to
 ** @param p_format message format
 ** @param p_args arguments to apply to message format
 */
template<class TError, typename... Args>
void raise(const string& p_module, const string& p_format, Args&&... p_args);


/**
 ** @brief Returns (or create) global @ref RootLogger instance
 ** @return Global @ref RootLogger instance
 */
RootLogger& getRoot(void);

}}

# include "log/helpers.hxx"

#endif // !XTD_CORE_LOG_HELPERS_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
