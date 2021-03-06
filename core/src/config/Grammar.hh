#ifndef  XTD_CORE_CONFIG_GRAMMAR_HH_
# define  XTD_CORE_CONFIG_GRAMMAR_HH_
# include <string>
# include <vector>
# include <boost/fusion/adapted.hpp>
# include <boost/preprocessor.hpp>
# include <boost/spirit/include/support.hpp>
# include <boost/spirit/include/classic.hpp>                   // IWYU pragma: keep
# include <boost/spirit/include/qi.hpp>                        // IWYU pragma: keep
# include <boost/spirit/include/qi_core.hpp>                   // IWYU pragma: keep
# include <boost/spirit/include/qi_eol.hpp>                    // IWYU pragma: keep
# include <boost/spirit/include/phoenix_core.hpp>              // IWYU pragma: keep
# include <boost/spirit/include/phoenix_operator.hpp>          // IWYU pragma: keep
# include <boost/spirit/include/phoenix_fusion.hpp>            // IWYU pragma: keep
# include <boost/spirit/include/phoenix_stl.hpp>               // IWYU pragma: keep
# include <boost/spirit/include/phoenix_object.hpp>            // IWYU pragma: keep
# include <boost/spirit/include/support_line_pos_iterator.hpp> // IWYU pragma: keep
# include <boost/spirit/include/phoenix_core.hpp>              // IWYU pragma: keep
# include <boost/spirit/include/phoenix_fusion.hpp>            // IWYU pragma: keep
# include <boost/spirit/include/phoenix_algorithm.hpp>         // IWYU pragma: keep
# include <boost/spirit/include/phoenix_bind.hpp>              // IWYU pragma: keep
# include <boost/spirit/include/phoenix_container.hpp>         // IWYU pragma: keep
# include <boost/spirit/include/phoenix_function.hpp>          // IWYU pragma: keep
# include <boost/spirit/include/phoenix_limits.hpp>            // IWYU pragma: keep
# include <boost/spirit/include/phoenix_object.hpp>            // IWYU pragma: keep
# include <boost/spirit/include/phoenix_operator.hpp>          // IWYU pragma: keep
# include <boost/spirit/include/phoenix_scope.hpp>             // IWYU pragma: keep
# include <boost/spirit/include/phoenix_statement.hpp>         // IWYU pragma: keep
# include <boost/spirit/include/phoenix_stl.hpp>               // IWYU pragma: keep
# include <boost/fusion/include/adapt_struct.hpp>              // IWYU pragma: keep
# include <types.hh>                                           // IWYU pragma: keep
# include "config/types.hh"                                    // IWYU pragma: keep


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
