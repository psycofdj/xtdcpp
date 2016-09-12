#ifndef COMMON_APPLICATION_HH_
# define COMMON_APPLICATION_HH_


# include <iostream>
# include <iomanip>
# include <limits>
# include <map>
# include <functional>
# include <getopt.h>
# include <libgen.h>
# include <stdlib.h>
# include <boost/system/error_code.hpp>
# include <boost/thread.hpp>
# include <boost/asio.hpp>
# include <boost/asio/signal_set.hpp>
# include "types.hh"
# include "logger.hh"


class TestApplication;

namespace xtd {


/**
 ** @brief Parses arguments from main(int,char**) function
 ** @details
 ** - @ref sec_intro
 ** - @ref sec_howto
 ** - @ref sec_execution_flow
 ** - @ref sec_binding_options
 **
 ** @section sec_intro Introcution
 ** This object provides a default application skeleton including :
 ** - user-friendly wrapping of argument parsing (on top of C's getopt_long)
 ** - usage generator for declared options
 ** - default options : --help and --log-level
 ** - default logging facility
 ** - signal handling on a separate thread
 ** - binary RCS identity tracking, usually generated by xtd compile-time
 **   dependency tracking system
 **
 ** @section sec_howto How to use
 ** -# Inherit your class from Application
 ** -# Define the mandatory method @ref Application::process
 ** -# Declare your program arguments in your constructor
 ** -# Optionaly parse your configuration, check your parameters and initialize
 **    your application in the dedicated methods
 ** -# Instantiate your class in main(int,char**) function and all Application::execute method
 **
 ** Typical example :
 ** @include Application.hh
 **
 ** @section sec_execution_flow Execution flow
 ** @startuml{Application.png}
 ** (*) --> initialize default logging (level::crit)
 **     --> read arguments from argc & argv
 **     --> parseConfig()
 **         note right: User may redefine this function
 **     --> checkOptions()
 **     --> initialize logging with parsed level
 **         note right: User may redefine this function
 **     --> initialize()
 **         note right: User may redefine this function
 **     --> ===B1===
 **         --> start signal thread
 **         --> stop & join signal thread
 **         --> ===B2===
 **    ===B1=== --> process()
 **         note left: User <b>must</b> re-define this function
 **         --> ===B2===
 **     --> (*)
 ** @enduml
 **
 ** @section sec_binding_options Bindind options
 ** - @ref bindDir          : @copybrief bindDir
 ** - @ref bindFile         : @copybrief bindFile
 ** - @ref bindGiven        : @copybrief bindGiven
 ** - @ref bindString       : @copybrief bindString
 ** - @ref bindCallback     : @copybrief bindCallback
 ** - @ref bindValueIfGiven : @copybrief bindValueIfGiven
 ** - @ref bindNumber       : @copybrief bindNumber
 ** - @ref bindValues       : @copybrief bindValues
 ** - @ref bindAccumulator  : @copybrief bindAccumulator
 */
class Application
{
  friend class ::TestApplication;

protected:
  /**
   ** @brief Signal handler functor prototype, @see addSignalHandler
   */
  typedef std::function<void(void)> t_sig_handler;


  /**
   ** @brief Behavior switch for the argument of an option
   */
  enum class argument : uint32_t
  {
    mandatory = 1, ///< mandatory argument for option
    none      = 2  ///< forbidden argument for option
  };


  /**
   ** @brief Behavior for the presence of the option itself
   */
  enum class requirement : uint32_t
  {
    optional  = 0, ///< option is optional on command line
    mandatory = 1  ///< option must be given on command line
  };



private:
  struct t_option;
  typedef std::function<void(const string&, const t_option&)> t_callback;
  struct t_option
  {
    char        m_shortOpt;     ///< short form
    string      m_longOpt;      ///< long form
    string      m_description;  ///< usage description
    argument    m_argumentType; ///< need argument ?
    t_callback  m_callback;     ///< associated callback functor
    requirement m_status;       ///< is option required ?
    bool        m_given;        ///< is option encountered ?
  };
  typedef vector<t_option> t_option_list;

public:
  /**
   ** @brief Constructor
   ** @param p_disableExit If true, throws exception instead of exiting on error
   ** @throw std::runtime_error
   */
  Application(bool p_disableExit = false);

  /**
   ** @brief Destructor
   */
  virtual ~Application(void);

public:
  /**
   ** @brief main entry point, usually called with main(int,char**)'s arguments
   ** @param p_argc argument count
   ** @param p_argv argument list (first is binary name)
   ** @return depends on @ref Application::process implementation, usually 0 if process succeed
   */
  int execute(int p_argc, const char* const p_argv[]);

protected:
  ///////////////
  // interface //
  ///////////////
  /**
   ** @brief Parse application configuration, (default nothing)
   */
  virtual void parseConfig(void)  {};

  /**
   ** @brief Check read options, (default nothing)
   */
  virtual void checkOptions(void) {};

  /**
   ** @brief initialize application, (default nothing)
   */
  virtual void initialize(void)   {};

  /**
   ** @brief Main application process function, (default nothing)
   ** @return Up to the user, generally 0 in case of success
   */
  virtual int  process(void);

protected:
  //////////////////
  // misc methods //
  //////////////////
  /**
   ** @brief register a signal callback
   ** @param p_signalNumber signal ID (man signal 7)
   ** @param p_handler handling callback
   ** @return True if signal registered correctly, false if signal was already registered
   */
  bool addSignalHandler(int p_signalNumber, t_sig_handler p_handler);

  /**
   ** @brief Get RCSID identity informations
   */
  const string& getVersion(void) const;

  /////////////////////
  // option handling //
  /////////////////////


