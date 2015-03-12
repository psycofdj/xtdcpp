#include "logger.hh"
#include <boost/algorithm/string.hpp>
#include <regex>


namespace xtd {

logger* logger::ms_pInstance = 0;


logger::logger(void) :
  m_name("unknown")
{
}

logger&
logger::get(void)
{
  if (0 == ms_pInstance)
    ms_pInstance = new logger();
  return *ms_pInstance;
}

void
logger::wrap_syslog(int __pri, const char *__fmt, ...)
{
  va_list l_ap;
  va_start(l_ap,__fmt);
  vsyslog(__pri, __fmt, l_ap);
  va_end(l_ap);
}

void
logger::initialize(void)
{
  initialize(m_name, level::crit);
}

void
logger::initialize(const string& p_name, level p_level)
{
  setName(p_name);
  setLevel("", p_level);
  openlog(m_name.c_str(), LOG_PID | LOG_PERROR, LOG_LOCAL0);
  m_initialized = true;
}

void
logger::setLevel(const string& p_module, level p_level)
{
  m_levels[p_module] = p_level;
}

void
logger::clearAll(level p_level)
{
  m_levels.clear();
  setLevel("", p_level);
}

void
logger::updateLevels(const string& p_module, level p_level)
{
  std::regex l_matcher(p_module + ".*");

  for (auto& c_item : m_levels)
  {
    if (true == std::regex_match(c_item.first, l_matcher))
      c_item.second = p_level;
  }
  setLevel(p_module, p_level);
}

logger::t_levels
logger::getLevels(void)
{
  return m_levels;
}

void
logger::setAllLevels(level p_level)
{
  for (auto& c_item : m_levels)
    c_item.second = p_level;
}

void
logger::setAllValueLevels(uint32_t p_level)
{
  setAllLevels(levelOf(p_level));
}



void
logger::setName(const string& p_name)
{
  m_name = p_name;
}


logger::level
logger::getLevel(const string& p_module)
{
  auto                     c_res = m_levels.find(p_module);
  level                    l_result = level::crit;
  vector<string>           l_parts;
  vector<string>::iterator c_part;

  if (c_res != m_levels.end())
    return c_res->second;

  boost::split(l_parts, p_module, boost::is_any_of("."), boost::token_compress_on);
  c_part = l_parts.end();

  while (c_res == m_levels.end())
  {
    c_part--;
    vector<string> l_tmp(l_parts.begin(), c_part);
    c_res = m_levels.find(boost::join(l_tmp, "."));
  }

  if (c_res != m_levels.end())
    l_result = c_res->second;

  setLevel(p_module, l_result);
  return level::crit;
}



logger::level
logger::levelOf(const uint32_t& p_level)
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

uint32_t
logger::valueOf(level p_level)
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


bool
logger::isValidStringLevel(string p_level)
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


string
logger::stringOf(level p_level)
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


logger::level
logger::fromString(string p_level)
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

}
