#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/MemoryAppender.hh"  //libcore
#include <boost/filesystem.hpp>
#include <boost/any.hpp>



using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestFields : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestFields);
  CPPUNIT_TEST(get);
  CPPUNIT_TEST(set);
  CPPUNIT_TEST(exists);
  CPPUNIT_TEST_SUITE_END();

private:
  void get(void);
  void set(void);
  void exists(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestFields);


void
TestFields::get(void)
{
  Fields<string> l_obj;
  string         l_data;

  l_obj.m_message = "message";

  CPPUNIT_ASSERT_EQUAL(false, l_obj.get("unknown", l_data));
  CPPUNIT_ASSERT_EQUAL(true, l_obj.get("message", l_data));
  CPPUNIT_ASSERT_EQUAL(string("message"), l_data);
  CPPUNIT_ASSERT_EQUAL(string("message"), l_obj.get("message"));
}

void
TestFields::set(void)
{
  Fields<string> l_obj;

  CPPUNIT_ASSERT_EQUAL(false, l_obj.set("unknown", "value"));
  CPPUNIT_ASSERT_EQUAL(true,  l_obj.set("message", "message"));
  CPPUNIT_ASSERT_EQUAL(string("message"), l_obj.get("message"));
}


void
TestFields::exists(void)
{
  Fields<string> l_obj;

  CPPUNIT_ASSERT_EQUAL(false, l_obj.exists("unknown"));
  CPPUNIT_ASSERT_EQUAL(true,  l_obj.exists("message"));
}

XTD_TEST_MAIN();
