#include "log.hh"                 //libcore
#include "Application.hh"         //libcore
#include <MainTestApplication.hh> //libtests
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <stdexcept>

using namespace xtd;


class TestApplication : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestApplication);
  CPPUNIT_TEST(addOption);
  CPPUNIT_TEST(readArgs);
  CPPUNIT_TEST(isOptionGiven);
  CPPUNIT_TEST(usage);
  CPPUNIT_TEST(handleSignal);
  CPPUNIT_TEST(bindFile);
  CPPUNIT_TEST(bindDir);
  CPPUNIT_TEST(bindString);
  CPPUNIT_TEST(bindRegex);
  CPPUNIT_TEST(bindValues);
  CPPUNIT_TEST(bindCallback);
  CPPUNIT_TEST(bindNumber);
  CPPUNIT_TEST(bindValueIfGiven);
  CPPUNIT_TEST(bindAccumulator);
  CPPUNIT_TEST(getVersion);
  CPPUNIT_TEST(warn);
  CPPUNIT_TEST_SUITE_END();

public:
  void addOption(void);
  void readArgs(void);
  void isOptionGiven(void);
  void usage(void);
  void handleSignal(void);
  void bindFile(void);
  void bindDir(void);
  void bindString(void);
  void bindRegex(void);
  void getVersion(void);
  void bindValues(void);
  void bindNumber(void);
  void bindCallback(void);
  void bindValueIfGiven(void);
  void bindAccumulator(void);
  void warn(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestApplication);


struct MyApp : public Application
{
  friend class ::TestApplication;

  MyApp(bool p_status = false) :
    Application(p_status),
    m_started(true)
  { }

  int process(void)
  {
    while (true == m_started) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
    return Application::process();
  }

  void stop(void) { m_started = false; }

  bool m_started;
};


void
TestApplication::warn(void)
{
  Application l_app(false);
  CPPUNIT_ASSERT_NO_THROW(l_app.warn("warning message with printf format %d", 5));
}

/**
 ** @details
 ** 1. wait for application starting
 */
void
TestApplication::handleSignal()
{
  MyApp             l_app(true);
  boost::thread     l_thread;
  const char* const l_args[] = { "binary" };
  bool              l_usr1Called = false;
  bool              l_usr2Called = false;

  CPPUNIT_ASSERT_EQUAL(true, l_app.addSignalHandler(SIGUSR1, [&l_usr1Called](void) {
        l_usr1Called = true;
      }));

  CPPUNIT_ASSERT_EQUAL(true, l_app.addSignalHandler(SIGUSR2, [&l_usr2Called](void) {
        l_usr2Called = true;
      }));

  // signal already registered
  CPPUNIT_ASSERT_EQUAL(false, l_app.addSignalHandler(SIGUSR2, [&l_usr2Called](void) {
        l_usr2Called = true;
      }));

  l_thread = boost::thread([&l_app, &l_args](void) {
      l_app.execute(1, l_args);
    });

  // 1.
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));
  CPPUNIT_ASSERT_EQUAL(false, l_usr1Called);
  CPPUNIT_ASSERT_EQUAL(false, l_usr2Called);

  kill(getpid(), SIGUSR1);
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));
  CPPUNIT_ASSERT_EQUAL(true, l_usr1Called);
  CPPUNIT_ASSERT_EQUAL(false, l_usr2Called);

  kill(getpid(), SIGUSR2);
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));
  CPPUNIT_ASSERT_EQUAL(true, l_usr1Called);
  CPPUNIT_ASSERT_EQUAL(true, l_usr2Called);

  l_usr2Called = false;
  l_app.m_signalHandlerMap.erase(SIGUSR2);
  kill(getpid(), SIGUSR2);
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));
  CPPUNIT_ASSERT_EQUAL(false, l_usr2Called);

  l_app.stop();
  l_thread.join();
}

