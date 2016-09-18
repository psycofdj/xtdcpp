#ifndef XTD_CORE_LOG_FORMATTER_HH_
# define XTD_CORE_LOG_FORMATTER_HH_
# include "log/fwd.hh"
# include "log/logtypes.hh"
# include "log/Fields.hh"

namespace xtd {
namespace log {

class Formatter
{
  friend class ConfLoader;

public:
  typedef Fields<string> t_formats;

protected:
  typedef fn<void(const FormattedRecord&, t_formats&)> t_format_modifier;

public:
  Formatter(void);
  virtual ~Formatter(void);

protected:
  static sptr<Formatter> create(const string& p_name, const map<string,string>& p_properties);

public:
  Formatter& setLayout(const string& p_name, const string& p_layout);
  Formatter& setLayouts(const map<string, string>& p_layouts);
  Formatter& setFieldFormat(const string& p_fieldName, const string& p_format);
  Formatter& setFieldFormats(const map<string, string>& p_formats);

  template<typename Handler>
  Formatter& addFormatModifier(Handler p_functor);

  template<typename... Arguments>
  FormattedRecord format(const Record& p_rec, Arguments&&... p_args) const;

protected:
  virtual void   createFields(const FormattedRecord& p_rec, Fields<string>& p_args) const;
  virtual string getFulllog(const FormattedRecord& p_rec)                           const;

  string resolveLayout(const string& p_layout, const Fields<string>& p_args)           const;
  string resolveTime(const string& p_layout,   const boost::posix_time::ptime& p_time) const;

protected:
  string                    m_fmt;
  string                    m_locFmt;
  string                    m_timeFmt;
  Fields<string>            m_fieldFormats;
  vector<t_format_modifier> m_formatModifiers;
};

}}

# include "log/Formatter.hxx"

#endif // !XTD_CORE_LOG_FORMATTER_HH_
