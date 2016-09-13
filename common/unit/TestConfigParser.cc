#include "MainTestApplication.hh" // libtests
#include "ConfParser.hh"          // libcommon
#include "error.hh"               // libcommon
#include "libc_wrapper.hh"

using namespace std;
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
    LibCWrapper::set("fread", {{ "error", 1 }});
    CPPUNIT_ASSERT_THROW(ConfParser l_obj("/dev/random"), xtd::error);
    LibCWrapper::reset();
  }

  {
    // OK read
    CPPUNIT_ASSERT_NO_THROW(ConfParser l_obj("/dev/random"));
  }
}

XTD_TEST_MAIN();
