#ifndef  XTD_COMMON_CONFIG_TYPES_HH_
# define  XTD_COMMON_CONFIG_TYPES_HH_
# include <boost/spirit/include/support_line_pos_iterator.hpp>
# include <boost/spirit/include/qi_char_class.hpp>

// namespace xtd {
// namespace config {

struct types
{
  typedef boost::spirit::line_pos_iterator<std::string::const_iterator> iterator;
  typedef decltype(boost::spirit::ascii::blank)                         skipper;
};

struct property
{
  std::string m_name;
  std::string m_value;
};

struct subsection
{
  std::string           m_name;
  std::vector<property> m_properties;
};

struct section
{
  string             m_name;
  std::vector<property>   m_properties;
  std::vector<subsection> m_subsections;
};

struct sections
{
  std::vector<section> m_sections;
};

// }}


BOOST_FUSION_ADAPT_STRUCT(property,
                          (std::string, m_name)
                          (std::string, m_value));


BOOST_FUSION_ADAPT_STRUCT(subsection,
                          (std::string,                        m_name)
                          (std::vector<property>, m_properties));

BOOST_FUSION_ADAPT_STRUCT(section,
                          (std::string,                          m_name)
                          (std::vector<property>,   m_properties)
                          (std::vector<subsection>, m_subsections));

BOOST_FUSION_ADAPT_STRUCT(sections,
                          (std::vector<section>, m_sections));

#endif // XTD_COMMON_CONFIG_TYPES_HH_
