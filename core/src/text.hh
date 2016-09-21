#ifndef COMMON_TEXT_HH_
# define COMMON_TEXT_HH_
# include <boost/tuple/tuple.hpp>
# include "types.hh"


namespace xtd {

/**
 ** @brief Various text manipulation routines
 */
namespace text {

/**
 ** @brief Helps with XML manipulation
 */
class xml
{
private:
  typedef boost::tuple<string, string> t_entity;
  typedef vector<t_entity>             t_data;

public:
  /**
   ** @brief Encode entities from p_str (inplace)
   ** @param p_str input string to encode
   ** @details
   ** Replaces all xml related characters from p_str to xml entities.
   **
   ** Ex: "<here> isn't it ?" becomes "&lt;here&gt;isn&quot;t it ?"
   **
   ** List of replaced entities:
   ** - @<
   ** - @>
   ** - &
   ** - "
   ** - '
   */
  static void   encode(string& p_str);

  /**
   ** @brief Same as @ref encode but returns a copy of input string
   */
  static string encode_copy(const string& p_str);

private:
  static t_data initialize(void);

private:
  static t_data ms_entityList;
};

}}

#endif // !COMMON_TEXT_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
