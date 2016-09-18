#ifndef  XTD_COMMON_CONFIG_TYPES_HH_
# define  XTD_COMMON_CONFIG_TYPES_HH_
# include <string>
# include <vector>
# include <boost/spirit/include/support_line_pos_iterator.hpp>
# include <boost/spirit/include/qi_char_class.hpp>

namespace xtd {
namespace config {
namespace impl {

struct types
{
  typedef boost::spirit::line_pos_iterator<std::string::const_iterator> iterator;
  typedef decltype(boost::spirit::ascii::blank)                         skipper;
};

struct error_info
{
  size_t      line;
  size_t      col;
  std::string preview;
  std::string token;
};

struct property
{
  std::string              m_name;
  std::string              m_value;
  std::vector<std::string> m_vars;
  std::vector<std::string> m_envs;
  std::vector<std::string> m_params;
};

struct subsection
{
  std::string           m_name;
  std::vector<property> m_properties;
};

struct section
{
  std::string             m_name;
  std::vector<property>   m_properties;
  std::vector<subsection> m_subsections;
};


}}}

#endif // XTD_COMMON_CONFIG_TYPES_HH_
