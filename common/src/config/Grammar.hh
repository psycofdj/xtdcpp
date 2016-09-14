#ifndef  XTD_COMMON_CONFIG_GRAMMAR_HH_
# define  XTD_COMMON_CONFIG_GRAMMAR_HH_
#include <types.hh>        // libcommon
#include "config/types.hh" // libcommon
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

namespace xtd {
namespace config {


typedef types::iterator iter;

template<typename Iterator, typename Skipper>
class Grammar : boost::spirit::qi::grammar<Iterator, sections(), Skipper>
{
public:
  Grammar(void);

public:
  struct error_info
  {
    size_t line;
    size_t col;
    string preview;
    string token;
  };

public:
  const error_info& getLastError(void) const;

private:
  bool handleError(Iterator                   p_first,
                   Iterator                   p_last,
                   Iterator                   p_pos,
                   const boost::spirit::info& p_info);

private:
  error_info                                               m_error;
  boost::spirit::qi::rule<Iterator, string(),     Skipper> m_key;
  boost::spirit::qi::rule<Iterator, string(),     Skipper> m_val;
  boost::spirit::qi::rule<Iterator, string(),     Skipper> m_section_name;
  boost::spirit::qi::rule<Iterator, property(),   Skipper> m_property;
  boost::spirit::qi::rule<Iterator, subsection(), Skipper> m_subsection;
  boost::spirit::qi::rule<Iterator, section(),    Skipper> m_section;
  boost::spirit::qi::rule<Iterator, sections(),   Skipper> m_start;
};

}}


namespace xtd {
namespace config {

template<typename Iterator, typename Skipper>
Grammar<Iterator, Skipper>::Grammar(void) :
  Grammar<Iterator, Skipper>::base_type(m_start, "sections")
{
  namespace ascii = boost::spirit::ascii;
  namespace qi    = boost::spirit::qi;
  namespace qil   = boost::spirit::qi::labels;
  namespace phx   = boost::phoenix;

  m_key = qi::lexeme[+(ascii::alnum)                      [ qil::_val += qil::_1 ]];
  m_val = qi::lexeme[+(ascii::char_ - boost::spirit::eol) [ qil::_val += qil::_1 ]];

  m_property =
    /**/ m_key
    >    ascii::char_(':')
    >    m_val
    >   +boost::spirit::eol
    ;

  m_section_name =
    /**/ qi::lit('[')
    >   *ascii::alnum           [ qil::_val += qil::_1 ]
    >    qi::lit(']')
    >   *boost::spirit::eol
    ;

  m_subsection =
    /**/ m_section_name         [phx::at_c<0>(qil::_val) = qil::_1]
    >    qi::lit('{')
    >    *boost::spirit::eol
    >    *m_property            [phx::push_back(phx::at_c<1>(qil::_val), qil::_1)]
    >    *boost::spirit::eol
    >    qi::lit('}')
    >    *boost::spirit::eol
    ;

  m_section =
    /**/ m_section_name         [phx::at_c<0>(qil::_val) = qil::_1]
    >     qi::lit('{')
    >   *boost::spirit::eol
    >>  *(
          m_subsection          [phx::push_back(phx::at_c<2>(qil::_val), qil::_1)]
          |
          m_property            [phx::push_back(phx::at_c<1>(qil::_val), qil::_1)]
          )
    >     qi::lit('}')
    >   *boost::spirit::eol
    ;

  m_start =
    /**/ *boost::spirit::eol
    >    *m_section             [phx::push_back(phx::at_c<0>(qil::_val), qil::_1)]
    >
    *boost::spirit::eol;



  m_key.name("key");
  m_val.name("value");
  m_property.name("property");
  m_start.name("sections");
  m_section.name("section");
  m_subsection.name("subsection");

  auto ll =  phx::bind(&Grammar<Iterator,Skipper>::handleError, this, qil::_1, qil::_2, qil::_3, qil::_4);
  qi::on_error<qi::fail>(m_section, ll);
}


template<typename Iterator, typename Skipper>
const typename Grammar<Iterator, Skipper>::error_info&
Grammar<Iterator, Skipper>::getLastError(void) const
{
  return m_error;
}

template<typename Iterator, typename Skipper>
bool
Grammar<Iterator, Skipper>::handleError(Iterator                   p_first,
                                        Iterator                   p_last,
                                        Iterator                   p_pos,
                                        const boost::spirit::info& p_info)
{
  std::stringstream l_token;
  std::size_t       l_dist = std::distance(p_pos, p_last);
  Iterator          l_end  = std::next(p_pos, std::min(10ul, l_dist));
  l_token << p_info;
  m_error.line = get_line(p_pos);
  m_error.col = get_column(p_first, p_pos);
  m_error.preview = std::string(p_pos, l_end);
  m_error.token   = l_token.str();
  return false;
}


}}


#endif // !XTD_COMMON_CONFIG_GRAMMAR_HH_
