#include "MainTestApplication.hh"
#include "ProgressListener.hh"
#include "QuietOutputter.hh"

#include <cppunit/Outputter.h>           // libcppunit
#include <cppunit/TestResult.h>          // libcppunit
#include <cppunit/CompilerOutputter.h>   // libcppunit
#include <cppunit/XmlOutputter.h>        // libcppunit
#include <cppunit/TextOutputter.h>       // libcppunit
#include <cppunit/extensions/TestFactoryRegistry.h> // libcppunit
#include <cppunit/TestResultCollector.h> // libcppunit
#include <cppunit/ui/text/TestRunner.h>  // libcppunit
#include <cppunit/TestResultCollector.h>
#include <boost/regex.hpp>
#include <log/ConfLoader.hh>

namespace xtd {
namespace tests {


MainTestApplication::MainTestApplication(void):
  Application(),
  m_filter(),
  m_ouputter("text")
{
  Globals& l_glob = Globals::get();

  addOption('f', "filter", argument::mandatory, requirement::optional,
            "filter test cases on given regex pattern",
            bindRegex(m_filter));

  addOption('p', "progress", argument::none, requirement::optional,
            "output test cases names while running test suite",
            bindGiven(m_progress));

  addOption('o', "outputter", argument::mandatory, requirement::optional,
            "use one of the following result outputter : none,text,compiler,xml",
            bindValues(m_ouputter, std::vector<string>({ "none", "text", "compiler", "xml"})));

  addOption('n', "nocatch", argument::none, requirement::optional,
            "disable cppunit excaption catching, useful when debugging in gdb",
            bindGiven(m_disableCatch));

  addOption('s', "srcdir", argument::mandatory, requirement::mandatory,
            "current source directory",
            bindDir(l_glob.sourceDir()));

  addOption('t', "top-srcdir", argument::mandatory, requirement::mandatory,
            "project top source directory",
            bindDir(l_glob.topSourceDir()));

  addOption('b', "top-builddir", argument::mandatory, requirement::mandatory,
            "project top build directory",
            bindDir(l_glob.topBuildDir()));

  addOption('d', "testdir", argument::mandatory, requirement::mandatory,
            "project test source directory",
            bindDir(l_glob.testDir()));
}



CppUnit::Outputter*
MainTestApplication::createOutputter(CppUnit::TestResultCollector& p_result, std::ostream& p_stream) const
{
  if (m_ouputter == "none")
    return new QuietOutputter(&p_result, p_stream);
  if (m_ouputter == "text")
    return new CppUnit::TextOutputter(&p_result, p_stream);
  if (m_ouputter == "compiler")
    return new CppUnit::CompilerOutputter(&p_result, p_stream, "%p:%l:1 : error: ");
  if (m_ouputter == "text")
    return new CppUnit::XmlOutputter(&p_result, p_stream, "UTF-8");
  return new CppUnit::TextOutputter(&p_result, p_stream);
}


void
MainTestApplication::initialize(void)
{
  auto& l_logger = log::ConfLoader::get();

  l_logger.configure({
      { "log.logger.root",             "DEBUG, A1"       },
      { "log.appender.A1.class",       "SyslogAppender"  },
      { "log.appender.syslog.options", "LOG_PID"         },
      { "log.appender.A1.facility",    "LOG_LOCAL0"      }
    });
}

/**
 ** @details
 ** 1. this code leaks, Cppunit wont allow to delete a single test from a suite
 **    and we dont want to double free.
 **    not a problem since this is not a common use case.
 */
int
MainTestApplication::process(void)
{

  CppUnit::TextUi::TestRunner   l_runner;
  CppUnit::TestFactoryRegistry& l_registry  = CppUnit::TestFactoryRegistry::getRegistry();
  CppUnit::TestResultCollector& l_results   = l_runner.result();
  CppUnit::Outputter*           l_outputter = createOutputter(l_results, std::cout);

  if (true == m_filter.empty())
    l_runner.addTest(l_registry.makeTest());
  else
  {
    // 1.
    CppUnit::TestSuite* l_baseSuite = dynamic_cast<CppUnit::TestSuite*>(l_registry.makeTest());
    CppUnit::TestSuite* l_suite     = new CppUnit::TestSuite(l_baseSuite->getName());
    boost::regex        l_filter(".*" + m_filter);

    for (CppUnit::Test* c_suite : l_baseSuite->getTests())
    {
      CppUnit::TestSuite* l_tests = dynamic_cast<CppUnit::TestSuite*>(c_suite);
      if (0 != l_tests)
      {
        for (auto c_test : l_tests->getTests())
        {
          string l_name = c_test->getName();
          if (boost::regex_match(l_name.c_str(), l_filter))
            l_suite->addTest(c_test);
        }
      }
    }
    l_runner.addTest(l_suite);
  }

  l_runner.setOutputter(l_outputter);
  if (m_disableCatch)
    l_runner.eventManager().popProtector();

  sptr<ProgressListener> l_obj;

  if (m_progress) {
    l_obj.reset(new ProgressListener(std::cerr));
    l_runner.eventManager().addListener(l_obj.get());
  }

  return !l_runner.run("", false, true, false);
}

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
