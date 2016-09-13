#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

class LibCWrapper
{
public:
  typedef std::pair<std::string, std::uint32_t> t_state;
  typedef std::vector<t_state>                  t_config;
  typedef std::map<std::string, t_config>       t_data;

public:
  LibCWrapper(void);

public:
  static std::string get(const std::string& p_funcName);
  static void        set(const std::string& p_name, const t_config& p_config);
  static void        reset(void);

private:
  void        initialize(void);
  std::string generate(const std::string& p_funcName);
  void        add(const std::string& p_name, const t_config& p_config);

private:
  t_data                              m_data;
  t_data                              m_origin;
  static std::shared_ptr<LibCWrapper> ms_instance;
};
