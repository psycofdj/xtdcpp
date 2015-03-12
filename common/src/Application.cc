#include "Application.hh"
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <sstream>
#include <utility>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

extern char rcsid[];

using namespace boost;
using namespace boost::assign;

namespace xtd {

Application::Application(void) :
  m_binName(),
  m_logLevel(logger::get().valueOf(logger::level::crit)),
  m_remainingArgs(),
  m_rcsid(rcsid),
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
            bindAction(boost::bind(&Application::usageWrapper, this)));

  addOption('e', "log-level",
            argument::mandatory,
            requirement::optional,
            "change le niveau de log à <arg> (defaut 2)",
            bindValues(m_logLevel, list_of(0u)(1u)(2u)(3u)(4u)(5u)(6u)(7u)));

  m_signals.async_wait(boost::bind(&Application::handleSignal, this, _1, _2));
}

const string&
Application::getVersion(void) const
{
  return m_rcsid;
}

Application::t_callback
Application::bindFile(string& p_target, bool p_readable)
{
  return boost::bind(&Application::h_file, this, _1, _2, boost::ref(p_target), p_readable);
}

Application::t_callback
Application::bindDir(string& p_target, bool p_readable)
{
  return boost::bind(&Application::h_dir, this, _1, _2, boost::ref(p_target), p_readable);
}

Application::t_callback
Application::bindString(string& p_target)
{
  return boost::bind(&Application::h_string, this, _1, _2, boost::ref(p_target));
}


Application::t_callback
Application::bindGiven(bool& p_target)
{
  p_target = false;
  return boost::bind(&Application::h_given, this, _1, _2, boost::ref(p_target));
}

void
Application::h_file(const string&     p_path,
                    const t_option& p_opt,
                    string&         p_target,
                    bool            p_readable)
{
  p_target = p_path;
  if (p_readable && (false == boost::filesystem::exists(p_path)))
    error(1, "invalid option -%c=%s, must be readable file", p_opt.m_shortOpt, p_path.c_str());
}


void
Application::h_dir(const string&   p_path,
                   const t_option& p_opt,
                   string&         p_target,
                   bool            p_readable)
{
  p_target = p_path;

  if (false == boost::filesystem::is_directory(p_path))
    error(1, "invalid option -%c=%s, must be a directory", p_opt.m_shortOpt, p_path);
  if (p_readable && (false == boost::filesystem::exists(p_path)))
    error(1, "invalid option -%c=%s, must be readable", p_opt.m_shortOpt, p_path);
}


void
Application::h_string(const string&      p_value,
                      const t_option& /* p_opt  */,
                      string&            p_target)
{
  p_target = p_value;
}

void
Application::h_given(const string&    /* p_path */,
                     const t_option&  /* p_opt  */,
                     bool&               p_target)
{
  p_target = true;
}

int
Application::execute(int p_argc, char** p_argv)
{
  int l_status = 0;

  try
  {
    m_binName = basename(p_argv[0]);

    logger::get().initialize(string(m_binName), logger::level::crit);
    readArgs(p_argc, p_argv);
    parseConfig();
    checkOptions();
    logger::get().setAllLevels(logger::get().levelOf(m_logLevel));
    initialize();

    m_runThread = thread(bind(&asio::io_service::run, &m_ioService));

    l_status = process();

    m_ioService.stop();
    m_runThread.join();
  }
  catch (std::exception& l_error) {
    error_nohelp(1, "caught exception : %s", l_error.what());
  }

  return l_status;
}

void
Application::handleSignal(const boost::system::error_code& /* p_error */,
                          int                                 p_signalNumber)
{
  int l_signal;
  if (p_signalNumber == 0)
    l_signal = 15;
  else
    l_signal = p_signalNumber;

  auto l_it = m_signalHandlerMap.find(l_signal);
  if (l_it == m_signalHandlerMap.end())
  {
    logger::crit("common.application", "No signal corresponding to %i found", p_signalNumber, HERE);
    m_signals.async_wait(bind(&Application::handleSignal, this, _1, _2));
    return;
  }

  (*l_it).second();
  m_signals.async_wait(bind(&Application::handleSignal, this, _1, _2));
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
  t_option                l_opt;

  auto l_checker =
    [this, p_shortOpt, &p_longOpt](const t_option_list::value_type& c_optItem) {
    if ((c_optItem.m_shortOpt == p_shortOpt)                       ||
        ((c_optItem.m_longOpt == p_longOpt) && (p_longOpt != "")))
    {
      error(1, "short option '%c' already exists", p_shortOpt);
    }
  };

  std::for_each(m_optionList.begin(), m_optionList.end(), l_checker);

  l_opt.m_given        = false;
  l_opt.m_shortOpt     = p_shortOpt;
  l_opt.m_longOpt      = p_longOpt;
  l_opt.m_argumentType = p_argType;
  l_opt.m_status       = p_status;
  l_opt.m_description  = p_description;
  l_opt.m_callback     = p_callback;
  m_optionList.push_back(l_opt);
}

