#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/Logger.hh"          //libcore
#include "log/MemoryAppender.hh"  //libcore

using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestLogger : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestLogger);
  CPPUNIT_TEST(get);
  CPPUNIT_TEST(getLevels);
  CPPUNIT_TEST(updateLevels);
  CPPUNIT_TEST(setAllLevels);
  CPPUNIT_TEST(getLevelTo);
  CPPUNIT_TEST(addAppender);
  CPPUNIT_TEST_SUITE_END();

private:
  void get(void);
  void getLevels(void);
  void setAllLevels(void);
  void updateLevels(void);
  void getLevelTo(void);
  void addAppender(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestLogger);



void
TestLogger::get(void)
{
  RootLogger  l_root;
  Logger*     l_ptr;

  CPPUNIT_ASSERT_EQUAL(0ul, l_root.m_loggers.size());

  {
    // OK empty name is self
    auto& l_log = l_root.get("");
    CPPUNIT_ASSERT_NO_THROW({
        RootLogger& l_cast = dynamic_cast<RootLogger&>(l_log);
        CPPUNIT_ASSERT(&l_log == &l_cast);
      });
  }

  {
    // OK first get creates logger
    auto& l_log = l_root.get("xtd");
    CPPUNIT_ASSERT_EQUAL(1ul, l_root.m_loggers.size());
    l_ptr = &l_log;
  }

  {
    // same get give same logger
    auto& l_log = l_root.get("xtd");
    CPPUNIT_ASSERT_EQUAL(1ul, l_root.m_loggers.size());
    CPPUNIT_ASSERT_EQUAL(l_ptr, &l_log);
  }

  {
    // child logger is created from parent
    l_root.get("xtd").setLevel(level::debug);
    auto& l_log = l_root.get("xtd.a.b.c");
    CPPUNIT_ASSERT_EQUAL(level::debug, l_log.getLevel());
    l_root.get("xtd.a").setLevel(level::info);
    auto& l_log2 = l_root.get("xtd.a.b");
    CPPUNIT_ASSERT_EQUAL(level::info, l_log2.getLevel());
  }
}

void
TestLogger::getLevels(void)
{
  RootLogger l_root;
  l_root.setLevel(level::notice);
  l_root.get("xtd.a.1").setLevel(level::debug);
  l_root.get("xtd.a.2").setLevel(level::info);
  l_root.get("xtd.b.1").setLevel(level::warning);
  l_root.get("xtd.b.2").setLevel(level::crit);

  auto l_levels = l_root.getLevels();
  CPPUNIT_ASSERT_EQUAL(level::notice,  l_levels[""]);
  CPPUNIT_ASSERT_EQUAL(level::debug,   l_levels["xtd.a.1"]);
  CPPUNIT_ASSERT_EQUAL(level::info,    l_levels["xtd.a.2"]);
  CPPUNIT_ASSERT_EQUAL(level::warning, l_levels["xtd.b.1"]);
  CPPUNIT_ASSERT_EQUAL(level::crit,    l_levels["xtd.b.2"]);
}

void
TestLogger::setAllLevels(void)
{
  RootLogger l_root;

  l_root.setLevel(level::notice);
  l_root.get("xtd.a.1").setLevel(level::debug);
  l_root.get("xtd.a.2").setLevel(level::info);
  l_root.get("xtd.b.1").setLevel(level::warning);
  l_root.get("xtd.b.2").setLevel(level::crit);

  l_root.setAllLevels(level::debug);
  auto l_levels = l_root.getLevels();
  CPPUNIT_ASSERT_EQUAL(level::debug, l_levels[""]);
  CPPUNIT_ASSERT_EQUAL(level::debug, l_levels["xtd.a.1"]);
  CPPUNIT_ASSERT_EQUAL(level::debug, l_levels["xtd.a.2"]);
  CPPUNIT_ASSERT_EQUAL(level::debug, l_levels["xtd.b.1"]);
  CPPUNIT_ASSERT_EQUAL(level::debug, l_levels["xtd.b.2"]);

  l_root.setAllValueLevels(LOG_CRIT);
  l_levels = l_root.getLevels();
  CPPUNIT_ASSERT_EQUAL(level::crit, l_levels[""]);
  CPPUNIT_ASSERT_EQUAL(level::crit, l_levels["xtd.a.1"]);
  CPPUNIT_ASSERT_EQUAL(level::crit, l_levels["xtd.a.2"]);
  CPPUNIT_ASSERT_EQUAL(level::crit, l_levels["xtd.b.1"]);
  CPPUNIT_ASSERT_EQUAL(level::crit, l_levels["xtd.b.2"]);

}