  /**
   ** @brief Register an option to parser
   ** @param p_shortOpt short form, single character
   ** @param p_longOpt long form, not starting with dash
   ** @param p_argType shall we expect an argument
   ** @param p_status  should the option be given by user
   ** @param p_description usage help description
   ** @param p_callback functor to call when option is encountered
   ** @details
   ** Option must :
   **  - a unique short form
   **  - a unique long form
   **  - short and long forms must not start by a dash (-)
   **
   ** Programs logs and exit in case of error.
   */
  void addOption(const char        p_shortOpt,
                 const string&     p_longOpt,
                 const argument    p_argType,
                 const requirement p_status,
                 const string&     p_description,
                 t_callback        p_callback);

  /**
   ** @brief Tells if given option was given on command line
   ** @param p_optionName long-form or short-form of the option
   ** @return true if option was given
   */
  bool isOptionGiven(const string& p_optionName) const;

  /**
   ** @brief Add additional usage line to display on help or errors
   ** @param p_helpMessage Message line to add
   */
  void addHelpMsg(const string& p_helpMessage);

  ////////////////////////
  // bindings callbacks //
  ////////////////////////
  /**
   ** @brief Bind option's parameter to a directory
   ** @param p_target Reference variable to store the option's value
   ** @return generated option callback
   */
  t_callback bindDir(string& p_target) const;

  /**
   ** @brief Bind option's parameter to a file name
   ** @param p_target Reference variable to store the option's value
   ** @param p_readable If true, check that given file name exists and is readable
   ** @return generated option callback
   */
  t_callback bindFile(string& p_target, bool p_readable = true) const;

  /**
   ** @brief Set targeted variable to true if option is given on command line
   ** @param p_target Reference variable
   ** @return generated option callback
   */
  t_callback bindGiven(bool& p_target) const;

  /**
   ** @brief Set targeted variable to option's parameter
   ** @param p_target Reference variable
   ** @return generated option callback
   */
  t_callback bindString(string& p_target) const;

  /**
   ** @brief Set targeted variable to option's parameter regex
   ** @param p_target Reference variable
   ** @return generated option callback
   */
  t_callback bindRegex(string& p_target) const;

  /**
   ** @brief Associate option to given generic callback
   ** @param p_action function compatible with @code std::function<void(void)> @endcode signature
   ** @return generated option callback
   */
  template<typename T>
  t_callback bindCallback(T p_action) const;

  /**
   ** @brief Set given value to referenced variable if option is given
   ** @param p_target Reference variable
   ** @param p_value Value to set to p_target
   ** @return generated option callback
   */
  template<typename T>
  t_callback bindValueIfGiven(T& p_target, const T& p_value) const;

  /**
   ** @brief Bind option's paramter as number to referenced variable
   ** @param p_target Reference variable
   ** @param p_min Minimum parameter value [inclusive]
   ** @param p_max Maximum parameter value [inclusive]
   ** @return generated option callback
   ** @details
   ** Will led to an error if input parameter :
   ** - not lexically convertible to T
   ** - is lower than p_min
   ** - is greater than p_max
   */
  template<typename T>
  t_callback bindNumber(T& p_target,
                        T  p_min = std::numeric_limits<T>::min(),
                        T  p_max = std::numeric_limits<T>::max()) const;


  /**
   ** @brief Bind option's parameter to target variable and checks among authorized values
   ** @param p_target Reference variable
   ** @param p_values Iterable container of authorized option's values
   ** @return generated option callback
   */
  template<typename T, class Iterable>
  t_callback bindValues(T& p_target, const Iterable& p_values) const;


  /**
   ** @brief Append option's parameter to target container for each command line hit
   ** @param p_target Reference variable container
   ** @return generated option callback
   */
  template<class TCollection>
  t_callback bindAccumulator(TCollection& p_target) const;


  ////////////////////
  // error handling //
  ////////////////////
  /**
   ** @brief Prints error on standard error and exit
   ** @param p_code exit code
   ** @param p_format error message format (boost::format compatible)
   ** @param p_args Template variadic arguments to format
   */
  template<typename... Arguments>
  void error_nohelp(int p_code, const string& p_format, Arguments&&... p_args) const;

  /**
   ** @brief Prints error and usage on standard error and exit
   ** @param p_code exit code
   ** @param p_format error message format (boost::format compatible)
   ** @param p_args Template variadic arguments to format
   */
  template<typename... Arguments>
  void error(int p_code, const string& p_format, Arguments&&... p_args) const;

  /**
   ** @brief Prints a warning on standard error
   ** @param p_format error message format (boost::format compatible)
   ** @param p_args Template variadic arguments to format
   */
  template<typename... Arguments>
  void warn(const string& p_format, Arguments&&... p_args) const;


private:
  void readArgs(int p_argc, const char* const p_argv[]);
  void usage(std::ostream& p_stream = std::cerr) const;
  void handleSignal(const boost::system::error_code& p_error, int p_signalNumber);

protected:
  string         m_binName;       ///< binary name (argv[0])
  uint32_t       m_logLevel;      ///< log level read from command line
  vector<string> m_remainingArgs; ///< positional command line arguments
  string         m_rcsid;         ///< binary identity information

private:
  t_option_list                 m_optionList;
  string                        m_helpText;
  boost::thread                 m_runThread;
  boost::asio::io_service       m_ioService;
  boost::asio::io_service::work m_work;
  boost::asio::signal_set       m_signals;
  map<int, t_sig_handler>       m_signalHandlerMap;
  bool                          m_disableExit;
};

}

# include "Application.hxx"

#endif // !COMMON_APPLICATION_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
