#ifndef XTD_CORE_LOG_CONFLOADER_HH_
# define XTD_CORE_LOG_CONFLOADER_HH_
# include <boost/preprocessor.hpp>
# include "types.hh"
# include "log/helpers.hh"
# include "mixins/singleton.hh"

class TestConfLoader;

namespace xtd { namespace log { class Appender; } }
namespace xtd { namespace log { class ConfLoader; } }
namespace xtd { namespace log { class Formatter; } }
namespace xtd { namespace log { class RootLogger; } }


/**
 ** @brief Registers an @ref xtd::log::Appender "Appender" class type to global
 **        @ref xtd::log::ConfLoader "ConfLoader" instance
 ** @param p_appender @ref xtd::log::Appender "Appender" class name
 */
#define XTD_CORE_LOG_REGISTER_APPENDER(p_appender)                \
  namespace {                                                     \
  bool BOOST_PP_CAT(l_, p_appender) __attribute__((unused))  =    \
    ConfLoader::get().registerAppender<p_appender>(#p_appender);  \
  }

/**
 ** @brief Registers an @ref xtd::log::Formatter "Formatter" to class type to global
 **        @ref xtd::log::ConfLoader "ConfLoader" instance
 ** @param p_formatter @ref xtd::log::Formatter "Formatter" class name
 */
#define XTD_CORE_LOG_REGISTER_FORMATTER(p_formatter)                  \
  namespace {                                                         \
  static bool BOOST_PP_CAT(l_, p_formatter) __attribute__((unused)) = \
    ConfLoader::get().registerFormatter<p_formatter>(#p_formatter);   \
  }


namespace xtd {
namespace log {


/**
 ** @brief Initializes XTD's logging facility from property map
 ** @details
 **
 ** This singleton object creates and configures Logger, Formatter and Appender
 ** objects from a (key,value) property map given to
 ** @ref ConfLoader::configure "configure" method.
 **
 ** ### Class registration
 **
 ** You can specify which class name of Appender and Formatter you want to
 ** instantiate with the <b> .class </b> keys of the property map.
 **
 ** By default, ConfLoader only knows about XTD's formatters and appenders classes.
 **
 ** If you want to use your own class types you must first register them to
 ** ConfLoader's factory with the following methods :
 ** - @ref ConfLoader::registerFormatter "registerFormatter<T>" :
 **   - @copybrief ConfLoader::registerFormatter
 ** - @ref ConfLoader::registerAppender "registerAppender<T>" :
 **   - @copybrief ConfLoader::registerAppender
 **
 ** You may also use following pre-defined macros to statically register your
 ** classes :
 ** - @ref XTD_CORE_LOG_REGISTER_FORMATTER :
 **   - @copybrief XTD_CORE_LOG_REGISTER_FORMATTER
 ** - @ref XTD_CORE_LOG_REGISTER_APPENDER
 **   - @copybrief XTD_CORE_LOG_REGISTER_APPENDER
 **
 ** Registered classes must implement the following function prototype :
 ** @code
 ** bool MyClass::create(const string& p_name, const map<string,string>& p_props);
 ** @endcode
 **
 **
 ** ### Configuration
 **
 ** Properties defined loggers, appenders and formatters.
 **
 ** Each kind of object are identified by a key build from
 ** a @b prefix and a @b name
 **
 ** - *log.logger.@<name@>* : defines a Logger
 ** - *log.appender.@<name@>* defines an Appender
 ** - *log.formatter.@<name@>* : defines a Formatter
 **
 ** The value of these keys depends on the object type and is described below.
 **
 ** #### %Logger definition
 **
 ** The defining key of a Logger must follow the following syntax :
 ** @code
 ** log.logger.<name> = <level>[, <appender_name>]
 ** @endcode
 **
 ** Where :
 ** - @<name@>
 **   - Logger's name which will be accessible from
 **   - @code log::getRoot().get(<name>) @endcode
 **   - The special value **root** stands for the @ref RootLogger itself.
 **
 ** - @<level@>
 **   - The @ref level threshold label to use for this logger
 **
 ** - @<appender_name@>
 **   - A comma separated list of appender names to add on defined logger.
 **   - Each name must correspond to a valid appender definition (see
 **     below).
 **   - The list of appender names may be empty.
 **
 **
 ** #### %Appender definition
 **
 ** For each @<appender_name@> found in logger definitions, the following key must
 ** be defined :
 ** @code
 ** log.appender.<appender_name>.class = <class_name>
 ** @endcode
 **
 ** Where :
 ** - @<class_name@>
 **   - An Appender class name registered to the ConfLoader object
 **
 **
 ** In addition, the following key may be defined :
 ** @code
 ** log.appender.<appender_name>.formatter = <formatter_name>
 ** @endcode
 **
 ** Where :
 ** - @<formatter_name@>
 **   - The name of the defined formatter to use with this appender
 **
 ** Other keys may be required depending of the requested appender class. Please
 ** refer class documentation for details.
 **
 ** #### %Formatter definition
 **
 ** For each @<formatter_name@> found in appender definitions, the following key must
 ** be defined :
 ** @code
 ** log.formatter.<formatter_name>.class = <class_name>
 ** @endcode
 **
 ** Where :
 ** - @<class_name@>
 **   - An Formatter class name registered to the ConfLoader object
 **
 ** Other keys may be required depending of the requested formatter class. Please
 ** refer class documentation for details.
 **
 ** ### Exemple
 **
 ** The following properties defines three loggers with difference log levels :
 ** - the RootLogger (named root)
 ** - a logger for module *proxy*
 ** - a logger for submodule *proxy.sensible*
 **
 ** The root logger send log records to two appenders, *A1* and *A2* and the other
 ** loggers send records only to *A1*.
 **
 ** *A2* uses a formatter named *F1* which tweaks how location informations are
 ** displayed and colors module name in red.
 **
 ** @code
 ** log.logger.root                       = err, A1, A2
 ** log.logger.proxy                      = debug, A2
 ** log.logger.proxy.sensible             = notice, A2
 **
 ** log.appender.A1.class                 = SyslogAppender
 ** log.appender.A1.identity              = my-bin
 ** log.appender.A1.facility              = LOG_USER
 ** log.appender.A1.options               = LOG_PID
 **
 ** log.appender.A2.class                 = StreamAppender
 ** log.appender.A2.fd                    = cerr
 ** log.appneder.A2.formatter             = F1
 **
 ** log.formatter.F1.class                = ColoredFormatter
 ** log.formatter.F1.layout.location      = %(file):%(line)
 ** log.formatter.F1.style.module.fgcolor = red
 ** @endcode
 **
 ** @b Note: All three loggers use the same *A2* formatter instance.
 **
 ** If you want to use distinct appender or formatters objects, you must define
 ** each one of them.
 */
class ConfLoader : public mixins::Singleton<ConfLoader>
{
  friend class ::TestConfLoader;

private:
  typedef map<string,string>                                      t_properties;
  typedef fn<sptr<Appender>(const string&, const t_properties&)>  t_appender_constructor;
  typedef fn<sptr<Formatter>(const string&, const t_properties&)> t_formatter_constructor;

public:
  /**
   ** @brief Initialize logging facility from properties
   ** @param p_properties logging configuration
   ** @param p_root Target root logger, default is global Rootlogger
   ** @throw log_error when encountered error in given properties
   */
  void configure(const t_properties& p_properties,
                 RootLogger&         p_root = getRoot());

  /**
   ** @brief Registers given class type to appender factory
   ** @tparam TAppender Appender class type
   ** @param p_name Appender's name
   ** @return true
   */
  template<typename TAppender>
  bool registerAppender(const string& p_name);

  /**
   ** @brief Registers given class type to formatter factory
   ** @tparam TFormatter Formatter class type
   ** @param p_name Formnatter's name
   ** @return true
   */
  template<typename TFormatter>
  bool registerFormatter(const string& p_name);

private:
  sptr<Formatter> getFormatter(const string&       p_name,
                               const t_properties& p_properties);

  sptr<Appender> getAppender(const string&       p_name,
                             const t_properties& p_properties);

  void createLogger(const string&       p_name,
                    const string&       p_value,
                    const t_properties& p_properties,
                    RootLogger&         p_root);

private:
  map<string, sptr<Appender>>          m_appenders;
  map<string, sptr<Formatter>>         m_formatters;
  map<string, t_appender_constructor>  m_appenderConstructors;
  map<string, t_formatter_constructor> m_formatterConstructors;
};



template<typename T>
bool
ConfLoader::registerAppender(const string& p_name)
{
  auto l_constructor = [](const string& p_name, const map<string,string>& p_properties) -> std::shared_ptr<Appender> {
    return T::create(p_name, p_properties);
  };

  m_appenderConstructors.insert(std::make_pair(p_name, l_constructor));
  return true;
}

template<typename T>
bool
ConfLoader::registerFormatter(const string& p_name)
{
  auto l_constructor = [](const string& p_name, const map<string,string>& p_properties) -> std::shared_ptr<Formatter> {
    return T::create(p_name, p_properties);
  };

  m_formatterConstructors.insert(std::make_pair(p_name, l_constructor));
  return true;
}

}}

#endif // !XTD_CORE_LOG_CONFLOADER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