void
TestLogger::updateLevels(void)
{
  RootLogger l_root;

  l_root.setLevel(level::notice);
  l_root.get("xtd.a.1").setLevel(level::debug);
  l_root.get("xtd.a.2").setLevel(level::info);
  l_root.get("xtd.b.1").setLevel(level::warning);
  l_root.get("xtd.b.2").setLevel(level::notice);


  l_root.updateLevels("xtd\\..\\.1", level::crit);
  CPPUNIT_ASSERT_EQUAL(level::crit,   l_root.get("xtd.a.1").getLevel());
  CPPUNIT_ASSERT_EQUAL(level::info,   l_root.get("xtd.a.2").getLevel());
  CPPUNIT_ASSERT_EQUAL(level::crit,   l_root.get("xtd.b.1").getLevel());
  CPPUNIT_ASSERT_EQUAL(level::notice, l_root.get("xtd.b.2").getLevel());

  l_root.updateLevels("", level::debug);
  CPPUNIT_ASSERT_EQUAL(level::debug,   l_root.get("").getLevel());
}

void
TestLogger::getLevelTo(void)
{
  RootLogger l_root;

  l_root.setLevel(level::debug);
  CPPUNIT_ASSERT_EQUAL(level::debug, l_root.getLevelTo(""));
  CPPUNIT_ASSERT_EQUAL(level::debug, l_root.getLevelTo("unknown module"));
  CPPUNIT_ASSERT_EQUAL(0ul, l_root.m_loggers.size());

  l_root.get("m1").setLevel(level::err);
  l_root.get("m2").setLevel(level::info);
  CPPUNIT_ASSERT_EQUAL(2ul, l_root.m_loggers.size());

  CPPUNIT_ASSERT_EQUAL(level::err,   l_root.getLevelTo("m1"));
  CPPUNIT_ASSERT_EQUAL(level::err,   l_root.getLevelTo("m1.s1"));
  CPPUNIT_ASSERT_EQUAL(level::info,  l_root.getLevelTo("m2"));
  CPPUNIT_ASSERT_EQUAL(level::info,  l_root.getLevelTo("m2.s2"));
  CPPUNIT_ASSERT_EQUAL(level::debug, l_root.getLevelTo("m3"));
  CPPUNIT_ASSERT_EQUAL(2ul, l_root.m_loggers.size());
}


void
TestLogger::addAppender(void)
{
  RootLogger     l_root;

  CPPUNIT_ASSERT_EQUAL(0ul, l_root.m_appenders.size());
  CPPUNIT_ASSERT_EQUAL(0ul, l_root.get("xtd").m_appenders.size());
  l_root.addAppender(std::make_shared<MemoryAppender>());
  CPPUNIT_ASSERT_EQUAL(1ul, l_root.m_appenders.size());
  CPPUNIT_ASSERT_EQUAL(0ul, l_root.get("xtd").m_appenders.size());
  CPPUNIT_ASSERT_EQUAL(1ul, l_root.get("m1").m_appenders.size());
  CPPUNIT_ASSERT_EQUAL(0ul, l_root.get("xtd.s1").m_appenders.size());

  l_root.log(level::crit, "message");

  l_root.clearAppenders();
  CPPUNIT_ASSERT_EQUAL(0ul, l_root.m_appenders.size());
  CPPUNIT_ASSERT_EQUAL(1ul, l_root.get("m1").m_appenders.size());
}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
