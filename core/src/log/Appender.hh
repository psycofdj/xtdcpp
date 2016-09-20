#ifndef XTD_CORE_LOG_APPENDER_HH_
# define XTD_CORE_LOG_APPENDER_HH_
# include "log/logtypes.hh"
# include "log/fwd.hh"
# include "log/Formatter.hh"

namespace xtd {
namespace log {

class Appender
{
private:
  typedef std::function<bool(const FormattedRecord&)> t_filter;
  typedef vector<t_filter>                            t_filters;

public:
  Appender(void);
  Appender(const sptr<Formatter>& p_formatter);
  virtual ~Appender(void);

public:
  static sptr<Appender> create(const string& p_name, const map<string,string>& p_properties);

public:
  void setFormatter(const sptr<Formatter>& p_formatter);
  void addFilter(t_filter p_filter);
  void clearFilters(void);
  void clearFormatter(void);

private:
  virtual void print(const FormattedRecord& /* p_rec */ ) const = 0;

public:
  template<typename... Arguments>
  void log(Record& p_record, Arguments&&... p_args) const;

private:
  sptr<Formatter> m_formatter;
  t_filters       m_filters;
};


template<typename... Arguments>
void
Appender::log(Record& p_record, Arguments&&... p_args) const
{
  FormattedRecord l_rec = m_formatter->format(p_record, p_args...);

  bool l_accepted = true;
  for (auto c_filter : m_filters)
    l_accepted = l_accepted && c_filter(l_rec);

  if (true == l_accepted)
    print(l_rec);
}

}}

#endif // !XTD_CORE_LOG_APPENDER_HH_
