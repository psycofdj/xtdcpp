#ifndef XTD_CORE_LOG_FORMATMODIFIERS_HH_
# define XTD_CORE_LOG_FORMATMODIFIERS_HH_
# include "log/Formatter.hh"
# include "log/ColoredFormatter.hh"

namespace xtd {
namespace log {


struct AutoWidth
{
  AutoWidth(const string& p_fields) :
    AutoWidth(vector<string>({ p_fields }))
  {
  }

  AutoWidth(const vector<string>& p_fields)
  {
    for (auto& c_field : p_fields)
      m_widths[c_field] = 0;
  }

  void operator()(const FormattedRecord& p_rec,
                  Formatter::t_formats&  p_formats)
  {
    static const map<string, std::function<size_t(const FormattedRecord&)>> l_func = {
      { "name",     [](const FormattedRecord& p_rec) { return p_rec.m_name.size();     } },
      { "message",  [](const FormattedRecord& p_rec) { return p_rec.m_message.size();  } },
      { "function", [](const FormattedRecord& p_rec) { return p_rec.m_function.size(); } },
      { "fulllog",  [](const FormattedRecord& p_rec) { return p_rec.m_fulllog.size();  } },
      { "module",   [](const FormattedRecord& p_rec) { return p_rec.m_module.size();   } },
      { "slevel",   [](const FormattedRecord& p_rec) { return to_string(p_rec.m_level).size();   } }
    };

    for (auto& c_width : m_widths)
    {
      auto c_functor = l_func.find(c_width.first);
      if (l_func.end() != c_functor) {
        c_width.second = std::max(c_width.second, c_functor->second(p_rec));
        string l_format = boost::str(boost::format("%%-%ds") % c_width.second);
        p_formats.set(c_width.first, l_format);
      }
    }
  }

private:
  map<string, size_t> m_widths;
};

struct StyleByLevel
{
  StyleByLevel(const map<level, tty::style>& p_conds) :
    m_conds(p_conds)
  { }

  void operator()(const FormattedRecord& p_rec, ColoredFormatter::t_styles& p_style)
  {
    auto c_item = m_conds.find(p_rec.m_level);
    if (c_item != m_conds.end())
      p_style.m_slevel = c_item->second;
  }

private:
  map<level, tty::style> m_conds;
};

struct MatchMessage
{
  MatchMessage(const std::regex& p_match, const tty::style& p_style) :
    m_match(p_match),
    m_style(p_style)
  { }

  void operator() (const FormattedRecord& p_rec, ColoredFormatter::t_styles& p_style)
  {
    if (true == std::regex_match(p_rec.m_message, m_match))
      p_style.m_message = m_style;
  };

private:
  std::regex m_match;
  tty::style m_style;
};



}}

#endif // !XTD_CORE_LOG_FORMATMODIFIERS_HH_
