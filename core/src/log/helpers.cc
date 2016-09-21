#include "log/helpers.hh"

namespace xtd {
namespace log {

namespace {
sptr<RootLogger> m_instance;
}

level from(const uint32_t& p_level)
{
  if (p_level == LOG_EMERG)
    return level::emerg;
  else if (p_level == LOG_ALERT)
    return level::alert;
  else if (p_level == LOG_CRIT)
    return level::crit;
  else if (p_level == LOG_ERR)
    return level::err;
  else if (p_level == LOG_WARNING)
    return level::warning;
  else if (p_level == LOG_NOTICE)
    return level::notice;
  else if (p_level == LOG_INFO)
    return level::info;
  else if (p_level == LOG_DEBUG)
    return level::debug;
  return level::crit;
}

level from(string p_level)
{
  if (p_level == "emerg")
    return level::emerg;
  if (p_level == "alert")
    return level::alert;
  if (p_level == "crit")
    return level::crit;
  if (p_level == "err")
    return level::err;
  if (p_level == "warning")
    return level::warning;
  if (p_level == "notice")
    return level::notice;
  if (p_level == "info")
    return level::info;
  if (p_level == "debug")
    return level::debug;
  return level::crit;
}

uint32_t to_value(level p_level)
{
  switch (p_level)
  {
  case level::emerg:
    return LOG_EMERG;
  case level::alert:
    return LOG_ALERT;
  case level::crit:
    return LOG_CRIT;
  case level::err:
    return LOG_ERR;
  case level::warning:
    return LOG_WARNING;
  case level::notice:
    return LOG_NOTICE;
  case level::info:
    return LOG_INFO;
  case level::debug:
    return LOG_DEBUG;
  }
  return LOG_CRIT;
}

string to_string(level p_level)
{
  switch (p_level)
  {
  case level::emerg:
    return "emerg";
  case level::alert:
    return "alert";
  case level::crit:
    return "crit";
  case level::err:
    return "err";
  case level::warning:
    return "warning";
  case level::notice:
    return "notice";
  case level::info:
    return "info";
  case level::debug:
    return "debug";
  }
  return "crit";
}

bool is_valid(string p_level)
{
  return
    ((p_level == "emerg")   ||
     (p_level == "alert")   ||
     (p_level == "crit")    ||
     (p_level == "err")     ||
     (p_level == "warning") ||
     (p_level == "notice")  ||
     (p_level == "info")    ||
     (p_level == "debug"));
}


RootLogger& getRoot(void)
{
  if (0 == m_instance.get())
    m_instance.reset(new RootLogger());
  return *m_instance;
}

}}
