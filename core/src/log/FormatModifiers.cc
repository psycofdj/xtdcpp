#include "log/FormatModifiers.hh"
#include "log/helpers.hh"

namespace xtd {
namespace log {


AutoWidth::AutoWidth(const vector<string>& p_fields) :
  m_widths(),
  m_fields(p_fields)
{
  for (auto& c_field : p_fields)
    m_widths.set(c_field, 0u);
}

AutoWidth::AutoWidth(const string& p_field) :
  AutoWidth(vector<string>({ p_field }))
{ }

void
AutoWidth::operator()(const FormattedRecord& p_rec, Fields<string>& p_formats)
{
  // LCOV_EXCL_START
  const static auto l_intSize = [](size_t p_value) {
    return std::to_string(p_value).size();
  };

  static const map<string, fn<size_t(const FormattedRecord&)>> lcs_func = {
    { "name",     [](const FormattedRecord& p_rec) { return p_rec.m_name.size();               } },
    { "module",   [](const FormattedRecord& p_rec) { return p_rec.m_module.size();             } },
    { "slevel",   [](const FormattedRecord& p_rec) { return to_string(p_rec.m_level).size();   } },
    { "ilevel",   [](const FormattedRecord& p_rec) { return l_intSize(valueof(p_rec.m_level)); } },
    { "threadid", [](const FormattedRecord& p_rec) { return l_intSize(p_rec.m_threadID);       } },
    { "filename", [](const FormattedRecord& p_rec) { return p_rec.m_filename.size();           } },
    { "line",     [](const FormattedRecord& p_rec) { return l_intSize(p_rec.m_line);           } },
    { "pid",      [](const FormattedRecord& p_rec) { return l_intSize(p_rec.m_pid);            } },
    { "ppid",     [](const FormattedRecord& p_rec) { return l_intSize(p_rec.m_ppid);           } },
    { "message",  [](const FormattedRecord& p_rec) { return p_rec.m_message.size();            } },
    { "function", [](const FormattedRecord& p_rec) { return p_rec.m_function.size();           } },
    { "fulllog",  [](const FormattedRecord& p_rec) { return p_rec.m_fulllog.size();            } }
  };
  // LCOV_EXCL_STOP

  for (auto& c_field : m_fields)
  {
    auto c_functor = lcs_func.find(c_field);
    if (lcs_func.end() != c_functor)
    {
      size_t l_size = 0;
      m_widths.get(c_field, l_size);
      l_size = std::max(l_size, c_functor->second(p_rec));
      m_widths.set(c_field, l_size);
      string l_format = boost::str(boost::format("%%-%ds") % l_size);
      p_formats.set(c_field, l_format);
    }
  }
}

StyleByLevel::StyleByLevel(const map<level, tty::style>& p_conds) :
  m_conds(p_conds)
{ }

void
StyleByLevel::operator()(const FormattedRecord& p_rec,
                         Fields<tty::style>&    p_styles) const
{
  auto c_item = m_conds.find(p_rec.m_level);
  if (c_item != m_conds.end())
    p_styles.m_slevel = c_item->second;
}

StyleMatch::StyleMatch(const std::regex& p_match, const tty::style& p_style) :
  m_match(p_match),
  m_style(p_style)
{ }

void
StyleMatch::operator()(const FormattedRecord& p_rec,
                       Fields<tty::style>&    p_styles) const
{
  if (true == std::regex_match(p_rec.m_message, m_match))
    p_styles.m_message = m_style;
};

}}
