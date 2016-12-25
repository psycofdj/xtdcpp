#ifndef CORE_LOG_LOGGER_HXX_
# define CORE_LOG_LOGGER_HXX_
# include "log/Appender.hh"

namespace xtd {
namespace log {


string
Logger::getName(void) const
{
  return m_name;
}

level
Logger::getLevel(void) const
{
  return m_level;
}

Logger&
Logger::setLevel(level p_level)
{
  m_level = p_level;
  return *this;
}

Logger&
Logger::setName(const string& p_name)
{
  m_name = p_name;
  return *this;
}

template<typename ... Args>
void
Logger::emerg(const string& p_format, Args... p_args) const
{
  log(level::emerg, p_format, p_args...);
}

template<typename ... Args>
void
Logger::alert(const string& p_format, Args... p_args) const
{
  log(level::alert, p_format, p_args...);
}

template<typename ... Args>
void
Logger::crit(const string& p_format, Args... p_args) const
{
  log(level::crit, p_format, p_args...);
}

template<typename ... Args>
void
Logger::err(const string& p_format, Args... p_args) const
{
  log(level::err, p_format, p_args...);
}

template<typename ... Args>
void
Logger::warning(const string& p_format, Args... p_args) const
{
  log(level::warning, p_format, p_args...);
}

template<typename ... Args>
void
Logger::info(const string& p_format, Args... p_args...) const
{
  log(level::info, p_format, p_args...);
}

template<typename ... Args>
void
Logger::notice(const string& p_format, Args... p_args) const
{
  log(level::notice, p_format, p_args...);
}

template<typename ... Args>
void
Logger::debug(const string& p_format, Args... p_args) const
{
  log(level::debug, p_format, p_args...);
}

template<typename ... Args>
void
Logger::log(level p_level, const string& p_format, Args... p_args) const
{
  if (m_level >= p_level)
  {
    Record l_rec;

    l_rec.m_time     = boost::posix_time::microsec_clock::local_time();
    l_rec.m_threadID = thread_counter::get_id();
    l_rec.m_format   = p_format;
    l_rec.m_level    = p_level;
    l_rec.m_module   = m_module;
    l_rec.m_name     = m_name;
    l_rec.m_pid      = getpid();
    l_rec.m_ppid     = getppid();

    for (auto c_appender : getAppenders())
      c_appender->log(l_rec, p_args...);
  }
}

template<typename ... Args>
void
RootLogger::emergTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).emerg(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::alertTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).alert(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::critTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).crit(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::errTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).err(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::warningTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).warning(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::infoTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).info(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::noticeTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).notice(p_format, p_args...);
}

template<typename ... Args>
void
RootLogger::debugTo(const string& p_module, const string& p_format, Args... p_args)
{
  get(p_module).debug(p_format, p_args...);
}

}}

#endif // !CORE_LOG_LOGGER_HXX_
