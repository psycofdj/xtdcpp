#include "logger.hh"
#include "Application.hh"

using namespace xtd;


class App : Application
{
  int process(void)
  {
    string   l_filename = "/tmp/conf/test.conf";
    uint32_t l_line  = 324;

    logger::get().initialize(m_binName, logger::level::crit);

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
    return 0;
  }
};


int main(int pArgc, char** pArgv)
{
  char **l_ptr = 0;
  *l_ptr = "toto";
  std::cout << **l_ptr << std::endl;
  Application lApp;
  return lApp.execute(pArgc, pArgv);
}
