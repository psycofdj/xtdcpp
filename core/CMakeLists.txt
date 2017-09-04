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
  src/log/FormatModifiers.cc
  src/tty.cc
  src/text.cc
  src/Application.cc
  src/config/Parser.cc
  src/config/Grammar.cc
  )

add_doc(core
  EXCLUDE
  src/json_parser.hpp
  src/json_parser_write.hpp
  PREDEFINED
  "XTD_CORE_LOG_REGISTER_FORMATTER"
  "XTD_CORE_LOG_REGISTER_APPENDER"
  "DOXYGEN_SKIP"
  EXPAND_AS_DEFINED
  "xtd_error_class(p_name,p_base)")

add_doc_coverage(core)
add_cloc(core)
add_cppcheck(core)
add_codedup(core)
add_check(core
  INCLUDES ${core_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    cwrap tests_s core_s boost_regex stdc++ )
add_cov(core)
add_memcheck(core)
add_iwyu(core
  COMMENTS
  DEPENDS core_s)

