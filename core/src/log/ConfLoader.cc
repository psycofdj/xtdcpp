# include <log/ConfLoader.hh>
# include <boost/algorithm/string/split.hpp>
# include <boost/algorithm/string/classification.hpp>
# include <boost/algorithm/string/case_conv.hpp>
# include "log/Appender.hh"
# include "log/Formatter.hh"
# include "log/Logger.hh"
# include "log/helpers.hh"
# include "log/MemoryAppender.hh"
# include "log/SyslogAppender.hh"
# include "log/StreamAppender.hh"
# include "log/Formatter.hh"
# include "log/ColoredFormatter.hh"
# include "error.hh"

namespace xtd {
namespace log {

bool
ConfLoader::configure(const map<string, string>& p_lines)
{
  for (auto c_line : p_lines)
  {
    boost::smatch l_matches;
    boost::regex  l_re("^log\\.logger\\.(.*)$");

    if (true == boost::regex_match(c_line.first, l_matches, l_re))
    {
      if (l_matches.size() == 2)
      {
        try {
          createLogger(l_matches[1], c_line.second, p_lines);
        } catch (const error& l_error) {
          std::cout << "unable to initialize loggers : " << l_error.what() << std::endl;
          return false;
        }
      }
    }
  }
  return true;
}

std::shared_ptr<Formatter>
ConfLoader::getFormatter(const string&              p_name,
                         const map<string, string>& p_properties)
{
  auto c_formatter = m_formatters.find(p_name);
  if (c_formatter != m_formatters.end())
    return c_formatter->second;

  auto c_value = p_properties.find("log.formatter." + p_name + ".class");
  if (c_value == p_properties.end())
    log::raise<log_error>("could not find formatter class key '%s'", "log.formatter." + p_name + ".class", HERE);

  auto c_constructor = m_formatterConstructors.find(c_value->second);
  if (c_constructor == m_formatterConstructors.end())
    log::raise<log_error>("unknown formatter class '%s' in key '%s'", c_value->second, c_value->first, HERE);

  std::shared_ptr<Formatter> l_result = c_constructor->second(p_name, p_properties);
  m_formatters.insert(std::make_pair(p_name, l_result));
  return l_result;
}

std::shared_ptr<Appender>
ConfLoader::getAppender(const string&              p_name,
                        const map<string, string>& p_properties)
{
  auto c_appender = m_appenders.find(p_name);
  if (c_appender != m_appenders.end())
    return c_appender->second;

  auto c_value = p_properties.find("log.appender." + p_name + ".class");
  if (c_value == p_properties.end())
    log::raise<log_error>("could not find appender class key '%s'", "log.appender." + p_name + ".class", HERE);

  auto c_constructor = m_appenderConstructors.find(c_value->second);
  if (c_constructor == m_appenderConstructors.end())
    log::raise<log_error>("unknown appender class '%s' in key '%s'", c_value->second, c_value->first, HERE);

  sptr<Appender> l_result = c_constructor->second(p_name, p_properties);

  c_value = p_properties.find("log.appender." + p_name + ".formatter");
  if (c_value != p_properties.end())
  {
    auto l_formatter = getFormatter(c_value->second, p_properties);
    l_result->setFormatter(l_formatter);
  }

  m_appenders.insert(std::make_pair(p_name, l_result));
  return l_result;
}


void
ConfLoader::createLogger(const string&              p_name,
                         const string&              p_value,
                         const map<string, string>& p_properties)
{
  vector<string> l_parts;
  vector<string> l_appenders;
  level          l_level;

  boost::split(l_parts, p_value, boost::is_any_of(", "), boost::token_compress_on);
  if (l_parts.size() < 2)
    log::raise<log_error>("ill format logger declaration : '%s'", p_value, HERE);

  if (false == is_valid(boost::to_lower_copy(l_parts[0])))
    log::raise<log_error>("invalid log level value '%s'", l_parts[0], HERE);

  l_level = from(l_parts[0]);
  std::copy(l_parts.begin() + 1, l_parts.end(), std::back_inserter(l_appenders));

  RootLogger& l_root   = getRoot();
  Logger*     l_logger = &l_root;

  if (p_name != "root")
    l_logger = &(l_root.get(p_name));

  l_logger->setLevel(l_level);
  l_logger->clearAppenders();
  for (auto c_appender : l_appenders)
  {
    auto l_appender = getAppender(c_appender, p_properties);
    l_logger->addAppender(l_appender);
  }
}

XTD_CORE_LOG_REGISTER_APPENDER(MemoryAppender);
XTD_CORE_LOG_REGISTER_APPENDER(SyslogAppender);
XTD_CORE_LOG_REGISTER_APPENDER(StreamAppender);
XTD_CORE_LOG_REGISTER_FORMATTER(Formatter);
XTD_CORE_LOG_REGISTER_FORMATTER(ColoredFormatter);

}}
