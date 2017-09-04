#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "error.hh"               //libcore
#include "log/helpers.hh"         //libcore
#include <boost/algorithm/string/predicate.hpp>

using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestHelpers : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestHelpers);
  CPPUNIT_TEST(from);
  CPPUNIT_TEST(to_value);
  CPPUNIT_TEST(to_string);
  CPPUNIT_TEST(is_valid);
  CPPUNIT_TEST(raise);
  CPPUNIT_TEST_SUITE_END();

private:
  void from(void);
  void to_value(void);
  void to_string(void);
  void is_valid(void);
  void raise(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestHelpers);


void
TestHelpers::from(void)
{
  CPPUNIT_ASSERT_EQUAL(level::err,    log::from(LOG_ERR));
  CPPUNIT_ASSERT_EQUAL(level::crit,   log::from(3425));
  CPPUNIT_ASSERT_EQUAL(level::debug,  log::from("debug"));
  CPPUNIT_ASSERT_EQUAL(level::crit,   log::from("unknown"));
}

void
TestHelpers::to_value(void)
{
  CPPUNIT_ASSERT_EQUAL(uint32_t(LOG_DEBUG), log::to_value(level::debug));
  CPPUNIT_ASSERT_EQUAL(uint32_t(LOG_INFO),  log::to_value(level::info));
}

void
TestHelpers::to_string(void)
{
  CPPUNIT_ASSERT_EQUAL(string("debug"), log::to_string(level::debug));
  CPPUNIT_ASSERT_EQUAL(string("err"),   log::to_string(level::err));
  CPPUNIT_ASSERT_EQUAL(string("crit"),  log::to_string(to_enum<level>(100)));

  stringstream l_str;

  l_str << level::debug;
  CPPUNIT_ASSERT_EQUAL(string("debug"), l_str.str());
}

void
TestHelpers::is_valid(void)
{
  CPPUNIT_ASSERT_EQUAL(true,  log::is_valid("debug"));
  CPPUNIT_ASSERT_EQUAL(false, log::is_valid("unknown"));
  CPPUNIT_ASSERT_EQUAL(true,  log::is_valid(LOG_DEBUG));
  CPPUNIT_ASSERT_EQUAL(false, log::is_valid(145));
}

void
TestHelpers::raise(void)
{
  int l_line;

  // raise without location
  try {
    l_line = __LINE__; log::raise<error>("core.unit.log", "message");
  } catch (error& l_err) {
    CPPUNIT_ASSERT_EQUAL(false, boost::contains(l_err.what(), std::to_string(l_line)));
  }

  // raise with location
  try {
    l_line = __LINE__; log::raise<error>("core.unit.log", "message", HERE);
  } catch (error& l_err) {
    std::cout << l_err.what() << std::endl;
    CPPUNIT_ASSERT_EQUAL(true, boost::contains(l_err.what(), std::to_string(l_line)));
  }

  // raise with wrong number of arguments
  try {
    log::raise<error>("core.unit.log", "message %s");
  } catch (error& l_err) {
    CPPUNIT_ASSERT_EQUAL(true, boost::contains(l_err.what(), "wrong number of arguments"));
  }

  // raise with invalid format
  try {
    log::raise<error>("core.unit.log", "message %1% %2 end");
  } catch (error& l_err) {
    CPPUNIT_ASSERT_EQUAL(true, boost::contains(l_err.what(), "bad_format"));
  }
}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
