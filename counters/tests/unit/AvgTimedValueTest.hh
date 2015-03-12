#ifndef COUNTERS_AVGTIMEDVALUETEST_HH_
# define COUNTERS_AVGTIMEDVALUETEST_HH_

# include <cppunit/extensions/HelperMacros.h>
# include <cppunit/TestFixture.h>

# define private   public
# define protected public
# include <AvgTimedValue.hh>
# undef private
# undef protected

class AvgTimedValueTest : public CppUnit::TestFixture
{
public :
  CPPUNIT_TEST_SUITE(AvgTimedValueTest);
  CPPUNIT_TEST(shrink_safeTest);
  CPPUNIT_TEST_SUITE_END();

public:
  void shrink_safeTest(void);
};

#endif // COUNTERS_AVGTIMEDVALUETEST_HH_
