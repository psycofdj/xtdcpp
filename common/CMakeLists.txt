include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  )

add_shared_static_library(common
  src/types.cc
  src/logger.cc
  src/text.cc
  src/Application.cc
  src/config/Parser.cc
  src/config/Grammar.cc)

add_doc(common
  EXCLUDE  src/json_parser.hpp src/json_parser_write.hpp
  WERROR)
add_doc_coverage(common)
add_cloc(common)
add_cppcheck(common)
add_check(common
  INCLUDES ${common_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    common_s tests_s cwrap)
add_cov(common)
