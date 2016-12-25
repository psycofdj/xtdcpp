#include "log/ColoredFormatter.hh"

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "log/helpers.hh"
#include "ConfLoader.hh"


namespace xtd {
namespace log {

ColoredFormatter::ColoredFormatter(void) :
  Formatter()
{
  using namespace tty;

  setStyles({
      { "name",     style(color::green)               },
      { "threadid", style(color::yellow)              },
      { "message",  style(color::white)               },
      { "module",   style(color::lyellow)             },
      { "time",     style(color::cyan)                },
      { "slevel",   style(color::lred, attrs::bold)   },
      { "location", style(color::lblack)              },
      { "pid",      style(color::lblue)               },
      { "ppid",     style(color::lblue, attrs::bold)  }
    });
}

ColoredFormatter::ColoredFormatter(const Formatter& p_base) :
  Formatter(p_base)
{
  using namespace tty;

  setStyles({
      { "name",     style(color::green)               },
      { "threadid", style(color::yellow)              },
      { "message",  style(color::white)               },
      { "module",   style(color::lyellow)             },
      { "time",     style(color::cyan)                },
      { "slevel",   style(color::lred, attrs::bold)   },
      { "location", style(color::lblack)              },
      { "pid",      style(color::lblue)               },
      { "ppid",     style(color::lblue, attrs::bold)  }
    });
}

ColoredFormatter::~ColoredFormatter(void)
{
}

ColoredFormatter&
ColoredFormatter::setAttrs(const string&     p_field,
                           const tty::attrs& p_attrs)
{
  if (true == m_fieldStyles.exists(p_field))
    m_fieldStyles.get(p_field).setAttrs(p_attrs);
  return *this;
}

ColoredFormatter&
ColoredFormatter::setFgColor(const string&     p_field,
                             const tty::color& p_fg)
{
  if (true == m_fieldStyles.exists(p_field))
    m_fieldStyles.get(p_field).setFgColor(p_fg);
  return *this;
}

ColoredFormatter&
ColoredFormatter::setBgColor(const string&     p_field,
                             const tty::color& p_bg)
{
  if (true == m_fieldStyles.exists(p_field))
    m_fieldStyles.get(p_field).setBgColor(p_bg);
  return *this;
}

ColoredFormatter&
ColoredFormatter::setStyle(const string&     p_field,
                           const tty::style& p_style)
{
  m_fieldStyles.set(p_field, p_style);
  return *this;
}

ColoredFormatter&
ColoredFormatter::setStyle(const string&     p_field,
                           const tty::color& p_fg,
                           const tty::color& p_bg,
                           const tty::attrs& p_attrs)
{
  return setStyle(p_field, tty::style(p_fg, p_bg, p_attrs));
}

ColoredFormatter&
ColoredFormatter::setStyles(const map<string, tty::style>& p_styles)
{
  for (auto c_style : p_styles)
    m_fieldStyles.set(c_style.first, c_style.second);
  return *this;
}

void
ColoredFormatter::createFields(const FormattedRecord& p_rec, Fields<string>& p_args) const
{
  t_styles l_style(m_fieldStyles);

  for (auto& c_fn : m_styleModifiers)
    c_fn(p_rec, l_style);

  Formatter::createFields(p_rec, p_args);
  p_args.m_name     = tty::Text(p_args.m_name,     l_style.m_name);
  p_args.m_threadid = tty::Text(p_args.m_threadid, l_style.m_threadid);
  p_args.m_module   = tty::Text(p_args.m_module,   l_style.m_module);
  p_args.m_slevel   = tty::Text(p_args.m_slevel,   l_style.m_slevel);
  p_args.m_ilevel   = tty::Text(p_args.m_ilevel,   l_style.m_ilevel);
  p_args.m_message  = tty::Text(p_args.m_message,  l_style.m_message);
  p_args.m_time     = tty::Text(p_args.m_time,     l_style.m_time);
  p_args.m_pid      = tty::Text(p_args.m_pid,      l_style.m_pid);
  p_args.m_ppid     = tty::Text(p_args.m_ppid,     l_style.m_ppid);
  p_args.m_filename = tty::Text(p_args.m_filename, l_style.m_filename);
  p_args.m_line     = tty::Text(p_args.m_line,     l_style.m_line);
  p_args.m_function = tty::Text(p_args.m_function, l_style.m_function);
  p_args.m_location = tty::Text(p_args.m_location, l_style.m_location);
}

string
ColoredFormatter::getFulllog(const FormattedRecord& p_rec)  const
{
  return tty::Text(Formatter::getFulllog(p_rec), m_fieldStyles.m_fulllog);
}

sptr<Formatter>
ColoredFormatter::create(const string& p_name, const map<string,string>& p_properties)
{
  sptr<Formatter>        l_base   = Formatter::create(p_name, p_properties);
  sptr<ColoredFormatter> l_result(new ColoredFormatter(*l_base));
  vector<string>         l_fields = {
    "name",     "pid",    "ppid",     "module",   "threadid",
    "slevel",   "ilevel", "message",  "filename", "line",
    "function", "time",   "location", "fulllog"
  };

  for (auto& c_field : l_fields)
  {
    tty::color l_fg    = tty::color::unset;
    tty::color l_bg    = tty::color::unset;
    tty::attrs l_attrs = tty::attrs::unset;

    auto c_fgColor = p_properties.find("log.formatter." + p_name + ".style." + c_field + ".fgColor");
    auto c_bgColor = p_properties.find("log.formatter." + p_name + ".style." + c_field + ".bgColor");
    auto c_attrs   = p_properties.find("log.formatter." + p_name + ".style." + c_field + ".attrs");

    if ((p_properties.end() == c_fgColor) &&
        (p_properties.end() == c_bgColor) &&
        (p_properties.end() == c_attrs))
      continue;

    if (p_properties.end() != c_fgColor)
      if (false == tty::color::from_string(c_fgColor->second, l_fg))
        log::raise<log_error>("core.log", "invalid color '%s' in key '%s'",
                              c_fgColor->second, c_fgColor->first, HERE);

    if (p_properties.end() != c_bgColor)
    {
      if (false == tty::color::from_string(c_bgColor->second, l_bg))
        log::raise<log_error>("core.log", "invalid color '%s' in key '%s'",
                              c_bgColor->second, c_bgColor->first, HERE);
    }

    if (p_properties.end() != c_attrs)
    {
      vector<string> l_parts;
      boost::split(l_parts, c_attrs->second, boost::is_any_of(" ,|"), boost::token_compress_on);
      std::cout << "args size : " << l_parts.size() << std::endl;
      for (auto& c_val : l_parts)
      {
        tty::attrs l_tmp(tty::attrs::unset);
        if (false == tty::attrs::from_string(c_val, l_tmp))
          log::raise<log_error>("core.log", "invalid attribute '%s' in key '%s'",
                                c_val, c_attrs->first, HERE);
        l_attrs |= l_tmp;
      }
    }
    l_result->setStyles({{ c_field, tty::style(l_fg, l_bg, l_attrs) }});
  }

  return l_result;
}

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
