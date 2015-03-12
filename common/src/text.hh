#ifndef COMMON_TEXT_HH_
# define COMMON_TEXT_HH_
# include <boost/tuple/tuple.hpp>
# include "types.hh"

namespace xtd {
namespace text {

class xml
{
private:
  typedef boost::tuple<string, string, string> t_entity;
  typedef vector<t_entity>                     t_data;

public:
  static void   encode(string& p_str);
  static string encode_copy(const string& p_str);

private:
  static t_data initialize(void);

private:
  static t_data ms_entityList;
};

}}

#endif // !COMMON_TEXT_HH_
