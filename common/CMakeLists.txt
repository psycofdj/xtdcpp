include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  )

add_shared_static_library(common
  src/logger.cc
  src/text.cc
  src/Application.cc
  src/ConfParser.cc)

add_doxygen(common
  EXCLUDE     src/json_parser.hpp src/json_parser_write.hpp
  WERROR)

add_executable(common_tf  src/main.cc)
add_executable(common_tf2 src/main.cc)

target_link_libraries(common_tf  common_s ${Boost_LIBRARIES})
target_link_libraries(common_tf2
  common
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_THREAD_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${Boost_SYSTEM_LIBRARIES}
  )
