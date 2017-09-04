#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> // libtests
#include <TestFixture.hh>         // libtests
#include <CWrap.hh>               // libtests
#include "log/Appender.hh"        // libcore
#include "log/MemoryAppender.hh"  // libcore
#include <boost/filesystem.hpp>
#include <boost/any.hpp>


using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestAppender : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestAppender);
  CPPUNIT_TEST(filter);
  CPPUNIT_TEST_SUITE_END();

private:
  void filter(void);

private:
  Record rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestAppender);


Record
TestAppender::rec(level p_level, const std::string& p_content) const
{
  Record l_rec;
  l_rec.m_format = p_content;
  l_rec.m_level  = p_level;
  return l_rec;
}


void
TestAppender::filter(void)
{
  MemoryAppender l_i1(10);

  {
    // record is not filtered
    l_i1.log(rec(level::debug, "log string"));
    CPPUNIT_ASSERT_EQUAL(1ul, l_i1.copyRecords().size());
  }

  {
    // record is filtered
    l_i1.addFilter([](const FormattedRecord& p_rec) {
        return p_rec.m_level != level::debug;
      });
    l_i1.log(rec(level::debug, "log string"));
    CPPUNIT_ASSERT_EQUAL(1ul, l_i1.copyRecords().size());
    l_i1.log(rec(level::info, "log string"));
    CPPUNIT_ASSERT_EQUAL(2ul, l_i1.copyRecords().size());
  }

  {
    // clearing filters
    l_i1.clearFilters();
    l_i1.log(rec(level::debug, "log string"));
    CPPUNIT_ASSERT_EQUAL(3ul, l_i1.copyRecords().size());
  }

  // idiot coverage calls
  {
    l_i1.clearFormatter();
    l_i1.setFormatter(std::make_shared<Formatter>());
  }
}


XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
