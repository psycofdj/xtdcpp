include_directories(${Boost_INCLUDE_DIRS})

set(common_INCLUDE_DIRS test/fdsfds/)
add_library(common
  src/logger.cc
  src/text.cc
  src/Application.cc
  src/ConfParser.cc)

install(TARGETS common DESTINATION xtdcpp/lib)
install(
  FILES
    src/Application.hh
    src/Application.hxx
    src/ConfParser.hh
    src/error.hh
    src/json_parser.hpp
    src/json_parser_write.hpp
    src/logger.hh
    src/logger.hxx
    src/text.hh
    src/types.hh
  DESTINATION
    xtdcpp/include/common)

add_executable(test src/main.cc)
target_link_libraries(test common ${Boost_LIBRARIES})
