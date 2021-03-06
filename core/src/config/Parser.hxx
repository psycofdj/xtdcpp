#ifndef XTD_CORE_CONFIG_PARSER_HXX_
# define XTD_CORE_CONFIG_PARSER_HXX_
# include <boost/lexical_cast.hpp>
# include <boost/spirit/include/support.hpp>
# include "types.hh"
# include "config/Grammar.hh"
# include "config/types.hh"
# include "log/helpers.hh"
# include "log/logtypes.hh"

namespace xtd {
namespace config {


template<typename T>
status
Parser::get(const string& p_name, T& p_val) const
{
  string l_value;
  status l_ret;

  if (status::ok != (l_ret = get(p_name, l_value)))
    return l_ret;
  try {
    p_val = boost::lexical_cast<T>(l_value);
  } catch (boost::bad_lexical_cast&) {
    string l_name = "unknown type";
#if DBOOST_MINOR_VERSION
# if DBOOST_MINOR_VERSION > 56
    l_name = boost::typeindex::type_id<T>().pretty_name();
# endif
#endif
    log::info("core.config", "key '%s' found but '%s' is lexically castable to %s",
              p_name,  l_value, l_name, HERE);
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
  typedef impl::Grammar<Iterator> TGrammar;

  auto                  l_cur = TGrammar::wrap(p_begin);
  auto                  l_end = TGrammar::wrap(p_end);
  TGrammar              l_grammar;
  impl::error_info      l_error;
  bool                  l_ret;
  vector<impl::section> l_sections;

  m_data.clear();
  l_ret = phrase_parse(l_cur, l_end, l_grammar, boost::spirit::ascii::blank, l_sections);
  if ((false == l_ret) || (l_cur != l_end))
  {
    l_error = l_grammar.getLastError();
    log::crit("core.config", "parse error at (line %d, column %d). expecting token %s near '%s'",
              l_error.line, l_error.col, l_error.token, l_error.preview, HERE);
    return status::error;
  }

  return translate(l_sections, m_data);
}


}}

#endif // !XTD_CORE_CONFIG_PARSER_HXX_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
