#ifndef XTD_COMMON_FORMAT_HH_
# define XTD_COMMON_FORMAT_HH_
# include <boost/format.hpp>
# include <string>
# include "types.hh"

namespace xtd {

/**
 ** @brief Regroups c++-style printf functions
 */
struct format
{

  /**
   ** @brief Applies given arguments to input format.
   ** @tparam Args Variable number of argument types
   ** @param p_fmt boost format
   ** @param p_args template variadic arguments to apply to p_fmt
   ** @throw boost::io::format_error on error
   ** @return resolved format
   */
  template<typename... Arguments>
  static string
  vargs(boost::format& p_fmt, Arguments&&... p_args)
  {
    int l_unroll[] {0, (p_fmt % std::forward<Arguments>(p_args), 0)...};
    static_cast<void>(l_unroll);
    return boost::str(p_fmt);
  }


  /**
   ** @brief Applies given arguments to input format.
   ** @tparam Args Variable number of argument types
   ** @param p_fmt Format string, compatible with boost::format
   ** @param p_args template variadic arguments to apply to p_fmt
   ** @throw boost::io::format_error on error
   ** @return resolved format
   */
  template<typename... Arguments>
  static string
  vargs(const string& p_fmt, Arguments&&... p_args)
  {
    boost::format l_format(p_fmt);
    return vargs(l_format, p_args...);
  }

  /**
   ** @brief Applies given arguments to input format, give adulterate result on error
   ** @tparam Args Variable number of argument types
   ** @param p_fmt Format string, compatible with boost::format
   ** @param p_args template variadic arguments to apply to p_fmt
   ** @return resolved format or modified format
   ** @details
   ** On error, the function returns the given p_fmt string concatenated to the caught
   ** boost::io::format_error.
   */
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
      l_result += "]";
      return l_result;
    }
  }
};


}

#endif // !XTD_COMMON_FORMAT_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
