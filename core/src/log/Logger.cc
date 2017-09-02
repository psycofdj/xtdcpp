#include "log/Logger.hh"

#include <regex>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include "log/helpers.hh"

namespace xtd {
namespace log {

/* ----------------------------------------------------------------------- */

map<size_t, uint32_t> Logger::thread_counter::ms_ids;
std::mutex            Logger::thread_counter::ms_mutex;
size_t                Logger::thread_counter::ms_mainID = thread_counter::get_id();

int32_t
Logger::thread_counter::get_id(void)
{
  std::hash<std::thread::id>  l_hasher;
  size_t                      l_hash = l_hasher(std::this_thread::get_id());
  std::lock_guard<std::mutex> l_lock(ms_mutex);
  auto                        c_item = ms_ids.find(l_hash);

  if (c_item != ms_ids.end())
    return c_item->second;
  auto l_result = ms_ids.insert(std::make_pair(l_hash, ms_ids.size()));
  return l_result.first->second;
}

/* ----------------------------------------------------------------------- */

Logger::Logger(void) :
  m_module(),
  m_level(level::crit),
  m_appenders(),
  m_mutex()
{
}

Logger::Logger(const Logger& p_parent, const string& p_module) :
  m_module(p_module),
  m_name(p_parent.getName()),
  m_level(p_parent.getLevel()),
  m_appenders(p_parent.m_appenders)
{
}

Logger::~Logger(void)
{
}

Logger&
Logger::addAppender(const sptr<Appender>& p_appender)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_appenders.push_back(p_appender);
  return *this;
}

Logger&
Logger::clearAppenders(void)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_appenders.clear();
  return *this;
}

const Logger::t_appenders
Logger::getAppenders(void) const
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  return t_appenders(m_appenders.begin(), m_appenders.end());
}

/* ----------------------------------------------------------------------- */

RootLogger::RootLogger(void) :
  Logger(),
  m_loggers()
{
}

RootLogger&
RootLogger::initialize(const string& p_name, level p_level)
{
  Logger::setName(p_name);
  Logger::setLevel(p_level);
  return *this;
}

Logger&
RootLogger::get(const string& p_module)
{
  if (p_module.empty())
    return *this;

  std::lock_guard<std::mutex> l_lock(m_mutex);
  Logger*                     l_model = this;
  auto                        c_log   = m_loggers.find(p_module);

  if (c_log != m_loggers.end())
    return *(c_log->second);

  auto c_target = getClosestParent(p_module);
  if (m_loggers.end() != c_target)
    l_model = c_target->second.get();

  m_loggers[p_module] = std::make_shared<Logger>(*l_model, p_module);
  return *(m_loggers[p_module]);
}

level
RootLogger::getLevelTo(const string& p_module) const
{
  if (p_module.empty())
    return getLevel();

  std::lock_guard<std::mutex> l_lock(m_mutex);
  auto                        c_log   = m_loggers.find(p_module);

  if (c_log != m_loggers.end())
    return c_log->second->getLevel();

  if (m_loggers.end() != (c_log = getClosestParent(p_module)))
    return c_log->second->getLevel();

  return getLevel();
}

const RootLogger::t_levels
RootLogger::getLevels(void) const
{
  t_levels l_result;
  std::lock_guard<std::mutex> l_lock(m_mutex);

  for (auto c_log : m_loggers)
    l_result.insert(std::make_pair(c_log.first, c_log.second->getLevel()));
  l_result.insert(std::make_pair("", getLevel()));
  return l_result;
}

RootLogger&
RootLogger::setLevelTo(const string& p_module, level p_level)
{
  get(p_module).setLevel(p_level);
  return *this;
}


RootLogger&
RootLogger::setAllLevels(level p_level)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);

  for (auto c_log : m_loggers)
    c_log.second->setLevel(p_level);
  Logger::setLevel(p_level);
  return *this;
}

RootLogger&
RootLogger::setAllValueLevels(uint32_t p_level)
{
  setAllLevels(from(p_level));
  return *this;
}

RootLogger&
RootLogger::updateLevels(const string& p_regexp, level p_level)
{
  std::regex l_matcher(p_regexp + ".*");
  std::lock_guard<std::mutex> l_lock(m_mutex);

  for (auto c_item : m_loggers)
  {
    if (true == std::regex_match(c_item.first, l_matcher))
      c_item.second->setLevel(p_level);
  }

  if (true == std::regex_match(string(), l_matcher))
    setLevel(p_level);
  return *this;
}

RootLogger::t_loggers::const_iterator
RootLogger::getClosestParent(const string& p_module) const
{
  auto                     c_res = m_loggers.end();
  vector<string>           l_parts;
  vector<string>::iterator c_part;

  boost::split(l_parts, p_module, boost::is_any_of("."), boost::token_compress_on);

  c_part = l_parts.end();
  while ((c_res == m_loggers.end()) && (c_part != l_parts.begin()))
  {
    c_part--;
    vector<string> l_tmp(l_parts.begin(), c_part);
    c_res = m_loggers.find(boost::join(l_tmp, "."));
  }

  return c_res;
}

}}
