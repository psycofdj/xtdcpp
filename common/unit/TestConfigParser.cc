#include <boost/filesystem.hpp>
#include "MainTestApplication.hh" // libtests
#include "CWrap.hh"               // libtests
#include "ConfParser.hh"          // libcommon
#include "error.hh"               // libcommon


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
}

XTD_TEST_MAIN();