void
TestApplication::getVersion(void)
{
  Application l_app(true);
  std::string l_version = l_app.getVersion();

#ifdef HAVE_DEPENDENCY_TRACKING
  CPPUNIT_ASSERT(string::npos != l_version.find("$date:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$time:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$name:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$user:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$host:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$pwd:"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$archive: [libxtd-core_s]"));
  CPPUNIT_ASSERT(string::npos != l_version.find("$archive: [libxtd-tests_s]"));
#endif
}

void
TestApplication::addOption(void)
{
  bool        l_called;

  {
    // KO : already defined option
    Application l_app(true);
    bool        l_called;

    CPPUNIT_ASSERT_THROW(l_app.addOption('e', "log-level", Application::argument::mandatory, Application::requirement::optional, "decr", l_app.bindGiven(l_called)), std::runtime_error);
    CPPUNIT_ASSERT_THROW(l_app.addOption('o', "log-level", Application::argument::mandatory, Application::requirement::optional, "decr", l_app.bindGiven(l_called)), std::runtime_error);
    CPPUNIT_ASSERT_THROW(l_app.addOption('e', "error",     Application::argument::mandatory, Application::requirement::optional, "decr", l_app.bindGiven(l_called)), std::runtime_error);
  }

  {
    // KO : mal-format option
    Application l_app(true);

    CPPUNIT_ASSERT_THROW(l_app.addOption('o', "--opt", Application::argument::mandatory, Application::requirement::optional, "decr", l_app.bindGiven(l_called)), std::runtime_error);
  }

  {
    // OK : initial state
    Application l_app(true);

    CPPUNIT_ASSERT_EQUAL(2ul,                      l_app.m_optionList.size());
    CPPUNIT_ASSERT_EQUAL('h',                      l_app.m_optionList[0].m_shortOpt);
    CPPUNIT_ASSERT_EQUAL(std::string("help"),      l_app.m_optionList[0].m_longOpt);
    CPPUNIT_ASSERT_EQUAL('e',                      l_app.m_optionList[1].m_shortOpt);
    CPPUNIT_ASSERT_EQUAL(std::string("log-level"), l_app.m_optionList[1].m_longOpt);

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt", Application::argument::mandatory, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_EQUAL(3ul, l_app.m_optionList.size());
  }
}


void
TestApplication::bindFile(void)
{
  {
    // OK bind file readable
    Application       l_app(true);
    std::string       l_file;
    const char* const l_args[] = { "binary", "--file", "/dev/random" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "file",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindFile(l_file, true)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("/dev/random"), l_file);
  }

  {
    // OK bind file unreadable
    Application       l_app(true);
    std::string       l_file;
    const char* const l_args[] = { "binary", "--file", "/dev/doesnotexist" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "file",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindFile(l_file, false)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("/dev/doesnotexist"), l_file);
  }

  {
    // KO bind file unreadable
    Application       l_app(true);
    std::string       l_file;
    const char* const l_args[] = { "binary", "--file", "/dev/doesnotexist" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "file",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindFile(l_file, true)));
    CPPUNIT_ASSERT_THROW(l_app.execute(3, l_args), std::runtime_error);
  }

  {
    // KO bind file not a file
    Application       l_app(true);
    std::string       l_file;
    const char* const l_args[] = { "binary", "--file", "/dev/" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "file",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindFile(l_file, true)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

}

void
TestApplication::bindDir(void)
{
  {
    // OK bind dir readable
    Application       l_app(true);
    std::string       l_dir;
    const char* const l_args[] = { "binary", "--dir", "/dev/" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "dir",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindDir(l_dir)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("/dev/"), l_dir);
  }

  {
    // KO bind dir unreadable
    Application       l_app(true);
    std::string       l_dir;
    const char* const l_args[] = { "binary", "--dir", "/doesnotexist" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "dir",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindDir(l_dir)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // KO bind dir not dir
    Application       l_app(true);
    std::string       l_dir;
    const char* const l_args[] = { "binary", "--dir", "/dev/random" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('f', "dir",   Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindDir(l_dir)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }
}

void
TestApplication::bindString(void)
{
  Application       l_app(true);
  std::string       l_string;
  const char* const l_args[] = { "binary", "--string", "toto" };

  CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string", Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindString(l_string)));


  CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
  CPPUNIT_ASSERT_EQUAL(std::string("toto"), l_string);
}


void
TestApplication::bindAccumulator(void)
{
  {
    // OK, options are stored
    Application       l_app(true);
    vector<uint32_t>  l_values;
    const char* const l_args[] = { "binary", "--string", "12", "--string", "21" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindAccumulator(l_values)));


    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(5, l_args));
    CPPUNIT_ASSERT_EQUAL(2ul, l_values.size());
    CPPUNIT_ASSERT_EQUAL(12u, l_values[0]);
    CPPUNIT_ASSERT_EQUAL(21u, l_values[1]);
  }

  {
    // KO, one of given parameters is not castable to dest
    Application       l_app(true);
    vector<uint32_t>  l_values;
    const char* const l_args[] = { "binary", "--string", "12", "--string", "titi" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindAccumulator(l_values)));


    CPPUNIT_ASSERT_THROW(l_app.readArgs(5, l_args), std::runtime_error);
  }
}


void
TestApplication::bindValueIfGiven(void)
{
  {
    Application       l_app(true);
    std::string       l_string = "toto";
    const char* const l_args[] = { "binary", "--opt" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::none,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindValueIfGiven(l_string, string("super"))));

    CPPUNIT_ASSERT_EQUAL(std::string("toto"), l_string);
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(2, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("super"), l_string);
  }

  {
    Application       l_app(true);
    std::string       l_string = "toto";
    const char* const l_args[] = { "binary" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::none,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindValueIfGiven(l_string, string("super"))));

    CPPUNIT_ASSERT_EQUAL(std::string("toto"), l_string);
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(1, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("toto"), l_string);
  }
}


void
TestApplication::bindCallback(void)
{
  bool              l_called = false;
  auto              l_function = [&l_called](void) {
    l_called = true;
  };


  {
    Application       l_app(true);
    const char* const l_args[] = { "binary" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt",
                                            Application::argument::none,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindCallback(l_function)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(1, l_args));
    CPPUNIT_ASSERT_EQUAL(false, l_called);
  }

  {
    Application       l_app(true);
    const char* const l_args[] = { "binary", "--opt" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt",
                                            Application::argument::none,
                                            Application::requirement::optional,
                                            "",
                                            l_app.bindCallback(l_function)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(2, l_args));
    CPPUNIT_ASSERT_EQUAL(true, l_called);
  }
}


void
TestApplication::bindValues(void)
{

  {
    // OK given string in list
    Application       l_app(true);
    std::string       l_string;
    const char* const l_args[] = { "binary", "--string", "string1" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_string, vector<string>({"string1", "string2"}))));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("string1"), l_string);
  }

  {
    // KO given string not list
    Application       l_app(true);
    std::string       l_string;
    const char* const l_args[] = { "binary", "--string", "unknown" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_string, vector<string>({"string1", "string2"}))));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // OK given unsigned int in list
    Application       l_app(true);
    uint32_t          l_value;
    const char* const l_args[] = { "binary", "--string", "50" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_value, vector<uint32_t>({10u, 30u, 50u}))));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(50u, l_value);
  }

  {
    // KO given unsigned int not in list
    Application       l_app(true);
    uint32_t          l_value;
    const char* const l_args[] = { "binary", "--string", "120" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_value, vector<uint32_t>({10u, 30u, 50u}))));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // KO given unsigned int not uint castable
    Application       l_app(true);
    uint32_t          l_value;
    const char* const l_args[] = { "binary", "--string", "fdsfds120" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_value, vector<uint32_t>({10u, 30u, 50u}))));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // KO given unsigned int not uint castable
    Application       l_app(true);
    uint32_t          l_value;
    const char* const l_args[] = { "binary", "--string", "-500" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "string",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindValues(l_value, vector<uint32_t>({10u, 30u, 50u}))));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }
}

