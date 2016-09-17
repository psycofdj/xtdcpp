#ifndef XTD_COMMON_CONFIG_PARSER_HXX_
# define XTD_COMMON_CONFIG_PARSER_HXX_
# include <boost/lexical_cast.hpp>
# include <boost/algorithm/string/compare.hpp>
# include <boost/range/algorithm/find_if.hpp>
# include "config/types.hh"
# include "config/Grammar.hh"
# include "error.hh"
# include "logger.hh"

namespace xtd {
namespace config {


template<typename T>
status
Parser::get(const string& p_name, T& p_val) const
{
  t_data::const_iterator c_item;
  if (m_data.end() == (c_item = m_data.find(p_name)))
  {
    logger::info("common.config", "key '%s' not found", p_name, HERE);
    return status::notfound;
  }

  try {
    p_val = boost::lexical_cast<T>(c_item->second);
  } catch (boost::bad_lexical_cast&) {
    logger::info("common.config", "key '%s' found but not lexicaly castable to %s",
                 p_name,  boost::typeindex::type_id<T>().pretty_name(), HERE);
    return status::error;
  }
  return status::ok;
}

template<typename T>
status
Parser::get_default(const string& p_key, T& p_dest, const T& p_default) const
{
  if (status::ok != get(p_key, p_dest))
  {
    p_dest = p_default;
    return status::notfound;
  }
  return status::ok;
}

template<typename Iterator>
status
Parser::parse(Iterator p_begin, Iterator p_end)
{
  typedef Grammar<Iterator> TGrammar;

  auto            l_cur = TGrammar::wrap(p_begin);
  auto            l_end = TGrammar::wrap(p_end);
  TGrammar        l_grammar;
  error_info      l_error;
  bool            l_ret;
  vector<section> l_sections;

  l_ret = phrase_parse(l_cur, l_end, l_grammar, boost::spirit::ascii::blank, l_sections);
  if ((false == l_ret) || (l_cur != l_end))
  {
    l_error = l_grammar.getLastError();
    logger::crit("common.config", "parse error at (line %d, column %d). expecting token '%s' near '%s'",
                 l_error.line, l_error.col, l_error.token, l_error.preview, HERE);
    return status::error;
  }

  return translate(l_sections, m_data);
}


}}

#endif // !XTD_COMMON_CONFIG_PARSER_HXX_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
