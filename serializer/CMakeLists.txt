include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  ${serializer_INCLUDE_DIRS}
)

add_shared_static_library(serializer
  src/objects/Doc.cc
  src/serializer.cc
)
