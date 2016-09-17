#include "Application.hh"
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <sstream>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <unistd.h>

#ifdef HAVE_DEPENDENCY_TRACKING
///! RSC keyword
extern char rcsid[];
#else
///! RSC keyword
const static char rcsid[] = "$rscid: include 'xtdmake/tracking/module.cmake' to enable binary tracking system $";
#endif

namespace xtd {

Application::Application(bool p_disableExit, bool p_disableCatch) :
  m_binName(),
  m_logLevel(logger::get().valueOf(logger::level::crit)),
  m_remainingArgs(),
  m_rcsid(rcsid),
  m_disableExit(p_disableExit),
  m_disableCatch(p_disableCatch),
  m_optionList(),
  m_helpText(),
  m_runThread(),
  m_work(m_ioService),
  m_signals(m_ioService),
  m_signalHandlerMap()
{
  addOption('h', "help",
            argument::none,
            requirement::optional,
            "imprime ce message",
            bindCallback(std::bind(&Application::usage, this, std::ref(std::cerr))));

  addOption('e', "log-level",
            argument::mandatory,
            requirement::optional,
            R"(change le niveau de log à <arg> (defaut 2).
               0 -> emergency
               1 -> alert
               2 -> critical
               3 -> error
               4 -> warning
               5 -> notice
               6 -> info
               7 -> debug)",
            bindValues(m_logLevel, vector<uint32_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u})));

  m_signals.async_wait(std::bind(&Application::handleSignal, this, std::placeholders::_1, std::placeholders::_2));
}

Application::~Application(void)
{
}

const string&
Application::getVersion(void) const
{
  return m_rcsid;
}


Application::t_callback
Application::bindFile(string& p_target, bool p_readable) const
{
  return [&p_target, p_readable, this](const string& p_value, const t_option& p_opt) {
    p_target = p_value;
    if ((p_readable && (false == boost::filesystem::exists(p_value))) ||
        (p_readable && (true  == boost::filesystem::is_directory(p_value))))
      error(1, "invalid option -%c=%s, must be readable file", p_opt.m_shortOpt, p_value);
  };
}

Application::t_callback
Application::bindDir(string& p_target) const
{
  return [&p_target, this](const string& p_value, const t_option& p_opt) {
    p_target = p_value;
    if (false == boost::filesystem::is_directory(p_value))
      error(1, "invalid option -%c=%s, must be a directory", p_opt.m_shortOpt, p_value);
  };

}

Application::t_callback
Application::bindRegex(string& p_target) const
{
  return [&p_target, this](const string& p_value, const t_option& p_opt) {
    try {
      boost::regex l_regex(p_value);
    } catch (boost::bad_expression&) {
      error(1, "invalid option --%s=%s, must be a valid regex", p_value, p_opt.m_longOpt);
    }
    p_target = p_value;
  };

}


Application::t_callback
Application::bindString(string& p_target) const
{
  return [&p_target](const string& p_value, const t_option&) {
    p_target = p_value;
  };

}


Application::t_callback
Application::bindGiven(bool& p_target) const
{
  p_target = false;
  return [&p_target](const string&, const t_option&) {
    p_target = true;
  };
}


int
Application::execute(int p_argc, const char* const p_argv[])
{
  int l_status = 0;
  char* l_name = 0;

  l_name = strdup(p_argv[0]);
  m_binName = basename(l_name);
  free(l_name);

  std::function<void(std::function<void(void)>)> l_noProtect = [&](std::function<void(void)> p_handler) {
    p_handler();
  };

  std::function<void(std::function<void(void)>)> l_doProtect = [&](std::function<void(void)> p_handler) {
    try {
      p_handler();
    }
    catch (std::exception& l_error) {
      error_nohelp(1, "caught exception : %s", l_error.what());
    }
  };

  auto l_init = [&](void) {
    logger::get().initialize(string(m_binName), logger::level::crit);
    readArgs(p_argc, p_argv);
    parseConfig();
    checkOptions();
    logger::get().setAllLevels(logger::get().levelOf(m_logLevel));
    initialize();
  };

  auto l_process = [&](void) {
    m_runThread = boost::thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
    l_status = process();
    m_ioService.stop();
    m_runThread.join();
  };

  auto l_getProtector = [&](void) {
    if (true == m_disableCatch)
      return l_noProtect;
    return l_doProtect;
  };

  l_getProtector()(l_init);
  l_getProtector()(l_process);

  return l_status;
}