void
Application::readArgs(int p_argc, char** p_argv)
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
      case argument::optional:  l_options[c_optIdx].has_arg = 2; break;
      }
    }
    l_shortOptString += m_optionList[c_optIdx].m_shortOpt;
    switch (m_optionList[c_optIdx].m_argumentType)
    {
    case argument::none:                               break;
    case argument::mandatory: l_shortOptString += ":"; break;
    case argument::optional:  l_shortOptString += ";"; break;
    }
  }

  while (1)
  {
    l_option_name = getopt_long(p_argc, p_argv, l_shortOptString.c_str(), l_options, &l_option_index);
    if (l_option_name == -1)
      break;

    if (l_option_name == '?')
      std::exit(1);

    for (c_opt = m_optionList.begin(); c_opt != m_optionList.end(); c_opt++)
    {
      if (c_opt->m_shortOpt == l_option_name)
      {
        c_opt->m_given = true;
        string l_argValue;
        if (optarg)
          l_argValue = optarg;
        c_opt->m_callback(l_argValue, *c_opt);
        break;
      }
    }
  }

  for (c_opt = m_optionList.begin(); c_opt != m_optionList.end(); c_opt++)
  {
    switch (c_opt->m_status)
    {
    case requirement::optional: break;
    case requirement::mandatory:
      if (c_opt->m_given == false)
        error(1, "option '%c' is mandatory", c_opt->m_shortOpt);
      break;
    }
  }

  while (optind < p_argc)
  {
    m_remainingArgs.push_back(string(p_argv[optind]));
    optind++;
  }
}

void
Application::usageWrapper(void) const
{
  usage();
}


bool
Application::isOptionGiven(const string& p_optionName) const
{
  for (auto cc_opt = m_optionList.begin();
       cc_opt != m_optionList.end();
       cc_opt++)
  {
    string l_shortOpt;

    l_shortOpt += cc_opt->m_shortOpt;

    if ((cc_opt->m_given    == true) &&
        ((l_shortOpt        == p_optionName) ||
         (cc_opt->m_longOpt == p_optionName)))
    {
      return true;
    }
  }
  return false;
}


void
Application::usage(void) const
{
  t_option_list::const_iterator cc_optItem;

  std::cerr << "usage : " << m_binName << " [options]" << endl
            << endl;

  size_t                             l_maxOptStrSize = 0;
  vector<std::pair<string, string> > l_data;

  for (cc_optItem = m_optionList.begin();
       cc_optItem != m_optionList.end();
       cc_optItem++)
  {
    string       l_shortForm = "-";
    string       l_longForm  = "";
    stringstream l_format;

    l_shortForm += cc_optItem->m_shortOpt;
    if (cc_optItem->m_longOpt.size())
    {
      l_longForm += "--";
      l_longForm += cc_optItem->m_longOpt;

      switch (cc_optItem->m_argumentType)
      {
      case argument::none:                                 break;
      case argument::optional:  l_longForm += " [<arg>]"; break;
      case argument::mandatory: l_longForm += " <arg>";    break;
      }
    }

    l_format << std::right << std::setw(5) << l_shortForm << " | " << l_longForm;
    l_maxOptStrSize = std::max(l_maxOptStrSize, l_format.str().size());
    l_data.push_back(std::make_pair(l_format.str(), cc_optItem->m_description));
  }

  for (size_t c_idx = 0; c_idx < l_data.size(); c_idx++)
  {
    std::cerr << std::left << std::setw(l_maxOptStrSize) << l_data[c_idx].first
              << " : " << l_data[c_idx].second << endl;
  }

  if (m_helpText.size() != 0)
  {
    std::cerr << endl
              << endl
              << m_helpText
              << endl;
  }

  std::exit(1);
}

void
Application::addHelpMsg(const string& p_helpMessage)
{
  if (m_helpText.size())
    m_helpText += "\n";
  m_helpText += "  " + p_helpMessage;
}


void
Application::h_action(const string&    /* p_value */,
                      const t_option&  /* p_opt */,
                      t_action            p_action)
{
  p_action();
}

}
