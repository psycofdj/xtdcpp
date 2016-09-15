#ifndef XTD_COMMON_FORMAT_HH_
# define XTD_COMMON_FORMAT_HH_
# include <boost/format.hpp>
# include <string>
# include "types.hh"

namespace xtd {

struct format
{
  template<typename... Arguments>
  static string
  vargs(string const& p_fmt, Arguments&&... p_args)
  {

    boost::format l_format(p_fmt);
    int l_unroll[] {0, (l_format % std::forward<Arguments>(p_args), 0)...};
    static_cast<void>(l_unroll);
    return boost::str(l_format);
  }

  template<typename... Arguments>
  static string
  vargs_noexcept(string const& p_fmt, Arguments&&... p_args)
  {
    try {
      return vargs(p_fmt, p_args...);
    } catch (boost::io::format_error& l_error) {
      string l_result;
      l_result += p_fmt;
      l_result += " [error while resolving format : ";
      l_result += l_error.what();
      l_result == "]";
      return l_result;
    }
  }
};


}

#endif // !XTD_COMMON_FORMAT_HH_
