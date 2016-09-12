include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  ${serializer_INCLUDE_DIRS}
)

add_shared_static_library(serializer
  src/objects/Doc.cc
  src/serializer.cc
)

add_doc(serializer
  EXCLUDE src/objects/unorderedmap.hh
  WERROR)
add_doc_coverage(serializer)
add_cloc(serializer)
add_cppcheck(serializer)
add_check(serializer
  ARGS     -p -o compiler
  INCLUDES ${common_INCLUDE_DIRS} ${serializer_INCLUDE_DIRS}${tests_INCLUDE_DIRS}
  LINKS    common_s tests_s serializer_s ${Boost_LIBRARIES})
add_cov(serializer)
