#ifndef COMMON_APPLICATION_HH_
# define COMMON_APPLICATION_HH_

# include <iostream>
# include <iomanip>
# include <limits>
# include <map>
# include <getopt.h>
# include <libgen.h>
# include <stdlib.h>

# include <boost/system/error_code.hpp>
# include <boost/thread.hpp>
# include <boost/asio.hpp>
# include <boost/asio/signal_set.hpp>
# include <boost/function.hpp>
# include <boost/bind.hpp>
# include "types.hh"
# include "logger.hh"

namespace xtd {

class Application
{
protected:
  typedef boost::function<void()>      t_action;
  typedef boost::function<void (void)> t_sig_handler;

  enum class argument    : uint32_t { optional = 0, mandatory  = 1, none = 2 };
  enum class requirement : uint32_t { optional = 0, mandatory  = 1           };



private:
  struct t_option;
  typedef boost::function<void(const string&, const t_option&)> t_callback;
  struct t_option
  {
    char        m_shortOpt;
    string      m_longOpt;
    string      m_description;
    argument    m_argumentType;
    t_callback  m_callback;
    requirement m_status;
    bool        m_given;
  };

  typedef vector<t_option> t_option_list;

public:
  Application(void);
  virtual ~Application(void) { };

public:
  int execute(int argc, char** argv);

protected:
  virtual void parseConfig(void)  {};
  virtual void checkOptions(void) {};
  virtual void initialize(void)   {};
  virtual int  process(void);

protected:
  const string& getVersion(void) const;

  void addOption(const char        p_shortOpt,
                 const string&     p_longOpt,
                 const argument    p_argType,
                 const requirement p_status,
                 const string&     p_description,
                 t_callback        p_callback);


  void addHelpMsg(const string& p_helpMessage);

  bool addSignalHandler(int p_signalNumber, t_sig_handler p_handler);

  /**
   ** @brief Indique si l'option p_optionName a ete donne a la ligne de commande
   ** @param p_optionName nom de l'option, court ou long (sans les '-' ou '--')
   */
  bool isOptionGiven(const string& p_optionName) const;

  t_callback bindDir(string& p_target,  bool p_readable = true);
  t_callback bindFile(string& p_target, bool p_readable = true);
  t_callback bindGiven(bool& p_target);
  t_callback bindString(string& p_target);

  template<typename T>
  t_callback bindAction(T p_action);

  template<typename T>
  t_callback bindMethod(T p_method);

  template<typename T>
  t_callback bindValueIfGiven(T& p_target, const T& p_value);

  template<typename T>
  t_callback bindNumber(T& p_target,
                        T  p_min = std::numeric_limits<T>::min(),
                        T  p_max = std::numeric_limits<T>::max());

  template<typename T, typename Iterable>
  t_callback bindValues(T& p_target, const Iterable& p_values);

  template<typename TCollection>
  t_callback bindAccumulator(TCollection& p_target);

  template<typename... Arguments>
  void error_nohelp(int p_code, const string& p_format, Arguments&&... p_args) const;

  template<typename... Arguments>
  void error(int p_code, const string& p_format, Arguments&&... p_args) const;

  template<typename... Arguments>
  void warn(const string& p_format, Arguments&&... p_args) const;


private:
  void readArgs(int p_argc, char** p_argv);
  void usageWrapper(void) const;
  void usage(void)        const;
  void handleSignal(const boost::system::error_code& p_error, int p_signalNumber);

  void h_file(const string&   p_path,
              const t_option& p_opt,
              string&         p_target,
              bool            p_readable);

  void h_dir(const string&   p_path,
             const t_option& p_opt,
             string&         p_target,
             bool            p_readable);

  void h_string(const string&   p_path,
                const t_option& p_opt,
                string&         p_target);

  void h_given(const string&   /* p_path */,
               const t_option& /* p_opt  */,
               bool&              p_target);

  template<typename T>
  void h_value_if_given(const string&   /* p_path */,
                        const t_option& /* p_opt  */,
                        T&                 p_target,
                        const T&           p_value);

  template<typename T>
  void h_number(const string&   p_value,
                const t_option& p_opt,
                T&              p_target,
                T               p_min = std::numeric_limits<T>::min(),
                T               p_max = std::numeric_limits<T>::max());


  template<typename T, typename Iterable>
  void h_values(const string&   p_value,
                const t_option& p_opt,
                T&              p_target,
                const Iterable& p_values);

  template<typename TCollection>
  void h_accumulator(const string&   p_value,
                     const t_option& p_opt,
                     TCollection&    p_dest);

  void h_action(const string&   p_value,
                const t_option& p_opt,
                t_action        p_method);

protected:
  string         m_binName;
  uint32_t       m_logLevel;
  vector<string> m_remainingArgs;
  string         m_rcsid;

private:
  t_option_list                 m_optionList;
  string                        m_helpText;
  boost::thread                 m_runThread;
  boost::asio::io_service       m_ioService;
  boost::asio::io_service::work m_work;
  boost::asio::signal_set       m_signals;
  map<int, t_sig_handler>       m_signalHandlerMap;
};

}

# include "Application.hxx"

#endif // !COMMON_APPLICATION_HH_
