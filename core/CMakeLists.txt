include_directories(
  ${Boost_INCLUDE_DIRS}
  ${core_INCLUDE_DIRS}
  )

add_shared_static_library(core
  src/types.cc
  src/log/Appender.cc
  src/log/ColoredFormatter.cc
  src/log/ConfLoader.cc
  src/log/Formatter.cc
  src/log/helpers.cc
  src/log/Logger.cc
  src/log/MemoryAppender.cc
  src/log/StreamAppender.cc
  src/log/Stream.cc
  src/log/SyslogAppender.cc
  src/tty.cc
  src/text.cc
  src/Application.cc
  src/config/Parser.cc
  src/config/Grammar.cc)

add_doc(core
  EXCLUDE  src/json_parser.hpp src/json_parser_write.hpp
  WERROR)
add_doc_coverage(core)
add_cloc(core)
add_cppcheck(core)
add_check(core
  INCLUDES ${core_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    core_s tests_s cwrap)
add_cov(core)
