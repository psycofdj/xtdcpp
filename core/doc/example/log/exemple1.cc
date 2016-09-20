#include <log.hh>

using namespace xtd;

int main(int,char**)
{
  std::shared_ptr<log::StreamAppender> l_stream =
    std::make_shared<log::StreamAppender>(std::cerr);

  std::shared_ptr<log::Formatter> l_format =
    std::make_shared<log::Formatter>();

  l_format->setLayouts({
      { "fulllog",  "%(name) [%(pid)] [%(ppid)] %(time) [%(threadid)] [%(module)] (%(slevel)) %(message) %(location)" },
      { "location", "at %(filename):%(line) -> %(function)"                                                           },
      { "time",     "%d-%m-%Y %H:%M:%s"                                                                               }
    });

  l_stream->setFormatter(l_format);

  log::getRoot()
    .addAppender(l_stream)
    .setLevel(log::level::info);

  // default log interface
  log::crit("module1",           "critical message");
  log::err ("module1",           "error message");
  log::info("module1.submodule", "info message");

  // specifying sub-logger paramters
  log::getRoot().setLevelTo("module1.submodule", log::level::error);
  log::getRoot().get("module1.submodule.submodule").setLevel(log::level::debug);

  // variadic args message formats
  log::info("module2", "with plain old char* arg : %s", "value");
  log::info("module2", "with string arg : %s",          std::string("value"));
  log::info("module2", "with multiple args : %.2f, %s", 0.87, string("value"));

  // location informations to log records
  log::info("module1", "this log is triggered with location data", HERE);

  return 0;
}
