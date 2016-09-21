#include <log.hh>

int main(int,char**)
{
  size_t l_ret = 0;
  auto   l_appender  = std::make_shared<log::StreamAppender>(cout);
  auto   l_formatter = std::make_shared<log::Formatter>();
  auto   l_root      = log::getRoot();

  l_formatter->setLayouts({
      { "fulllog",  "level=%(slevel), module=%(module), message=%(message) at %(location)" },
      { "location", "file=%(file), line=%(line) in func %(function)"                       }});
  l_formatter->setFieldFormats("slevel", "[% 10s]");
  l_appender->setFormatter(l_formatter);
  l_root->addAppender(l_appender);

  log::crit("module1", "process return code is %d", l_ret);
  // prints to std::cout :
  // level=[       err], module=module1, message=process return code is 0, at file=formatter.cc, line=19 in func int main(int,char**)

  return 0;
}
