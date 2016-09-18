#include "CounterManager.hh"

#include <fstream>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include "JsonVisitor.hh"
#include "FileVisitor.hh"
#include <log.hh> // libcore



namespace bfs = boost::filesystem;
namespace bp  = boost::posix_time;
namespace bt  = boost::this_thread;

namespace xtd {
namespace counters {

CounterManager::CounterManager(const uint32_t p_delay, const string& p_path) :
  m_refreshDelay(p_delay),
  m_snmpPath(p_path),
  m_counters(),
  m_isRunning(false)
{
}

CounterManager::~CounterManager(void)
{
}

status
CounterManager::start(void)
{
  if (m_counters.size() == 0)
  {
    log::crit("counters.manager", "no counter to monitor", HERE);
    return status::error;
  }

  t_counters::const_iterator l_it = m_counters.begin();

  for (; l_it != m_counters.end(); l_it++)
  {
    string l_path = m_snmpPath + "/" + (*l_it).first.string() + "/";
    bfs::create_directories(l_path);
  }

  m_isRunning = true;
  m_thread    = boost::thread(boost::bind(&CounterManager::refresh, this));

  return status::ok;
}

string
CounterManager::getSnmpPath(void) const
{
  return m_snmpPath;
}

const CounterManager::t_counters
CounterManager::getCounters(void)
{
  return m_counters;
}

status
CounterManager::add(Base::t_sptr  p_counter, const string& p_subDir)
{
  bfs::path l_subDir(p_subDir);
  m_counters.insert(std::make_pair(l_subDir, p_counter));
  return status::ok;
}

void
CounterManager::stop(void)
{
  if (m_isRunning)
  {
    m_isRunning = false;
    m_thread.interrupt();
    m_thread.join();
    writeOnDisk();
  }
}

void
CounterManager::toJson(boost::property_tree::ptree& p_json)
{
  string l_path;

  for (auto& c_data : m_counters)
  {
    l_path = c_data.first.string();
    boost::replace_all(l_path, "/", "!");

    boost::property_tree::ptree::path_type l_jsonPath(l_path, '!');
    JsonVisitor                            l_jsonVisitor(l_jsonPath, p_json);
    c_data.second->update();
    c_data.second->visit(l_jsonVisitor);
  }
}

void
CounterManager::refresh(void) const
{
  while (m_isRunning)
  {
    writeOnDisk();
    bt::sleep(bp::seconds(m_refreshDelay));
  }
}

void
CounterManager::writeOnDisk(void) const
{
  for (auto& c_data : m_counters)
  {
    bfs::path   l_path(bfs::path(m_snmpPath) / c_data.first);
    FileVisitor l_fileVisitor(l_path);

    c_data.second->update();
    c_data.second->visit(l_fileVisitor);
  }
}

}}
