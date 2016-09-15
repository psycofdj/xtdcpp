#ifndef XTD_COMMON_CONFIG_GRAMMAR_HXX_
# define XTD_COMMON_CONFIG_GRAMMAR_HXX_

namespace xtd {
namespace config {

template<typename Iterator>
Grammar<Iterator>::Grammar(void) :
  Grammar<Iterator>::base_type(m_start)
{
  namespace qil   = boost::spirit::qi::labels;
  namespace phx   = boost::phoenix;
  using boost::spirit::ascii::alnum;
  using boost::spirit::ascii::blank;
  using boost::spirit::ascii::char_;
  using boost::spirit::eol;
  using boost::spirit::qi::lit;
  using boost::spirit::qi::lexeme;
  using boost::spirit::qi::on_error;
  using boost::spirit::qi::fail;
  using boost::spirit::qi::no_skip;


  m_var =
    /**/ no_skip[boost::spirit::qi::string("${")       [ qil::_val += qil::_1 ]]
    >    no_skip[+(alnum)   [ qil::_val += qil::_1 ]]
    >    no_skip[char_('}') [ qil::_val += qil::_1 ]]
    ;

  m_env =
    /**/ no_skip[boost::spirit::qi::string("$ENV{")    [ qil::_val += qil::_1 ]]
    >    no_skip[+(alnum)   [ qil::_val += qil::_1 ]]
    >    no_skip[char_('}') [ qil::_val += qil::_1 ]]
    ;

  m_param =
    /**/ no_skip[boost::spirit::qi::string("$PARAM{")  [ qil::_val += qil::_1 ]]
    >    no_skip[+(alnum)   [ qil::_val += qil::_1 ]]
    >    no_skip[char_('}') [ qil::_val += qil::_1 ]]
    ;

  m_property =
    /**/ lexeme[+(alnum)        [ phx::at_c<0>(qil::_val) += qil::_1 ]]
    > lit(':')
    > *blank
    >
    *(
      m_var                    [ phx::at_c<1>(qil::_val) +=  qil::_1,
                                 phx::push_back(phx::at_c<2>(qil::_val), qil::_1) ]
      |
      m_env                    [ phx::at_c<1>(qil::_val) += qil::_1,
                                 phx::push_back(phx::at_c<3>(qil::_val), qil::_1) ]
      |
      m_param                  [ phx::at_c<1>(qil::_val) += qil::_1,
                                 phx::push_back(phx::at_c<4>(qil::_val), qil::_1) ]
      |
      no_skip[+(char_ - eol - lit('$')) [ phx::at_c<1>(qil::_val) += qil::_1 ]]
      |
      no_skip[char_('$')                       [ phx::at_c<1>(qil::_val) += qil::_1 ]]
      )
    >   +eol
    ;

  m_section_name =
    /**/ lit('[')
    >   *alnum                  [ qil::_val += qil::_1 ]
    >    lit(']')
    >   *eol
    ;

  m_subsection =
    /**/ m_section_name         [phx::at_c<0>(qil::_val) = qil::_1]
    >    lit('{')
    >    *eol
    >    *m_property            [phx::push_back(phx::at_c<1>(qil::_val), qil::_1)]
    >    *eol
    >    lit('}')
    >    *eol
    ;

  m_section =
    /**/ m_section_name         [phx::at_c<0>(qil::_val) = qil::_1]
    >     lit('{')
    >   *eol
    >>  *(
          m_subsection          [phx::push_back(phx::at_c<2>(qil::_val), qil::_1)]
          |
          m_property            [phx::push_back(phx::at_c<1>(qil::_val), qil::_1)]
          )
    >     lit('}')
    >    *eol
    ;

  m_start =
    /**/ *eol
    >    *m_section             [phx::push_back(qil::_val, qil::_1)]
    >    *eol
    ;

  m_var.name("variable");
  m_property.name("property");
  m_start.name("sections");
  m_section.name("section");
  m_subsection.name("subsection");

  auto ll =  phx::bind(&Grammar<Iterator>::handleError, this, qil::_1, qil::_2, qil::_3, qil::_4);
  on_error<fail>(m_section, ll);
}


template<typename Iterator>
const error_info&
Grammar<Iterator>::getLastError(void) const
{
  return m_error;
}

template<typename Iterator>
typename Grammar<Iterator>::wrapped_iterator
Grammar<Iterator>::wrap(Iterator p_iter)
{
  return wrapped_iterator(p_iter);
}

template<typename Iterator>
bool
Grammar<Iterator>::handleError(wrapped_iterator           p_first,
                               wrapped_iterator           p_last,
                               wrapped_iterator           p_pos,
                               const boost::spirit::info& p_info)
{
  std::stringstream l_token;
  size_t            l_dist = std::distance(p_pos, p_last);
  wrapped_iterator  l_end  = std::next(p_pos, std::min(10ul, l_dist));

  l_token << p_info;
  m_error.line    = boost::spirit::get_line(p_pos);
  m_error.col     =  boost::spirit::get_column(p_first, p_pos);
  m_error.preview = string(p_pos, l_end);
  m_error.token   = l_token.str();
  return false;
}


}}


#endif // XTD_COMMON_CONFIG_GRAMMAR_HXX_
