// IWYU pragma: private, include "log/Formatter.hh"
#ifndef CORE_LOG_FORMATTER_HXX_
# define CORE_LOG_FORMATTER_HXX_
# include <boost/format.hpp>
# include <boost/lexical_cast.hpp>
# include <regex>
# include <exception>
# include "types.hh"        // libcore
# include "text.hh"         // libcore
# include "format.hh"       // libcore
# include "log/logtypes.hh" // libcore

namespace xtd { namespace log { class Formatter; } }
namespace xtd {
namespace log {

template<typename... Arguments>
FormattedRecord
Formatter::format(const Record& p_rec, Arguments&&... p_args) const
{
  FormattedRecord l_rec(p_rec);

  try
  {
    boost::format   l_format(l_rec.m_format);
    size_t          l_fmtArgsCount = l_format.expected_args();
    size_t          l_argsCount    = sizeof...(Arguments);

    if (l_fmtArgsCount == l_argsCount) {
      l_rec.m_message = format::vargs(l_format, p_args...);
    }
    else if (l_fmtArgsCount + 3 == l_argsCount)
    {
      vector<string> l_parts;

      l_rec.m_message  = format::vargs(p_rec.m_format + "//sep//%s//sep//%s//sep//%s", p_args...);
      text::algorithm::regex::split(l_parts, l_rec.m_message, std::regex("//sep//"));
      l_rec.m_message  = l_parts[0];
      l_rec.m_function = l_parts[1];
      l_rec.m_filename = l_parts[2];
      l_rec.m_line     = boost::lexical_cast<size_t>(l_parts[3]);
      l_rec.m_hasLoc   = true;
    } else {
      l_rec.m_message = "unable to render format : " + p_rec.m_format + " : wrong number of arguments";
    }
  } catch (std::exception& l_error) {
    l_rec.m_message = "unable to render format : " + p_rec.m_format + " : " + l_error.what();
  }

  l_rec.m_fulllog = getFulllog(l_rec);
  return l_rec;
}

template<typename Handler>
Formatter&
Formatter::addFormatModifier(Handler p_functor)
{
  t_format_modifier l_modifier = [=](const FormattedRecord& p_rec, t_formats& p_format) mutable {
    p_functor(p_rec, p_format);
  };
  m_formatModifiers.push_back(l_modifier);
  return *this;
}

}}

#endif // !CORE_LOG_FORMATTER_HXX_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
