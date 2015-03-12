#include "AvgTimedValueTest.hh"

using namespace xtd::counters;
namespace bpt = boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION(AvgTimedValueTest);

void
AvgTimedValueTest::shrink_safeTest(void)
{
  size_t l_maxSampleSize = 600000;

  // check nominal
  {
    AvgTimedValue l_counter("test", 1, 10000, 0);
    bpt::ptime    l_now = bpt::microsec_clock::local_time();

    for (size_t c_idx = 0; c_idx < l_maxSampleSize; c_idx++)
      l_counter.m_samples.push_back(std::make_pair(l_now - bpt::seconds(c_idx), 0));

    CPPUNIT_ASSERT(l_counter.m_samples.size() == l_maxSampleSize);
    l_counter.shrink_safe();
    CPPUNIT_ASSERT_EQUAL(10ul, l_counter.m_samples.size());

    for (size_t c_idx = 0; c_idx < 10; c_idx++)
      CPPUNIT_ASSERT(l_counter.m_samples[c_idx].first == l_now - bpt::seconds(c_idx));
  }

  // check en cas d'egalitÃ©es de valeurs de timpestamps
  {
    AvgTimedValue l_counter("test", 1, 10000, 0);
    bpt::ptime    l_now = bpt::microsec_clock::local_time();

    for (size_t c_idx = 0; c_idx < l_maxSampleSize; c_idx++)
    {
      l_counter.m_samples.push_back(std::make_pair(l_now - bpt::seconds(c_idx), 0));
      l_counter.m_samples.push_back(std::make_pair(l_now - bpt::seconds(c_idx), 0));
    }

    CPPUNIT_ASSERT(l_counter.m_samples.size() == 2 * l_maxSampleSize);
    l_counter.shrink_safe();
    CPPUNIT_ASSERT_EQUAL(20ul, l_counter.m_samples.size());

    for (size_t c_idx = 0; c_idx < 20; c_idx += 2)
    {
      CPPUNIT_ASSERT(l_counter.m_samples[c_idx].first     == l_now - bpt::seconds(c_idx / 2));
      CPPUNIT_ASSERT(l_counter.m_samples[c_idx + 1].first == l_now - bpt::seconds(c_idx / 2));
    }
  }
}