/**
 ** @details
 ** 1. Mysteriously appeared here. Commented until any bug pops back.
 */
void
Application::handleSignal(const boost::system::error_code& /* p_error */,
                          int                                 p_signalNumber)
{
  int l_signal = p_signalNumber;

  // 1.
  // if (p_signalNumber == 0)
  //   l_signal = 15;

  auto l_it = m_signalHandlerMap.find(l_signal);
  if (l_it == m_signalHandlerMap.end())
  {
    logger::crit("common.application", "No signal corresponding to %i found", p_signalNumber, HERE);
    m_signals.async_wait(std::bind(&Application::handleSignal, this, std::placeholders::_1, std::placeholders::_2));
    return;
  }

  (*l_it).second();
  m_signals.async_wait(std::bind(&Application::handleSignal, this, std::placeholders::_1, std::placeholders::_2));
}

bool
Application::addSignalHandler(int p_signalNumber, t_sig_handler p_handler)
{
  if (m_signalHandlerMap.find(p_signalNumber) != m_signalHandlerMap.end())
    return false;

  m_signals.add(p_signalNumber);
  m_signalHandlerMap[p_signalNumber] = p_handler;
  return true;
}

int
Application::process(void)
{
  return 0;
}

void
Application::addOption(const char        p_shortOpt,
                       const string&     p_longOpt,
                       const argument    p_argType,
                       const requirement p_status,
                       const string&     p_description,
                       t_callback        p_callback)
{
  t_option l_opt;

  auto l_checker =
    [this, p_shortOpt, &p_longOpt](const t_option_list::value_type& c_optItem) {
    if (c_optItem.m_shortOpt == p_shortOpt)
      error(1, "short option '%c' already exists", p_shortOpt);
    if ((c_optItem.m_longOpt == p_longOpt) && (p_longOpt != ""))
      error(1, "long option '%s' already exists", p_longOpt);
  };

  std::for_each(m_optionList.begin(), m_optionList.end(), l_checker);

  if (0 == p_longOpt.find_first_of("-") || (p_shortOpt == '-'))
    error(1, "invalid option '%s', leading dash is forbidden", p_longOpt);

  l_opt.m_given        = false;
  l_opt.m_shortOpt     = p_shortOpt;
  l_opt.m_longOpt      = p_longOpt;
  l_opt.m_argumentType = p_argType;
  l_opt.m_status       = p_status;
  l_opt.m_description  = p_description;
  l_opt.m_callback     = p_callback;
  m_optionList.push_back(l_opt);
}

/**
 ** @details
 ** 1. Reset getopt_long internal global variable.
 */
