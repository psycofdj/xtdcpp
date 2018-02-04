// IWYU pragma: private, include "log/helpers.hh"
#ifndef XTD_CORE_LOG_HELPERS_HXX_
# define XTD_CORE_LOG_HELPERS_HXX_
# include <boost/format.hpp>
# include <exception>
# include "types.hh"
# include "format.hh"
# include "log/Logger.hh"

namespace xtd {
namespace log {

template<typename ... Args>
void emerg(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).emerg(p_format, p_args...);
}

template<typename ... Args>
void alert(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).alert(p_format, p_args...);
}

template<typename ... Args>
void crit(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).crit(p_format, p_args...);
}

template<typename ... Args>
void err(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).err(p_format, p_args...);
}

template<typename ... Args>
void warning(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).warning(p_format, p_args...);
}

template<typename ... Args>
void info(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).info(p_format, p_args...);
}

template<typename ... Args>
void notice(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).notice(p_format, p_args...);
}

template<typename ... Args>
void debug(const string& p_module, const string& p_format, Args... p_args)
{
  getRoot().get(p_module).debug(p_format, p_args...);
}


template<class TError, typename... Arguments>
void raise(const string& p_module, const string& p_format, Arguments&&... p_args)
{
  log::crit(p_module, p_format, p_args...);

  string l_message;

  try {
    boost::format l_format(p_format);
    size_t        l_fmtArgsCount = l_format.expected_args();
    size_t        l_argsCount    = sizeof...(Arguments);

    if (l_fmtArgsCount == l_argsCount) {
      l_message = format::vargs(l_format, p_args...);
    }
    else if (l_fmtArgsCount + 3 == l_argsCount) {
      l_message = format::vargs(p_format + " in %s:%s:%d", p_args...);
    } else {
      l_message = "unable to render format : " + p_format + " : wrong number of arguments";
    }
  } catch (std::exception& l_error) {
    l_message = "unable to render format : " + p_format + " : " + l_error.what();
  }

  throw TError(format::vargs_noexcept(l_message, p_args...));

}


}}

#endif // !XTD_CORE_LOG_HELPERS_HXX_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
