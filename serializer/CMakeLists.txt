include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/common/src
)

add_library(serializer
  src/objects/Doc.cc
  src/serializer.cc
)
