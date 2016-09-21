#include "param/Handler.hh"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <error.hh> // libcore
#include <log.hh>   // libcore
#include "param/Base.hh"
#include "param/Visitor.hh"


using boost::filesystem::create_directories;
using boost::filesystem::is_directory;



namespace bfs = boost::filesystem;


namespace xtd {
namespace servers {
namespace param {

Handler::Handler(const string& p_actionPath) :
  m_actionPath(p_actionPath),
  m_params()
{
  if (false == is_directory(m_actionPath))
  {
    log::info("servers.param.handler", "action directory '%s' does not exist, trying to create it", m_actionPath, HERE);
    if (false == create_directories(m_actionPath))
    {
      if (false == is_directory(m_actionPath))
        log::raise<param_error>("servers.param.handler", "failed to create action directory '%s', exit now!", m_actionPath, HERE);
    }
    else
    {
      log::info("servers.param.handler", "action directory '%s' created", m_actionPath, HERE);
    }
  }
}


void
Handler::initialize(void)
{
  for (auto& c_param : m_params)
  {
    // Only if parameter is synchronized
    if(c_param.second->getSync())
    {
      bfs::path l_path(m_actionPath + "/" + c_param.second->getName());

      if (false == is_regular_file(l_path))
      {
        log::info("servers.param.handler", "can't find persistent file to initalize parameter '%s', path = %s", c_param.first, l_path, HERE);
        c_param.second->sync();
      }
      else
      {
        // Persistent file found, read value and initialize parameter
        ifstream l_file(l_path.c_str());
        string   l_content;

        l_content.assign(std::istreambuf_iterator<char>(l_file), std::istreambuf_iterator<char>());
        l_file.close();
        if (false == c_param.second->fromStr(l_content))
          log::crit("servers.param.handler", "unable to initialize param '%s' from value '%s'", c_param.first, l_content, HERE);
      }
    }
  }
}


void
Handler::sync(const string& p_name)
{
  t_params::iterator c_param = find(p_name);

  if (c_param != m_params.end())
  {
    c_param->second->sync();
  }
}

bool
Handler::exists(const string& p_name)
{
  return (find(p_name) != m_params.end());
}

bool
Handler::verify(const string& p_name, const string& p_src)
{
  t_params::iterator c_param = find(p_name);
  if (c_param == m_params.end())
    return false;
  return c_param->second->verify(p_src);
}

bool
Handler::fromStr(const string& p_name, const string& p_src, const string& p_log)
{
  bool l_ret = false;
  t_params::iterator c_param = find(p_name);
  if (c_param != m_params.end())
  {
    if((l_ret = c_param->second->fromStr(p_src)))
    {
      c_param->second->setLog(p_log);
    }
  }
  return l_ret;
}

void
Handler::accept(Visitor& p_visitor) const
{
  BOOST_FOREACH(const t_params::value_type& c_param, m_params)
    c_param.second->accept(p_visitor);
}

Handler::t_param_sptr
Handler::add(const string& p_name, t_param_sptr p_param, bool p_syncDisk)
{
  string l_lowerName = p_name;
  boost::algorithm::to_lower(l_lowerName);

  p_param->setName(p_name);
  p_param->setSync(p_syncDisk);
  p_param->setActionPath(m_actionPath);
  m_params.insert(make_pair(l_lowerName, p_param));

  if (p_syncDisk)
    p_param->listen(boost::bind(&Handler::sync, this, p_name));

  return p_param;
}

Handler::t_params::iterator
Handler::find(const string& p_name)
{
  string l_lowerName = p_name;
  boost::algorithm::to_lower(l_lowerName);
  return m_params.find(l_lowerName);
}

}}}
