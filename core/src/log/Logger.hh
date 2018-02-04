#ifndef CORE_LOG_LOGGER_HH_
# define CORE_LOG_LOGGER_HH_
# include <mutex>
# include "types.hh"
# include "log/logtypes.hh"

class TestLogger;

namespace xtd { namespace log { class Appender; } }
namespace xtd {
namespace log {


/**
 ** @brief Handles log record and log @ref level
 ** @details
 **
 ** The Logger handles user logs by filtering those under configured @ref level
 ** and propagating the others to its registered @ref Appender objects.
 **
 ** ### Object methods
 **
 ** - @ref Logger::getLevel "getLevel()"
 **   - @copybrief Logger::getLevel
 ** - @ref Logger::setLevel "setLevel(level)"
 **   - @copybrief Logger::setLevel
 ** - @ref Logger::addAppender "addAppender(object)"
 **   - @copybrief Logger::addAppender
 ** - @ref Logger::clearAppenders "clearAppenders()"
 **   - @copybrief Logger::clearAppenders
 ** - @ref Logger::log "log(format, args...)"
 **   - @copybrief Logger::log
 **
 ** ### Variadic arguments format
 **
 ** @ref Logger::log and all following logging shorthands allow to provide a format
 ** message with variadic template argument. Functionally, its equivalent to plain
 ** old **C-printf** argument but allows to pass std::string and user-defined
 ** objects. We internally rely on boost::format, please see
 ** http://www.boost.org/doc/libs/1_58_0/libs/format/doc/format.html for more details
 **
 ** @code
 ** string  l_name = "my name";
 ** Logger& l_obj  = log::getRoot();
 **
 ** l_obj.log(level::err, "message %d variadic args : %s, %.2f", 2, l_name, 2.256);
 ** @endcode
 **
 **
 ** ### Logging shorthands
 **
 ** - @ref Logger::emerg "emerg(format, args...)"
 **   - @copybrief Logger::emerg
 ** - @ref Logger::alert "alert(format, args...)"
 **   - @copybrief Logger::alert
 ** - @ref Logger::crit "crit(format, args...)"
 **   - @copybrief Logger::crit
 ** - @ref Logger::err "err(format, args...)"
 **   - @copybrief Logger::err
 ** - @ref Logger::warning "warning(format, args...)"
 **   - @copybrief Logger::warning
 ** - @ref Logger::notice "notice(format, args...)"
 **   - @copybrief Logger::notice
 ** - @ref Logger::info "info(format, args...)"
 **   - @copybrief Logger::info
 ** - @ref Logger::debug "debug(format, args...)"
 **   - @copybrief Logger::debug
 **
 ** @code
 ** Logger& l_obj  = log::getRoot();
 **
 ** l_obj.emerg("emergency message");
 ** l_obj.alert("alert message");
 ** l_obj.crit("critical message");
 ** l_obj.err("error message");
 ** l_obj.warning("warning message");
 ** l_obj.notice("notice message");
 ** l_obj.info("info message");
 ** l_obj.debug("debug message");
 ** @endcode
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** The class uses a mutex-based locking that provides thread safeness for all
 ** read and write operations on internal Appender list.
 ** However record forwarding to each appender it not lock. The Appender object
 ** is responsible of its own thread safeness.
 */
class Logger : public std::enable_shared_from_this<Logger>
{
  friend class ::TestLogger;

protected:
  /**
   ** @brief Type for child Appender list
   */
  typedef vector<sptr<Appender>> t_appenders;

  /**
   ** @brief Generates growing thread identifiers
   ** @details
   ** Internally maintain a conversion map from std::thread::id to int32_t.
   **
   ** This object is thread safe.
   */
  struct thread_counter
  {
  public:
    /**
     ** @brief Generate identifier for current thread
     */
    static int32_t get_id(void);
  private:
    static map<size_t, uint32_t> ms_ids;
    static std::mutex            ms_mutex;
    static size_t                ms_mainID;
  };

public:
  /**
   ** @brief Default Constructor
   */
  Logger(void);

  /**
   ** @brief Deleted copy constructor
   */
  Logger(const Logger&) = delete;

  /**
   ** @brief Constructs for a specific module
   ** @param p_parent Parent Logger object
   ** @param p_module Module name
   ** @details
   ** Logger will inherit parent settings :
   ** - current log @ref level
   ** - name
   ** - list of @ref Appender
   */
  Logger(const Logger& p_parent, const string& p_module);

  /**
   ** @brief Destructor
   */
  virtual ~Logger(void);

public:
  /**
   ** @brief Get logger name
   */
  inline string getName(void)  const;

  /**
   ** @brief Get current logger @ref level
   */
  inline level getLevel(void) const;

  /**
   ** @brief Set logger name
   ** @param p_name new logger name
   ** @return self
   */
  inline Logger& setName(const string& p_name);

