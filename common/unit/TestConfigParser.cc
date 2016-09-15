#include <boost/filesystem.hpp>
#include "MainTestApplication.hh" // libtests
#include "CWrap.hh"               // libtests
#include "ConfParser.hh"          // libcommon
#include "error.hh"               // libcommon
#include <config/Parser.hh>       // libcommon


using namespace std;
using namespace xtd::tests;
using xtd::ConfParser;

class TestConfParser : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestConfParser);
  CPPUNIT_TEST(Constructor);
  CPPUNIT_TEST_SUITE_END();
private:

  void Constructor(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestConfParser);

void
TestConfParser::Constructor(void)
{
  {
    // KO file not exsist
    CPPUNIT_ASSERT_THROW(ConfParser l_obj("doesnotexist"), xtd::error);
  }

  {
    // KO unreadable existing file
    CPPUNIT_ASSERT_THROW(ConfParser l_obj("/dev/console"), xtd::error);
  }

  {
    // KO read error
    CWrap::state l_state("fread", {{ "error", 1 }});

    CPPUNIT_ASSERT_THROW(ConfParser l_obj("/dev/random"), xtd::error);
  }

  {
    // OK read
    string l_config = Globals::get().testDir() + "/data/simple.conf";
    CPPUNIT_ASSERT_NO_THROW(ConfParser l_obj(l_config));
  }


  string l_data = R"data(
  [s1]
  {
    s1key1 : s1key1val
    [s1s2]
    {
      s1s2key1 : s1s2key1val
      s1s2key2 : s1s2key2val
      var   : ${testwithenv}
      env   : $ENV{testwithenv}
      param : $PARAM{testwithenv}

      param1 : ${testwithenv} with trail
      param2 : ${testwithenv} ${double} with trail
      param3 :   ${testwithenv} $PARAM{double} with trail and ${param}
      param4 :   ${testwithenv} $PARAM{double} with $fakes vars and ${param}
    }
  })data";


  xtd::config::Parser l_parser;
  l_parser.parse(l_data.begin(), l_data.end());
  l_parser.dump();
  CPPUNIT_ASSERT(false);
}

XTD_TEST_MAIN();