void
Application::readArgs(int p_argc, const char* const p_argv[])
{
  char                    l_option_name = 0;
  int                     l_option_index = 0;
  string                  l_shortOptString;
  struct option           l_options[100];
  size_t                  c_optIdx = 0;
  t_option_list::iterator c_opt;

  for (c_optIdx = 0; c_optIdx < m_optionList.size(); c_optIdx++)
  {
    if (m_optionList[c_optIdx].m_longOpt != "")
    {
      l_options[c_optIdx].name = m_optionList[c_optIdx].m_longOpt.c_str();
      l_options[c_optIdx].flag = 0;
      l_options[c_optIdx].val = m_optionList[c_optIdx].m_shortOpt;

      switch (m_optionList[c_optIdx].m_argumentType)
      {
      case argument::none:      l_options[c_optIdx].has_arg = 0; break;
      case argument::mandatory: l_options[c_optIdx].has_arg = 1; break;
      }
    }
    l_shortOptString += m_optionList[c_optIdx].m_shortOpt;
    switch (m_optionList[c_optIdx].m_argumentType)
    {
    case argument::none:                               break;
    case argument::mandatory: l_shortOptString += ":"; break;
    }
  }


  l_options[m_optionList.size()].name    = NULL;
  l_options[m_optionList.size()].flag    = 0;
  l_options[m_optionList.size()].val     = 0;
  l_options[m_optionList.size()].has_arg = 0;

  // 1.
  optind = 1;
  optopt = 0;
  opterr = 0;

  while (1)
  {
    l_option_name = getopt_long(p_argc, const_cast<char**>(p_argv), l_shortOptString.c_str(), l_options, &l_option_index);

    if (l_option_name == -1)
      break;

    if (l_option_name == '?')
      error(1, "invalid option -%c", static_cast<char>(optopt));

    for (auto& c_opt : m_optionList)
    {
      if (c_opt.m_shortOpt == l_option_name)
      {
        c_opt.m_given = true;
        string l_argValue;
        if (optarg) {
          l_argValue = optarg;
        }
        c_opt.m_callback(l_argValue, c_opt);
        break;
      }
    }
  }

  for (auto& c_opt : m_optionList)
  {
    switch (c_opt.m_status)
    {
    case requirement::optional: break;
    case requirement::mandatory:
      if (c_opt.m_given == false)
        error(1, "option '%c' is mandatory", c_opt.m_shortOpt);
      break;
    }
  }

  while (optind < p_argc)
  {
    m_remainingArgs.push_back(string(p_argv[optind]));
    optind++;
  }
}


bool
Application::isOptionGiven(const string& p_optionName) const
{
  for (const auto& cc_opt : m_optionList)
  {
    string l_shortOpt;

    l_shortOpt += cc_opt.m_shortOpt;

    if ((cc_opt.m_given    == true) &&
        ((l_shortOpt        == p_optionName) ||
         (cc_opt.m_longOpt == p_optionName)))
    {
      return true;
    }
  }
  return false;
}


void
Application::usage(std::ostream& p_stream) const
{
  size_t                                    l_maxOptStrSize = 0;
  vector<std::pair<string, vector<string>>> l_data;

  p_stream << "usage : " << m_binName << " [options]" << endl
           << endl;

  for (const auto& cc_optItem : m_optionList)
  {
    string         l_shortForm = "-";
    string         l_longForm  = "";
    stringstream   l_format;
    vector<string> l_lines;

    l_shortForm += cc_optItem.m_shortOpt;
    if (cc_optItem.m_longOpt.size())
    {
      l_longForm += "--";
      l_longForm += cc_optItem.m_longOpt;

      switch (cc_optItem.m_argumentType)
      {
      case argument::none:                                 break;
      case argument::mandatory: l_longForm += " <arg>";    break;
      }
    }

    l_format << std::right << std::setw(5) << l_shortForm << " | " << l_longForm;
    l_maxOptStrSize = std::max(l_maxOptStrSize, l_format.str().size());

    boost::split(l_lines, cc_optItem.m_description, boost::is_any_of("\n"));
    l_data.push_back(std::make_pair(l_format.str(), l_lines));
  }

  for (size_t c_idx = 0; c_idx < l_data.size(); c_idx++)
  {
    p_stream << std::left << std::setw(l_maxOptStrSize) << l_data[c_idx].first
             << " : " << l_data[c_idx].second[0] << endl;
    for (size_t c_lineIdx = 1; c_lineIdx < l_data[c_idx].second.size(); c_lineIdx++)
      p_stream << std::setw(l_maxOptStrSize + 3) << " " << boost::trim_copy(l_data[c_idx].second[c_lineIdx]) << std::endl;
  }

  if (m_helpText.size() != 0)
  {
    p_stream << endl
             << endl
             << m_helpText
             << endl;
  }


  // LCOV_EXCL_START
  if (false == m_disableExit)
    std::exit(1);
  // LCOV_EXCL_STOP
}

void
Application::addHelpMsg(const string& p_helpMessage)
{
  if (m_helpText.size())
    m_helpText += "\n";
  m_helpText += "  " + p_helpMessage;
}


}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
