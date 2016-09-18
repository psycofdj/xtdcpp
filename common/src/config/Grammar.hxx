#ifndef XTD_COMMON_CONFIG_GRAMMAR_HXX_
# define XTD_COMMON_CONFIG_GRAMMAR_HXX_

namespace xtd {
namespace config {
namespace impl {

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

  auto add     = qil::_val += qil::_1;
  auto add_m0  = phx::at_c<0>(qil::_val) += qil::_1;
  auto add_m1  = phx::at_c<1>(qil::_val) += qil::_1;
  auto set_m0  = phx::at_c<0>(qil::_val)  = qil::_1;
  auto push_m1 = phx::push_back(phx::at_c<1>(qil::_val), qil::_1);
  auto push_m2 = phx::push_back(phx::at_c<2>(qil::_val), qil::_1);
  auto push_m3 = phx::push_back(phx::at_c<3>(qil::_val), qil::_1);
  auto push_m4 = phx::push_back(phx::at_c<4>(qil::_val), qil::_1);

  m_var =
    /**/ no_skip[boost::spirit::qi::string("${")      [ add ]]
    >    no_skip[+(alnum | char_('_') | char_('.'))   [ add ]]
    >    no_skip[char_('}')                           [ add ]]
    ;

  m_env =
    /**/ no_skip[boost::spirit::qi::string("$ENV{")   [ add ]]
    >    no_skip[+(alnum | char_('_') | char_('.'))   [ add ]]
    >    no_skip[char_('}')                           [ add ]]
    ;

  m_param =
    /**/ no_skip[boost::spirit::qi::string("$PARAM{") [ add ]]
    >    no_skip[+(alnum | char_('_') | char_('.'))   [ add ]]
    >    no_skip[char_('}')                           [ add ]]
    ;

  m_property =
    /**/ lexeme[+(alnum | char_('_'))                 [ add_m0 ]]
    > lit(':')
    > *blank
    >
    *(m_var                                           [ add_m1, push_m2 ]
      | m_env                                         [ add_m1, push_m3 ]
      | m_param                                       [ add_m1, push_m4 ]
      | no_skip[+(char_ - eol - lit('$'))             [ add_m1 ]]
      | no_skip[char_('$')                            [ add_m1 ]])
    >   +eol
    ;

  m_section_name =
    /**/ lit('[')
    >   *(alnum|char_('_'))                           [ add ]
    >    lit(']')
    >   *eol
    ;

  m_subsection =
    /**/ m_section_name                               [ set_m0 ]
    >    lit('{')
    >    *eol
    >    *m_property                                  [ push_m1 ]
    >    *eol
    >    lit('}')
    >    *eol
    ;

  m_section =
    /**/ m_section_name                               [ set_m0 ]
    >    lit('{')
    >   *eol
    >>  *(m_subsection [ push_m2 ] | m_property[ push_m1 ])
    >    lit('}')
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
  on_error<fail>(m_start, ll);
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
  wrapped_iterator  l_end  = std::next(p_pos, std::min(30ul, l_dist));

  l_token << p_info;
  m_error.line    = boost::spirit::get_line(p_pos);
  m_error.col     =  boost::spirit::get_column(p_first, p_pos);
  m_error.preview = string(p_pos, l_end);
  m_error.token   = l_token.str();
  return false;
}


}}}


#endif // XTD_COMMON_CONFIG_GRAMMAR_HXX_