  /**
   ** @brief Set logger active log @ref level
   ** @param p_level new level
   ** @return self
   */
  inline Logger& setLevel(level p_level);

  /**
   ** @brief Add an @ref Appender to internal list
   ** @param p_appender Appender to add
   ** @return self
   */
  Logger& addAppender(const sptr<Appender>& p_appender);

  /**
   ** @brief Remove all registered @ref Appender
   ** @return self
   */
  Logger& clearAppenders(void);

  /**
   ** @brief Trigger @ref level::emerg log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void emerg(const string& p_format, Args... p_args) const;

  /**
   ** @brief Trigger @ref level::alert log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void alert(const string& p_format, Args... p_args) const;


  /**
   ** @brief Trigger @ref level::crit log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void crit(const string& p_format, Args... p_args) const;


  /**
   ** @brief Trigger @ref level::err log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void err(const string& p_format, Args... p_args) const;

  /**
   ** @brief Trigger @ref level::warning log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void warning(const string& p_format, Args... p_args) const;


  /**
   ** @brief Trigger @ref level::notice log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void notice(const string& p_format, Args... p_args) const;

  /**
   ** @brief Trigger @ref level::info log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void info(const string& p_format, Args... p_args) const;

  /**
   ** @brief Trigger @ref level::debug log
   ** @tparam Args Variable template argument list
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
  template<typename ... Args>
  void debug(const string& p_format, Args... p_args) const;

  /**
   ** @brief Trigger @ref level::emerg log
   ** @tparam Args Variable template argument list
   ** @param p_level @ref level of the debug record
   ** @param p_format boost::format compatible format message
   ** @param p_args arguments to apply to given p_format
   */
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

/**
 ** @brief Manage @ref Logger childs, one for each requests module
 ** @details
 **
 **
 ** This object is responsible for creating and serving Logger for requested
 ** modules.
 **
 ** In addition, it offer convenient methods to forward calls to
 ** targeted module Logger.
 **
 ** ### Methods
 **
 ** - @ref RootLogger::get "get(module)"
 **   - @copybrief RootLogger::get
 ** - @ref RootLogger::getLevels "getLevels()"
 **   - @copybrief RootLogger::getLevels
 ** - @ref RootLogger::setAllLevels "setAllLevels(level)"
 **   - @copybrief RootLogger::setAllLevels
 ** - @ref RootLogger::setAllValueLevels "setAllValueLevels(int)"
 **   - @copybrief RootLogger::setAllValueLevels
 ** - @ref RootLogger::updateLevels "updateLevels(regexp, level)"
 **   - @copybrief RootLogger::updateLevels
 **
 **
 ** ### Shorthand methods
 **
 ** - @ref RootLogger::initialize "initialize(name, level)"
 **   - @copybrief RootLogger::initialize
 ** - @ref RootLogger::getLevelTo "getLevelTo(module)"
 **   - @copybrief RootLogger::getLevelTo
 ** - @ref RootLogger::setLevelTo "setLevelTo(module, level)"
 **   - @copybrief RootLogger::setLevelTo
 ** - @ref RootLogger::alertTo "alertTo(module, format, args...)"
 **   - @copybrief RootLogger::alertTo
 ** - @ref RootLogger::critTo "critTo(module, format, args...)"
 **   - @copybrief RootLogger::critTo
 ** - @ref RootLogger::warningTo "warningTo(module, format, args...)"
 **   - @copybrief RootLogger::warningTo
 ** - @ref RootLogger::infoTo "infoTo(module, format, args...)"
 **   - @copybrief RootLogger::infoTo
 ** - @ref RootLogger::noticeTo "noticeTo(module, format, args...)"
 **   - @copybrief RootLogger::noticeTo
 ** - @ref RootLogger::debugTo "debugTo(module, format, args...)"
 **   - @copybrief RootLogger::debugTo
 **
 **
 ** ### Modules and hierarchy
 **
 ** When a Logger is requested by any of the object methods referring to the
 ** **p_module** parameter, the RootLogger will either return the previously
 ** created Logger if it exists, or will create a new instance.
 ** Newly created instances inherit all parameters from their closest parent
 ** in the module hierarchy. Ultimately, the RootLogger itself will be used
 ** create the new instance if no parent were found.
 **
 ** Hierarchy among module is determined by their name. Each period **.** found
 ** in the **p_module** string creates a new layer in the hierarchy tree.
 ** Example :
 ** - module **"a.b.c"** is direct child of **"a.b"**, which itself child
 **   of **"a"**
 **
 ** When looking for the closest parent of a module, we're looking for the closest
 ** ancestor in this hierarchy.
 ** Example :
 ** - module "a.b.c" as closest parent "a" (assuming "a.b" was never created)
 **
 ** @code
 ** RootLogger& l_root = log::getRoot();
 **
 ** l_root.setLevel(level::crit);
 **
 ** Logger& l_module1 = l_root.get("a"); // logger for module "a"
 ** assert(l_module1.getLevel() == level::crit);
 **
 ** l_module1.setLevel(level::debug);
 **
 ** // new module will be constructed from closest parent : module "a"
 ** Logger& l_module2 = l_root.get("a.b.c"); // logger for module "a.b.c"
 ** assert(l_module2.getLevel() == level::debug);
 **
 ** // Once created for a given module, Logger are kept in memory
 ** Logger& l_ref1 = l_root.get("a");
 ** l_ref1.setLevel(level::alert);
 **
 ** Logger& l_ref2 = l_root.get("a.b.c");
 ** assert(l_module1.getLevel() == level::alert);
 ** assert(l_module2.getLevel() == level::debug);
 ** @endcode
 **
 **
 ** ### Multiple RootLogger
 **
 ** Using multiple instances of RootLogger is possible. However, modules should
 ** give you already enough flexibility. If you really need to, be aware that
 ** all helper free functions defined in the namespace uses a single static
 ** RootLogger instance.
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** The class uses a mutex-based locking that provides thread safeness for all
 ** read and write operations on internal child Logger list.
 **
 ** Calls to child logger methods are not protected, Logger object is
 ** responsible of its own thread safeness.
 */
class RootLogger : public Logger
{
  friend class ::TestLogger;

private:
  typedef map<string, sptr<Logger>> t_loggers;
  typedef map<string, level>        t_levels;

public:
  /**
   ** @brief Constructor
   */
  RootLogger(void);

