#ifndef CORE_LOG_SYSLOGAPPENDER_HH_
# define CORE_LOG_SYSLOGAPPENDER_HH_
# include "log/Appender.hh"
# include "mixins/cloneable.hh"

namespace xtd {
namespace log {

class SyslogAppender : public Appender
{
  friend class ConfLoader;

public:
  SyslogAppender(const string& p_identity, int32_t p_opts, int32_t p_facility);
  virtual ~SyslogAppender(void);

private:
  static sptr<Appender> create(const string& p_name, const map<string,string>& p_properties);
  static bool           to_facility(const string& p_value, uint32_t& p_result);
  static bool           to_option(const string& p_value, uint32_t& p_result);

private:
  void print(const FormattedRecord& p_rec) const override;

private:
  int32_t         m_opts;
  int32_t         m_facility;
  static uint32_t ms_instances;
};

}}

#endif // !CORE_LOG_SYSLOGAPPENDER_HH_
