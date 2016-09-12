include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  )

add_shared_static_library(common
  src/logger.cc
  src/text.cc
  src/Application.cc
  src/ConfParser.cc)


add_doc(common
  EXCLUDE  src/json_parser.hpp src/json_parser_write.hpp
  WERROR)
add_doc_coverage(common)
add_cloc(common)
add_cppcheck(common)
add_check(common
  ARGS     -p -o compiler
  INCLUDES ${common_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    common_s tests_s ${Boost_LIBRARIES})
add_cov(common)
