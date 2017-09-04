#ifndef  XTD_CORE_CONFIG_GRAMMAR_HH_
# define  XTD_CORE_CONFIG_GRAMMAR_HH_

# include <boost/preprocessor.hpp>
# include <boost/fusion/adapted.hpp>
# include <boost/spirit/include/qi.hpp>
# include <boost/spirit/include/support.hpp>
# include <string>
# include <types.hh>
# include <vector>
# include "config/types.hh"


BOOST_FUSION_ADAPT_STRUCT(xtd::config::impl::property,
                          (std::string, m_name)
                          (std::string, m_value)
                          (std::vector<std::string>, m_vars)
                          (std::vector<std::string>, m_envs)
                          (std::vector<std::string>, m_params));

BOOST_FUSION_ADAPT_STRUCT(xtd::config::impl::subsection,
                          (std::string, m_name)
                          (std::vector<xtd::config::impl::property>, m_properties));

BOOST_FUSION_ADAPT_STRUCT(xtd::config::impl::section,
                          (std::string, m_name)
                          (std::vector<xtd::config::impl::property>, m_properties)
                          (std::vector<xtd::config::impl::subsection>, m_subsections));

namespace xtd {
namespace config {
namespace impl {


template<typename Iterator>
class Grammar : public boost::spirit::qi::grammar<boost::spirit::line_pos_iterator<Iterator>, std::vector<section>(), types::skipper>
{
public:
  typedef boost::spirit::line_pos_iterator<Iterator> wrapped_iterator;

public:
  Grammar(void);

public:
  const error_info& getLastError(void) const;

public:
  static wrapped_iterator wrap(Iterator p_iter);

private:
  bool handleError(wrapped_iterator           p_first,
                   wrapped_iterator           p_last,
                   wrapped_iterator           p_pos,
                   const boost::spirit::info& p_info);

private:
  error_info                                                                        m_error;
  boost::spirit::qi::rule<wrapped_iterator, std::string(),          types::skipper> m_var;
  boost::spirit::qi::rule<wrapped_iterator, std::string(),          types::skipper> m_env;
  boost::spirit::qi::rule<wrapped_iterator, std::string(),          types::skipper> m_param;
  boost::spirit::qi::rule<wrapped_iterator, std::string(),          types::skipper> m_section_name;
  boost::spirit::qi::rule<wrapped_iterator, property(),             types::skipper> m_property;
  boost::spirit::qi::rule<wrapped_iterator, subsection(),           types::skipper> m_subsection;
  boost::spirit::qi::rule<wrapped_iterator, section(),              types::skipper> m_section;
  boost::spirit::qi::rule<wrapped_iterator, std::vector<section>(), types::skipper> m_start;
};

}}}

#endif // !XTD_CORE_CONFIG_GRAMMAR_HH_
