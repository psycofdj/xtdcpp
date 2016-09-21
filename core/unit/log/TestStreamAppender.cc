#include <MainTestApplication.hh> //libtests
#include "log/StreamAppender.hh"  //libtests

class TestStreamAppender : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestStreamAppender);
  CPPUNIT_TEST(constructor);
  CPPUNIT_TEST_SUITE_END();

private:
  void constructor(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestStreamAppender);

void
TestStreamAppender::constructor(void)
{
}


XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
