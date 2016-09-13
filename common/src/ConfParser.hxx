#include <boost/spirit/include/classic.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>


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
  std::string             m_name;
  std::vector<property>   m_properties;
  std::vector<subsection> m_subsections;
};

struct sections
{
  std::vector<section> m_sections;
};


BOOST_FUSION_ADAPT_STRUCT(property,
                          (std::string, m_name)
                          (std::string, m_value));


BOOST_FUSION_ADAPT_STRUCT(subsection,
                          (std::string,            m_name)
                          (std::vector<property>,  m_properties));

BOOST_FUSION_ADAPT_STRUCT(section,
                          (std::string,             m_name)
                          (std::vector<property>,   m_properties)
                          (std::vector<subsection>, m_subsections));

BOOST_FUSION_ADAPT_STRUCT(sections,
                          (std::vector<section>, m_sections));

namespace xtd {



namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

template<typename Iterator>
struct config_parser : qi::grammar<Iterator, sections(), ascii::space_type>
{
public:
  config_parser(void);

private:
  qi::rule<Iterator, property(),   ascii::space_type> m_property;
  qi::rule<Iterator, subsection(), ascii::space_type> m_subsection;
  qi::rule<Iterator, section(),    ascii::space_type> m_section;
  qi::rule<Iterator, sections(),   ascii::space_type> m_start;
};


template<typename Iterator>
config_parser<Iterator>::config_parser(void) :
  config_parser<Iterator>::base_type(m_start)
{
  using boost::phoenix::at_c;
  using boost::phoenix::push_back;
  namespace qil = boost::spirit::qi::labels;

  m_property =
    *ascii::alnum >> ascii::char_(':') >> *ascii::alnum;

  m_subsection =
    /**/ ascii::char_('[')
    >>  *ascii::alnum            [at_c<0>(qil::_val) = qil::_1]
    >>   ascii::char_(']')

    >>   ascii::char_('{')
    >>   *m_property             [push_back(at_c<1>(qil::_val), qil::_1)]
    >>   ascii::char_('}');

  m_section =
    /**/ ascii::char_('[')
    >>   *ascii::alnum           [at_c<0>(qil::_val) = qil::_1]
    >>   ascii::char_(']')

    >>   ascii::char_('{')
    >> *(
         m_subsection          [push_back(at_c<2>(qil::_val), qil::_1)]
         |
         m_property            [push_back(at_c<1>(qil::_val), qil::_1)]
         )
    >> ascii::char_('}');

  m_start =
    *m_section                 [push_back(at_c<0>(qil::_val), qil::_1)];
}

}
