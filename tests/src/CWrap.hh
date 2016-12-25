#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "types.hh"            // /libcore
#include "mixins/singleton.hh" // /libcore
#include <boost/any.hpp>
#include <tuple>
#include <map>

namespace xtd {
namespace tests {

class CWrap : private mixins::Singleton<CWrap>
{
public:
  typedef std::pair<string, uint32_t>            t_state;
  typedef vector<t_state>                        t_config;
  typedef map<string, t_config>                  t_data;
  typedef std::function<void(const boost::any&)> t_hook;
  typedef std::multimap<string, t_hook>          t_hooks;

public:
  CWrap(void);

public:
  class state
  {
  public:
    state(const string& p_name, const t_config& p_conf);
    ~state(void);
  private:
    string m_name;
    t_config m_base;
  };

  class hook_state
  {
  public:
    hook_state(const string& p_name, const t_hook& p_conf);
    ~hook_state(void);
  private:
    string  m_name;
    t_hooks m_base;
  };

public:
  static string gen(const string& p_funcName);
  static void   set(const string& p_name, const t_config& p_config);
  static void   reset(void);
  static void   runHooks(const string& p_name, const boost::any& p_args);
  static void   addHook(const string& p_name, const t_hook& p_hook);
  static void   setHooks(const t_hooks& p_hooks);

private:
  void         initialize(void);
  string       generate(const string& p_funcName);
  void         add(const string& p_name, const t_config& p_config);
  t_config     getConfig(const string& p_name);
  t_hooks      getHooks(void);

private:
  t_data  m_data;
  t_data  m_origin;
  t_hooks m_hooks;
};

}}
