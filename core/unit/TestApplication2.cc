#include "log.hh"                 // libcore
#include "Application.hh"         // libcore
#include "MainTestApplication.hh" // libtests

using namespace xtd;


class TestApplication2 : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestApplication2);
  CPPUNIT_TEST(fakeTest1);
  CPPUNIT_TEST(fakeTest2);
  CPPUNIT_TEST_SUITE_END();

public:
  void fakeTest1(void);
  void fakeTest2(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestApplication2);

void
TestApplication2::fakeTest1(void)
{
  string   l_filename = "/tmp/conf/test.conf";
  uint32_t l_line  = 324;

  log::getRoot().initialize("binname", log::level::crit);

  log::getRoot().setLevelTo("module1",            log::level::info);
  log::getRoot().setLevelTo("module2",            log::level::err);
  log::getRoot().setLevelTo("module2.submodule2", log::level::crit);

  log::err   ("module1.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module1.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module1.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::err   ("module1.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module1.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module1.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);

  log::err   ("module2.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module2.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module2.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::err   ("module2.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module2.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module2.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
}



void
TestApplication2::fakeTest2(void)
{
  string   l_filename = "/tmp/conf/test.conf";
  uint32_t l_line  = 324;

  log::getRoot().initialize("binname", log::level::crit);

  log::getRoot().setLevelTo("module1",            log::level::info);
  log::getRoot().setLevelTo("module2",            log::level::err);
  log::getRoot().setLevelTo("module2.submodule2", log::level::crit);

  log::err   ("module1.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module1.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module1.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::err   ("module1.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module1.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module1.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);

  log::err   ("module2.submodule1", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module2.submodule1", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module2.submodule1", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::err   ("module2.submodule2", "error  in file '%s' line '%d'", l_filename, l_line, HERE);
  log::info  ("module2.submodule2", "info   in file '%s' line '%d'", l_filename, l_line, HERE);
  log::debug ("module2.submodule2", "debug  in file '%s' line '%d'", l_filename, l_line, HERE);
  CPPUNIT_ASSERT_EQUAL(true, true);
}

XTD_TEST_MAIN();
