#include "log/SyslogAppender.hh"
#include "log/helpers.hh"
#include "log/ConfLoader.hh"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace xtd {
namespace log {

std::mutex SyslogAppender::ms_mutex;
uint32_t   SyslogAppender::ms_instances = 0;

SyslogAppender::SyslogAppender(const string&          p_identity,
                               int32_t                p_opts,
                               int32_t                p_facility,
                               const sptr<Formatter>& p_formatter) :
  Appender(p_formatter),
  m_opts(p_opts),
  m_facility(p_facility)
{
  std::lock_guard<std::mutex> l_lock(ms_mutex);

  if (0 == ms_instances)
  {
    if (p_identity == "%(binname)")
      openlog(0, p_opts, p_facility);
    else
      openlog(p_identity.c_str(), p_opts, p_facility);
  }
  ms_instances++;
}

SyslogAppender::~SyslogAppender(void)
{
  std::lock_guard<std::mutex> l_lock(ms_mutex);

  ms_instances--;
  if (0 == ms_instances)
    closelog();
}

void
SyslogAppender::print(const FormattedRecord& p_rec) const
{
  syslog(to_value(p_rec.m_level), "%s", p_rec.m_fulllog.c_str());
}


bool
SyslogAppender::to_option(const string& p_value, uint32_t& p_result)
{
  static const map<string, uint32_t> l_values = {
    { "LOG_CONS",   LOG_CONS   },
    { "LOG_NDELAY", LOG_NDELAY },
    { "LOG_NOWAIT", LOG_NOWAIT },
    { "LOG_ODELAY", LOG_ODELAY },
    { "LOG_PERROR", LOG_PERROR },
    { "LOG_PID",    LOG_PID    }
  };

  auto c_item = l_values.find(boost::to_upper_copy(p_value));
  if (c_item != l_values.end())
  {
    p_result = c_item->second;
    return true;
  }
  return false;
}


bool
SyslogAppender::to_facility(const string& p_value, uint32_t& p_result)
{
  static const map<string, uint32_t> l_values = {
    { "LOG_AUTH",     LOG_AUTH },
    { "LOG_AUTHPRIV", LOG_AUTHPRIV },
    { "LOG_CRON",     LOG_CRON },
    { "LOG_DAEMON",   LOG_DAEMON },
    { "LOG_FTP",      LOG_FTP },
    { "LOG_KERN",     LOG_KERN },
    { "LOG_LOCAL0",   LOG_LOCAL0 },
    { "LOG_LOCAL1",   LOG_LOCAL1 },
    { "LOG_LOCAL2",   LOG_LOCAL2 },
    { "LOG_LOCAL3",   LOG_LOCAL3 },
    { "LOG_LOCAL4",   LOG_LOCAL4 },
    { "LOG_LOCAL5",   LOG_LOCAL5 },
    { "LOG_LOCAL6",   LOG_LOCAL6 },
    { "LOG_LOCAL7",   LOG_LOCAL7 },
    { "LOG_LPR",      LOG_LPR },
    { "LOG_MAIL",     LOG_MAIL },
    { "LOG_NEWS",     LOG_NEWS },
    { "LOG_SYSLOG",   LOG_SYSLOG },
    { "LOG_USER",     LOG_USER },
    { "LOG_UUCP",     LOG_UUCP }
  };

  auto c_item = l_values.find(boost::to_upper_copy(p_value));
  if (c_item != l_values.end())
  {
    p_result = c_item->second;
    return true;
  }
  return false;
}

std::shared_ptr<Appender>
SyslogAppender::create(const string& p_name, const map<string,string>& p_properties)
{
  string                          l_identity = "%(binname)";
  uint32_t                        l_facility = 0;
  uint32_t                        l_options  = 0;
  vector<string>                  l_parts;

  auto c_identity = p_properties.find("log.appender." + p_name + ".identity");
  auto c_options  = p_properties.find("log.appender." + p_name + ".options");
  auto c_facility = p_properties.find("log.appender." + p_name + ".facility");

  if (p_properties.end() != c_identity)
    l_identity = c_identity->second;

  if (p_properties.end() == c_options)
    log::raise<log_error>("unable to find key 'log.appender.%s.options'", p_name, HERE);

  if (false == to_facility(c_facility->second, l_facility))
    log::raise<log_error>("unable to convert '%s' as syslog facility", c_facility->second, HERE);

  if (p_properties.end() != c_options)
  {
    l_parts.clear();
    boost::split(l_parts, c_options->second, boost::is_any_of(", "), boost::token_compress_on);
    for (auto c_opt : l_parts)
    {
      uint32_t l_value;
      if (false == to_option(c_opt, l_value))
        log::raise<log_error>("unable to convert '%s' as syslog options", c_opt, HERE);
      l_options |= l_value;
    }
  }

  return std::make_shared<SyslogAppender>(l_identity, l_options, l_facility);
}

}}