void
TestApplication::bindNumber(void)
{

  {
    // OK given arg is number
    Application       l_app(true);
    int32_t           l_value;
    const char* const l_args[] = { "binary", "--opt", "123" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindNumber(l_value)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(123, l_value);
  }

  {
    // KO given arg is not a number
    Application       l_app(true);
    int32_t           l_value;
    const char* const l_args[] = { "binary", "--opt", "toto" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindNumber(l_value)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // KO given arg is too low
    Application       l_app(true);
    int32_t           l_value;
    const char* const l_args[] = { "binary", "--opt", "100" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindNumber(l_value, 150, 300)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

  {
    // KO given arg is too high
    Application       l_app(true);
    int32_t           l_value;
    const char* const l_args[] = { "binary", "--opt", "500" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "opt",
                                            Application::argument::mandatory,
                                            Application::requirement::mandatory,
                                            "",
                                            l_app.bindNumber(l_value, 150, 300)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }

}


void
TestApplication::bindRegex(void)
{
  {
    // OK valid regex
    Application       l_app(true);
    std::string       l_regex;
    const char* const l_args[] = { "binary", "--regex", "^to.*(ta|tal)$" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "regex", Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindRegex(l_regex)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(std::string("^to.*(ta|tal)$"), l_regex);
  }

  {
    // KO invalid regex
    Application       l_app(true);
    std::string       l_regex;
    const char* const l_args[] = { "binary", "--regex", "^/*[sS]*?*/|^//" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('s', "regex", Application::argument::mandatory, Application::requirement::mandatory, "", l_app.bindRegex(l_regex)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(3, l_args), std::runtime_error);
  }
}

void
TestApplication::readArgs(void)
{
  bool l_called;

  {
    // OK no args
    Application       l_app(true);
    const char* const l_args[] = { "binary" };
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(1, l_args));
    CPPUNIT_ASSERT_EQUAL(false, l_app.isOptionGiven("help"));
    CPPUNIT_ASSERT_EQUAL(false, l_app.isOptionGiven("log-level"));
  }

  {
    // OK missing optional arg
    Application       l_app(true);
    const char* const l_args[] = { "binary" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt", Application::argument::none, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(1, l_args));
    CPPUNIT_ASSERT_EQUAL(false, l_app.isOptionGiven("opt"));
  }

  {
    // KO missing mandatory arg
    Application       l_app(true);
    const char* const l_args[] = { "binary" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt", Application::argument::none, Application::requirement::mandatory, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_THROW(l_app.readArgs(1, l_args), std::runtime_error);
  }

  {
    // OK given mandatory arg (long form)
    Application       l_app(true);
    const char* const l_args[] = { "binary", "--opt" };
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt", Application::argument::none, Application::requirement::mandatory, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(2, l_args));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("opt"));
  }

  {
    // OK given mandatory arg (short form)
    Application       l_app(true);
    const char* const l_args[] = { "binary", "-o" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "opt", Application::argument::none, Application::requirement::mandatory, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(2, l_args));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("opt"));
  }

  {
    // KO unknown option
    Application       l_app(true);
    const char* const l_args[] = { "binary", "-u" };

    CPPUNIT_ASSERT_THROW(l_app.readArgs(2, l_args), std::runtime_error);
  }


  {
    // OK unparsed arguments
    Application       l_app(true);
    const char* const l_args[] = { "binary", "bla1", "bla2", "bla3" };

    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(4, l_args));
    CPPUNIT_ASSERT_EQUAL(3ul, l_app.m_remainingArgs.size());
    CPPUNIT_ASSERT_EQUAL(std::string("bla1"), l_app.m_remainingArgs[0]);
    CPPUNIT_ASSERT_EQUAL(std::string("bla2"), l_app.m_remainingArgs[1]);
    CPPUNIT_ASSERT_EQUAL(std::string("bla3"), l_app.m_remainingArgs[2]);
  }
}

void
TestApplication::isOptionGiven(void)
{
  bool l_called;

  {
    // OK single option
    Application       l_app(true);
    const char* const l_args[] = { "binary", "-o" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "oo",  Application::argument::none, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('p', "pp",  Application::argument::none, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(2, l_args));
    CPPUNIT_ASSERT_EQUAL(true,  l_app.isOptionGiven("o"));
    CPPUNIT_ASSERT_EQUAL(true,  l_app.isOptionGiven("oo"));
    CPPUNIT_ASSERT_EQUAL(false, l_app.isOptionGiven("p"));
    CPPUNIT_ASSERT_EQUAL(false, l_app.isOptionGiven("pp"));
  }


  {
    // OK multiple options
    Application       l_app(true);
    const char* const l_args[] = { "binary", "-o", "--pp" };

    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('o', "oo",  Application::argument::none, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('p', "pp",  Application::argument::none, Application::requirement::optional, "decr", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.readArgs(3, l_args));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("o"));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("oo"));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("p"));
    CPPUNIT_ASSERT_EQUAL(true, l_app.isOptionGiven("pp"));
  }
}


void
TestApplication::usage(void)
{
  {
    // OK no additions, check alignment
    Application       l_app(true);
    std::stringstream l_out;

    l_app.m_binName = "binary";
    CPPUNIT_ASSERT_NO_THROW(l_app.usage(l_out));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("-e | --log-level <arg> : "));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("-h | --help            : "));
  }

  {
    // OK multi line descr
    Application       l_app(true);
    bool              l_called;
    std::stringstream l_out;

    l_app.m_binName = "binary";
    CPPUNIT_ASSERT_NO_THROW(l_app.addOption('p', "pp",  Application::argument::none, Application::requirement::optional, "line1\nline2 line3", l_app.bindGiven(l_called)));
    CPPUNIT_ASSERT_NO_THROW(l_app.usage(l_out));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("-e | --log-level <arg> : "));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("-h | --help            : "));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("-p | --pp              : line1"));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("                         line2 line3"));
  }



  {
    // OK with help text
    Application       l_app(true);
    std::stringstream l_out;

    l_app.m_binName = "binary";
    CPPUNIT_ASSERT_NO_THROW(l_app.addHelpMsg("this is help line 1"));
    CPPUNIT_ASSERT_NO_THROW(l_app.addHelpMsg("this is help line 2"));
    CPPUNIT_ASSERT_NO_THROW(l_app.usage(l_out));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("  this is help line 1\n"));
    CPPUNIT_ASSERT(string::npos != l_out.str().find("  this is help line 2\n"));
  }

}



XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
