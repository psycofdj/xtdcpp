#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/SyslogAppender.hh"  //libcore
#include <boost/filesystem.hpp>
#include <boost/any.hpp>


using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestSyslogAppender : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestSyslogAppender);
  CPPUNIT_TEST(constructor);
  CPPUNIT_TEST(print);
  CPPUNIT_TEST(create);
  CPPUNIT_TEST_SUITE_END();

private:
  void constructor(void);
  void print(void);
  void create(void);


private:
  FormattedRecord rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestSyslogAppender);


FormattedRecord
TestSyslogAppender::rec(level p_level, const std::string& p_content) const
{
  Record          l_rec;
  FormattedRecord l_frec(l_rec);
  l_frec.m_fulllog = p_content;
  l_frec.m_level   = p_level;

  return l_frec;
}


void
TestSyslogAppender::constructor(void)
{
  uint32_t          l_open    = 0;
  uint32_t          l_close   = 0;
  auto              l_onOpen  = [&](const boost::any&) { l_open++;  };
  auto              l_onClose = [&](const boost::any&) { l_close++; };
  CWrap::hook_state l_state1("openlog",  l_onOpen);
  CWrap::hook_state l_state2("closelog", l_onClose);

  CPPUNIT_ASSERT_EQUAL(0u, SyslogAppender::ms_instances);
  {
    SyslogAppender l_i1("name", LOG_PID, LOG_LOCAL0);
    CPPUNIT_ASSERT_EQUAL(1u, SyslogAppender::ms_instances);
    CPPUNIT_ASSERT_EQUAL(1u, l_open);
    CPPUNIT_ASSERT_EQUAL(0u, l_close);
    {
      SyslogAppender l_i2("name", LOG_PID, LOG_LOCAL0);
      CPPUNIT_ASSERT_EQUAL(2u, SyslogAppender::ms_instances);
      CPPUNIT_ASSERT_EQUAL(1u, l_open);
      CPPUNIT_ASSERT_EQUAL(0u, l_close);
    }
    CPPUNIT_ASSERT_EQUAL(1u, SyslogAppender::ms_instances);
    CPPUNIT_ASSERT_EQUAL(1u, l_open);
    CPPUNIT_ASSERT_EQUAL(0u, l_close);
  }
  CPPUNIT_ASSERT_EQUAL(0u, SyslogAppender::ms_instances);
  CPPUNIT_ASSERT_EQUAL(1u, l_open);
  CPPUNIT_ASSERT_EQUAL(1u, l_close);
}


void
TestSyslogAppender::print(void)
{
  SyslogAppender l_i1("name", LOG_PID, LOG_LOCAL0);
  string         l_format;
  auto           l_frec = rec(level::debug, "log string");

  CPPUNIT_ASSERT_NO_THROW(l_i1.print(l_frec));
}

void
TestSyslogAppender::create(void)
{
  map<string,string> l_props;

  {
    // KO missing options
    l_props = {};
    CPPUNIT_ASSERT_THROW(SyslogAppender::create("A1", l_props), log_error);
  }

  {
    // KO missing facility
    l_props = {
      { "log.appender.A1.options", "LOG_PID" }
    };
    CPPUNIT_ASSERT_THROW(SyslogAppender::create("A1", l_props), log_error);
  }

  {
    // KO invalid option
    l_props = {
      { "log.appender.A1.options",  "unknown"    },
      { "log.appender.A1.facility", "LOG_LOCAL0" }
    };
    CPPUNIT_ASSERT_THROW(SyslogAppender::create("A1", l_props), log_error);
  }

  {
    // KO invalid facility
    l_props = {
      { "log.appender.A1.options",  ""        },
      { "log.appender.A1.facility", "unknown" }
    };
    CPPUNIT_ASSERT_THROW(SyslogAppender::create("A1", l_props), log_error);
  }


  {
    // OK
    l_props = {
      { "log.appender.A1.identity", "%(binname)" },
      { "log.appender.A1.options",  "LOG_PID, LOG_NOWAIT" },
      { "log.appender.A1.facility", "LOG_LOCAL0"          }
    };
    sptr<Appender> l_a1;
    CPPUNIT_ASSERT_NO_THROW(l_a1 = SyslogAppender::create("A1", l_props));
    sptr<SyslogAppender>   l_obj = std::static_pointer_cast<SyslogAppender>(l_a1);
    CPPUNIT_ASSERT_EQUAL(int32_t(LOG_PID | LOG_NOWAIT), l_obj->m_opts);
    CPPUNIT_ASSERT_EQUAL(LOG_LOCAL0, l_obj->m_facility);
  }

}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
