#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "types.hh"           // /libcommon
#include "mixin/Singleton.hh" // /libcommon

namespace xtd {
namespace tests {

class CWrap : private mixin::Singleton<CWrap>
{
public:
  typedef std::pair<string, uint32_t> t_state;
  typedef vector<t_state>        t_config;
  typedef map<string, t_config>  t_data;

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

public:
  static string gen(const string& p_funcName);
  static void   set(const string& p_name, const t_config& p_config);
  static void   reset(void);

private:
  void         initialize(void);
  string       generate(const string& p_funcName);
  void         add(const string& p_name, const t_config& p_config);
  t_config     getConfig(const string& p_name);

private:
  t_data m_data;
  t_data m_origin;
};

}}
