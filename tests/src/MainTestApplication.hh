#pragma once
#include "Application.hh" // libcore
#include "cppunit/TestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include "Globals.h"

namespace CppUnit {
class Outputter;
class TestResultCollector;
}

namespace xtd {
namespace tests {



class MainTestApplication : public Application
{
public:
  MainTestApplication(void);


private:
  int  process(void);
  void initialize(void);

  CppUnit::Outputter* createOutputter(CppUnit::TestResultCollector& p_result, std::ostream& p_stream) const;

protected:
  string m_filter;
  bool   m_progress;
  string m_ouputter;
  string m_srcDir;
  string m_topSrcDir;
  string m_topBuildDir;
  string m_testDir;
};

}}


#define XTD_TEST_MAIN_CLASS(p_className)        \
  int main(int p_argc, char** p_argv)           \
  {                                             \
    p_className l_testClass;                    \
    return l_testClass.execute(p_argc, p_argv); \
  }

#define XTD_TEST_MAIN() XTD_TEST_MAIN_CLASS(xtd::tests::MainTestApplication);
