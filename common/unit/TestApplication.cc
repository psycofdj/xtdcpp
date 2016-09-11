#include "logger.hh"
#include "Application.hh"        //libcommon
#include "MainTestApplication.hh" //libtests

using namespace xtd;


class TestApplication : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestApplication);
  CPPUNIT_TEST(fakeTest1);
  CPPUNIT_TEST(fakeTest2);
  CPPUNIT_TEST_SUITE_END();

public:
  void fakeTest1(void);
  void fakeTest2(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestApplication);

void
TestApplication::fakeTest1(void)
{
  string   l_filename = "/tmp/conf/test.conf";
  uint32_t l_line  = 324;

  logger::get().initialize("binname", logger::level::crit);

  logger::get().setLevel("module1",            logger::level::info);
  logger::get().setLevel("module2",            logger::level::err);
  logger::get().setLevel("module2.submodule2", logger::level::crit);

  logger::err   ("module1.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module1.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module1.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::err   ("module1.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module1.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module1.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);

  logger::err   ("module2.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module2.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module2.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::err   ("module2.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module2.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module2.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  CPPUNIT_ASSERT_EQUAL(false, true);
}


void
TestApplication::fakeTest2(void)
{
  string   l_filename = "/tmp/conf/test.conf";
  uint32_t l_line  = 324;

  logger::get().initialize("binname", logger::level::crit);

  logger::get().setLevel("module1",            logger::level::info);
  logger::get().setLevel("module2",            logger::level::err);
  logger::get().setLevel("module2.submodule2", logger::level::crit);

  logger::err   ("module1.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module1.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module1.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::err   ("module1.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module1.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module1.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);

  logger::err   ("module2.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module2.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module2.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::err   ("module2.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::info  ("module2.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  logger::debug ("module2.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  CPPUNIT_ASSERT_EQUAL(true, true);
}

XTD_TEST_MAIN();