  /**
   ** @brief Deleted copy constructor
   */
  RootLogger(const RootLogger&) = delete;

public:
  /**
   ** @brief Retrieve logger for given module
   ** @param p_module module name
   ** @return Logger for given module
   ** @details
   ** This function searches its internal data for a previously created logger
   ** associated to module name p_module. If none found, a new Logger is created
   ** and will inherit all parameters if its closest parent.
   */
  Logger& get(const string& p_module);

  /**
   ** @brief Returns @ref level for all child loggers
   ** @return map associating module name to their corresponding @ref level
   */
  const t_levels getLevels(void) const;

  /**
   ** @brief Shortand for @ref Logger::setName and @ref Logger::setLevel
   ** @param p_name New module name
   ** @param p_level New active log @ref level
   ** @return self
   */
  RootLogger& initialize(const string& p_name, level p_level);

  /**
   ** @brief Set active log level for all child loggers
   ** @param p_level new active log level
   ** @return self
   */
  RootLogger& setAllLevels(level p_level);


  /**
   ** @brief Same as @ref RootLogger::setAllLevels with integer level value
   ** @param p_level integer level value
   ** @return self
   */
  RootLogger& setAllValueLevels(uint32_t p_level);

  /**
   ** @brief Set active log level for all child loggers matchin given regexp prefix
   ** @param p_regexpPrefix Logger name matching regexp
   ** @param p_level new active log @ref level
   ** @return self
   ** @details
   ** Given regexp is considered as a prefix. Internally, this method will match
   ** on "@<prefix_regex@>.*".
   */
  RootLogger& updateLevels(const string& p_regexpPrefix, level p_level);

public:
  /**
   ** @brief Returns level applicable to module.
   ** @param p_module module name
   ** @return active current level of given module
   ** @details
   ** This method is @b not equivalent to get(p_module).getLevel() when logger for
   ** module does not exist. In that case, the method returns the level of closest
   ** parent in hierarchy.
   */
  level getLevelTo(const string& p_module) const;

  /**
   ** @brief Shorthand for get(p_module).setLevel(p_level)
   ** @param p_module module name
   ** @param p_level new active log @ref level
   ** @return self
   */
  RootLogger& setLevelTo(const string& p_module, level p_level);

  /**
   ** @brief Shorthand for get(p_module).emerg(p_format, p_args)
   */
  template<typename ... Args>
  void emergTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).alert(p_format, p_args)
   */
  template<typename ... Args>
  void alertTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).crit(p_format, p_args)
   */
  template<typename ... Args>
  void critTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).err(p_format, p_args)
   */
  template<typename ... Args>
  void errTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).warning(p_format, p_args)
   */
  template<typename ... Args>
  void warningTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).info(p_format, p_args)
   */
  template<typename ... Args>
  void infoTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).notice(p_format, p_args)
   */
  template<typename ... Args>
  void noticeTo(const string& p_module, const string& p_format, Args... p_args);

  /**
   ** @brief Shorthand for get(p_module).debug(p_format, p_args)
   */
  template<typename ... Args>
  void debugTo(const string& p_module, const string& p_format, Args... p_args);

private:
  t_loggers::const_iterator getClosestParent(const string& p_module) const;

private:
  t_loggers          m_loggers;
  mutable std::mutex m_mutex;
};

}}

# include "log/Logger.hxx" // IWYU pragma: export

#endif // !CORE_LOG_LOGGER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
