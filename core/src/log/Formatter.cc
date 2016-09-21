#include "log/Formatter.hh"

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "log/helpers.hh"
#include "log/ConfLoader.hh"
#include "log/Fields.hh"

namespace xtd {
namespace log {



Formatter::Formatter(void)
{
  setLayouts({
      { "fulllog",  "%(name) [%(pid)] [%(ppid)] %(time) [%(threadid)] [%(module)] (%(slevel)) %(message) %(location)" },
      { "location", "at %(filename):%(line) -> %(function)"                                                           },
      { "time",     "%d-%m-%Y %H:%M:%s"                                                                               }
    });

  setFieldFormats({
      { "name",     "%s"   },
      { "pid",      "%d"   },
      { "ppid",     "%d"   },
      { "module",   "%s"   },
      { "threadid", "%02d" },
      { "slevel",   "%7s"  },
      { "ilevel",   "%d"   },
      { "message",  "%s"   },
      { "filename", "%s"   },
      { "line",     "%d"   },
      { "function", "%s"   },
      { "time",     "%s"   },
      { "location", "%s"   },
      { "fulllog",  "%s"   }
    });
}

Formatter::~Formatter(void)
{
}

Formatter&
Formatter::setLayout(const string& p_name, const string& p_layout)
{
  if ("fulllog" == p_name)
    m_fmt = p_layout;
  else if ("location" == p_name)
    m_locFmt = p_layout;
  else if ("time" == p_name)
    m_timeFmt = p_layout;
  return *this;
}

Formatter&
Formatter::setLayouts(const map<string, string>& p_layouts)
{
  for (auto& c_layout : p_layouts)
    setLayout(c_layout.first, c_layout.second);
  return *this;
}

Formatter&
Formatter::setFieldFormat(const string& p_fieldName, const string& p_format)
{
  m_fieldFormats.set(p_fieldName, p_format);
  return *this;
}

Formatter&
Formatter::setFieldFormats(const map<string, string>& p_formats)
{
  for (auto c_format : p_formats)
    setFieldFormat(c_format.first, c_format.second);
  return *this;
}

string
Formatter::resolveTime(const string& p_format, const boost::posix_time::ptime& p_time) const
{
  static std::locale l_locale(std::cout.getloc(), new boost::posix_time::time_facet(p_format.c_str()));
  std::stringstream  l_buffer;

  l_buffer.imbue(l_locale);
  l_buffer << p_time;
  return l_buffer.str();
}

string
Formatter::resolveLayout(const string& p_layout, const Fields<string>& p_args) const
{
  string l_result = p_layout;

  boost::replace_all(l_result, "%(name)",     p_args.m_name);
  boost::replace_all(l_result, "%(module)",   p_args.m_module);
  boost::replace_all(l_result, "%(threadid)", p_args.m_threadid);
  boost::replace_all(l_result, "%(slevel)",   p_args.m_slevel);
  boost::replace_all(l_result, "%(ilevel)",   p_args.m_ilevel);
  boost::replace_all(l_result, "%(message)",  p_args.m_message);
  boost::replace_all(l_result, "%(filename)", p_args.m_filename);
  boost::replace_all(l_result, "%(line)",     p_args.m_line);
  boost::replace_all(l_result, "%(function)", p_args.m_function);
  boost::replace_all(l_result, "%(location)", p_args.m_location);
  boost::replace_all(l_result, "%(time)",     p_args.m_time);
  boost::replace_all(l_result, "%(pid)",      p_args.m_pid);
  boost::replace_all(l_result, "%(ppid)",     p_args.m_ppid);
  return l_result;
}

void
Formatter::createFields(const FormattedRecord& p_rec, Fields<string>& p_args) const
{
  using boost::str;
  using boost::format;

  t_formats l_formats(m_fieldFormats);
  for (auto& c_fn : m_formatModifiers)
    c_fn(p_rec, l_formats);

  p_args.m_name     = str(format(l_formats.m_name)     % p_rec.m_name);
  p_args.m_threadid = str(format(l_formats.m_threadid) % p_rec.m_threadID);
  p_args.m_module   = str(format(l_formats.m_module)   % p_rec.m_module);
  p_args.m_slevel   = str(format(l_formats.m_slevel)   % to_string(p_rec.m_level));
  p_args.m_ilevel   = str(format(l_formats.m_ilevel)   % to_value(p_rec.m_level));
  p_args.m_message  = str(format(l_formats.m_message)  % p_rec.m_message);
  p_args.m_time     = str(format(l_formats.m_time)     % resolveTime(m_timeFmt, p_rec.m_time));
  p_args.m_pid      = str(format(l_formats.m_pid)      % p_rec.m_pid);
  p_args.m_ppid     = str(format(l_formats.m_ppid)     % p_rec.m_ppid);

  if (true == p_rec.m_hasLoc)
  {
    p_args.m_filename = str(format(l_formats.m_filename) % p_rec.m_filename);
    p_args.m_line     = str(format(l_formats.m_line)     % p_rec.m_line);
    p_args.m_function = str(format(l_formats.m_function) % p_rec.m_function);
    p_args.m_location = str(format(l_formats.m_location) % resolveLayout(m_locFmt, p_args));
  }
}

string
Formatter::getFulllog(const FormattedRecord& p_rec) const
{
  Fields<string> l_args;

  createFields(p_rec, l_args);
  return boost::str(boost::format(m_fieldFormats.m_fulllog) % resolveLayout(m_fmt, l_args));
}

std::shared_ptr<Formatter>
Formatter::create(const string& p_name, const map<string,string>& p_properties)
{
  std::shared_ptr<Formatter> l_result(new Formatter());

  static const vector<string> ls_formats = { "fulllog", "location", "time" };
  static const vector<string> ls_fields  = {
    "name",     "pid",    "ppid",     "module",   "threadid",
    "slevel",   "ilevel", "message",  "filename", "line",
    "function", "time",   "location", "fulllog"
  };

  for (const auto& c_format : ls_formats)
  {
    auto c_item = p_properties.find("log.formatter." + p_name + ".layout." + c_format);
    if (c_item != p_properties.end())
      l_result->setLayout(c_format, c_item->second);
  }

  for (const auto& c_field : ls_fields)
  {
    auto c_item = p_properties.find("log.formatter." + p_name + ".format." + c_field);
    if (c_item != p_properties.end())
      l_result->setFieldFormat(c_field, c_item->second);
  }

  return l_result;
}

}}
