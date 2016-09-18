#ifndef COMMON_LOG_HH_
# define COMMON_LOG_HH_
# include <iostream>
# include <vector>
# include <map>
# include <syslog.h>
# include <stdarg.h>
# include "types.hh"
# define HERE __PRETTY_FUNCTION__,__FILE__,__LINE__

namespace xtd {

class logger
{
public:
  enum class level : int32_t {
    emerg   = LOG_EMERG,
    alert   = LOG_ALERT,
    crit    = LOG_CRIT,
    err     = LOG_ERR,
    warning = LOG_WARNING,
    notice  = LOG_NOTICE,
    info    = LOG_INFO,
    debug   = LOG_DEBUG
  };

private:
  typedef map<string, level> t_levels;

public:
  logger(void);

public:
  void     initialize(void);
  void     initialize(const string& p_name, level p_level);
  void     setLevel(const string& p_module, level p_level);
  void     updateLevels(const string& p_regex, level p_level);
  void     setAllLevels(level p_level);
  void     setAllValueLevels(uint32_t p_level);
  void     clearAll(level p_default);
  void     setName(const string& p_name);

  level    levelOf(const uint32_t& p_level);
  uint32_t valueOf(level p_level);
  string   stringOf(level p_level);
  level    fromString(string p_level);

  level    getLevel(const string& p_module);
  t_levels getLevels(void);
  bool     isValidStringLevel(string p_level);

  static logger&  get(void);

  template<typename ... Args>
  static void emerg(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void alert(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void crit(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void err(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void warning(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void info(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void notice(const string& p_module, const string& p_format, Args... p_args);

  template<typename ... Args>
  static void debug(const string& p_module, const string& p_format, Args... p_args);

private:
  void  wrap_syslog(int __pri, const char *__fmt, ...);

  template<typename ... Args>
  void log(level p_level, const string& p_module, const string& p_format, Args... p_args);

private:
  string         m_name;
  bool           m_initialized;
  t_levels       m_levels;
  static logger* ms_pInstance;
};

}

# include "logger.hxx"

#endif // !COMMON_LOG_HH_
