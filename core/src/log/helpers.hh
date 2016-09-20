#ifndef XTD_CORE_LOG_HELPERS_HH_
# define XTD_CORE_LOG_HELPERS_HH_
# include "log/logtypes.hh"

namespace xtd {
namespace log {

bool        is_valid(string p_level);
bool        is_valid(const uint32_t& p_level);
level       from(const uint32_t& p_level);
level       from(string p_level);
uint32_t    to_value(level p_level);
string      to_string(level p_level);
RootLogger& getRoot(void);

template<typename ... Args>
void emerg(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void alert(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void crit(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void err(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void warning(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void info(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void notice(const string& p_module, const string& p_format, Args... p_args);
template<typename ... Args>
void debug(const string& p_module, const string& p_format, Args... p_args);
template<class TError, typename... Arguments>
void raise(const string& p_module, const string& p_format, Arguments&&... p_args);

}}

# include "log/helpers.hxx"

#endif // !XTD_CORE_LOG_HELPERS_HH_
