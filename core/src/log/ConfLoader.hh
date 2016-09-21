#ifndef XTD_CORE_LOG_CONFLOADER_HH_
# define XTD_CORE_LOG_CONFLOADER_HH_
# include <regex>
# include "types.hh"
# include "mixins/singleton.hh"
# include <iostream>
# include <boost/preprocessor/cat.hpp>

# define XTD_CORE_LOG_REGISTER_APPENDER(p_appender)               \
  namespace {                                                     \
  bool BOOST_PP_CAT(l_, p_appender) =                             \
    ConfLoader::get().registerAppender<p_appender>(#p_appender);  \
  }

# define XTD_CORE_LOG_REGISTER_FORMATTER(p_formatter)               \
  namespace {                                                       \
  static bool BOOST_PP_CAT(l_, p_formatter) =                       \
    ConfLoader::get().registerFormatter<p_formatter>(#p_formatter); \
  }



namespace xtd {
namespace log {

class Appender;
class Formatter;

class ConfLoader : public mixins::Singleton<ConfLoader>
{
public:
  typedef fn<sptr<Appender>(const string&, const map<string,string>&)>  t_appender_constructor;
  typedef fn<sptr<Formatter>(const string&, const map<string,string>&)> t_formatter_constructor;

public:
  bool configure(const map<string, string>& p_lines);

  template<typename T>
  bool registerAppender(const string& p_name)
  {
    m_appenderConstructors.insert(std::make_pair(p_name, [](const string& p_name, const map<string,string>& p_properties) -> std::shared_ptr<Appender> {
          return T::create(p_name, p_properties);
        }));
    return true;
  }

  template<typename T>
  bool registerFormatter(const string& p_name)
  {
    m_formatterConstructors.insert(std::make_pair(p_name, [](const string& p_name, const map<string,string>& p_properties) -> std::shared_ptr<Formatter> {
          return T::create(p_name, p_properties);
        }));
    return true;
  }

private:
  std::shared_ptr<Formatter> getFormatter(const string&              p_name,
                                          const map<string, string>& p_properties);

  std::shared_ptr<Appender> getAppender(const string&              p_name,
                                        const map<string, string>& p_properties);

  void createLogger(const string&              p_name,
                    const string&              p_value,
                    const map<string, string>& p_properties);

private:
  map<string, sptr<Appender>>          m_appenders;
  map<string, sptr<Formatter>>         m_formatters;
  map<string, t_appender_constructor>  m_appenderConstructors;
  map<string, t_formatter_constructor> m_formatterConstructors;
};

}}

#endif // !XTD_CORE_LOG_CONFLOADER_HH_
