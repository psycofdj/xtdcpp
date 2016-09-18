#ifndef CORE_LOG_LOGGER_HH_
# define CORE_LOG_LOGGER_HH_
# include "log/logtypes.hh"
# include <mutex>
# include <thread>
# include "log/Appender.hh"
# include "mixins/shared.hh"

namespace xtd {
namespace log {

class Logger
{
protected:
  typedef vector<sptr<Appender>> t_appenders;

  struct thread_counter
  {
  public:
    static int32_t get_id(void);
  private:
    static map<size_t, uint32_t> ms_ids;
    static std::mutex            ms_mutex;
    static size_t                ms_mainID;
  };

public:
  Logger(void);
  Logger(const Logger&) = delete;
  Logger(const Logger& p_parent, const string& p_module);
  virtual ~Logger(void);

public:
  inline string  getName(void)  const;
  inline level   getLevel(void) const;
  inline Logger& setName(const string& p_name);
  inline Logger& setLevel(level p_level);
  Logger&        addAppender(const sptr<Appender>& p_formatter);
  Logger&        replaceAppenders(const sptr<Appender>& p_formatter);
  Logger&        clearAppenders(void);

  template<typename ... Args>
  void emerg(const string& p_format, Args... p_args) const;
  template<typename ... Args>
  void alert(const string& p_format, Args... p_args);
  template<typename ... Args>
  void crit(const string& p_format, Args... p_args);
  template<typename ... Args>
  void err(const string& p_format, Args... p_args);
  template<typename ... Args>
  void warning(const string& p_format, Args... p_args);
  template<typename ... Args>
  void info(const string& p_format, Args... p_args...);
  template<typename ... Args>
  void notice(const string& p_format, Args... p_args);
  template<typename ... Args>
  void debug(const string& p_format, Args... p_args);
  template<typename ... Args>
  void log(level p_level, const string& p_format, Args... p_args) const;

private:
  const t_appenders getAppenders(void) const;

private:
  string             m_module;
  string             m_name;
  level              m_level;
  t_appenders        m_appenders;
  mutable std::mutex m_mutex;
};

/* ----------------------------------------------------------------------- */

class RootLogger : public Logger
{
private:
  typedef map<string, sptr<Logger>> t_loggers;
  typedef map<string, level>        t_levels;

public:
  RootLogger(void);
  RootLogger(const RootLogger&) = delete;

public:
  Logger&        get(const string& p_module);
  const t_levels getLevels(void) const;
  RootLogger&    initialize(const string& p_name, level p_level);
  RootLogger&    setAllLevels(level p_level);
  RootLogger&    setAllValueLevels(uint32_t p_level);
  RootLogger&    clearAll(level p_default);
  RootLogger&    updateLevels(const string& p_module, level p_level);


public:
  level       getLevelTo(const string& p_module) const;
  RootLogger& setLevelTo(const string& p_module, level p_level);
  template<typename ... Args>
  void emergTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void alertTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void critTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void errTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void warningTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void infoTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void noticeTo(const string& p_module, const string& p_format, Args... p_args);
  template<typename ... Args>
  void debugTo(const string& p_module, const string& p_format, Args... p_args);

private:
  t_loggers::iterator getClosestParent(const string& p_module);

private:
  t_loggers          m_loggers;
  mutable std::mutex m_mutex;

};

}}

# include "log/Logger.hxx"

#endif // !CORE_LOG_LOGGER_HH_
