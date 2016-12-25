#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/MemoryAppender.hh"  //libcore
#include <boost/filesystem.hpp>
#include <boost/any.hpp>


using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestMemoryAppender : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestMemoryAppender);
  CPPUNIT_TEST(print);
  CPPUNIT_TEST(create);
  CPPUNIT_TEST_SUITE_END();

private:
  void print(void);
  void create(void);

private:
  FormattedRecord rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestMemoryAppender);


FormattedRecord
TestMemoryAppender::rec(level p_level, const std::string& p_content) const
{
  Record          l_rec;
  FormattedRecord l_frec(l_rec);
  l_frec.m_fulllog = p_content;
  l_frec.m_level   = p_level;

  return l_frec;
}


void
TestMemoryAppender::print(void)
{
  MemoryAppender l_i1(10);

  {
    // record is copied
    l_i1.print(rec(level::debug, "log string"));
    CPPUNIT_ASSERT_EQUAL(1ul, l_i1.copyRecords().size());
    CPPUNIT_ASSERT_EQUAL(string("log string"), l_i1.copyRecords()[0].m_fulllog);
  }

  {
    // last 10 record are kept
    for (size_t c_count = 0; c_count < 20; c_count++)
      l_i1.print(rec(level::debug, "log string " + std::to_string(c_count)));
    CPPUNIT_ASSERT_EQUAL(10ul, l_i1.copyRecords().size());
    CPPUNIT_ASSERT_EQUAL(string("log string 10"), l_i1.copyRecords().front().m_fulllog);
    CPPUNIT_ASSERT_EQUAL(string("log string 19"), l_i1.copyRecords().back().m_fulllog);
  }
}

void
TestMemoryAppender::create(void)
{
  map<string,string> l_props;
  {
    // KO size not a number
    l_props = {
      { "log.appender.A1.size", "value" }
    };
    CPPUNIT_ASSERT_THROW(MemoryAppender::create("A1", l_props), log_error);
  }

  {
    // KO size negative
    l_props = {
      { "log.appender.A1.size", "-87" }
    };
    CPPUNIT_ASSERT_THROW(MemoryAppender::create("A1", l_props), log_error);
  }


  {
    // OK, default size
    l_props = {};
    CPPUNIT_ASSERT_NO_THROW(MemoryAppender::create("A1", l_props));
  }

  {
    // OK
    l_props = {
      { "log.appender.A1.size", "10" }
    };
    CPPUNIT_ASSERT_NO_THROW(MemoryAppender::create("A1", l_props));
  }
}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
