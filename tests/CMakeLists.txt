include_directories(
  ${Boost_INCLUDE_DIRS}
  ${core_INCLUDE_DIRS}
  ${Cppunit_INCLUDE_DIR}
  )

add_shared_static_library(xtd-tests
  src/MainTestApplication.cc
  src/ProgressListener.cc
  src/TestFixture.cc
)

add_shared_static_library(xtd-cwrap
  src/CWrap.cc
)


add_doc(tests)
add_doc_coverage(tests)
add_cloc(tests)
add_cppcheck(tests)
add_check(tests
  ARGS     -p -o compiler
  INCLUDES ${core_INCLUDE_DIRS} ${tests_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    xtd-core_s xtd-tests_s ${Boost_LIBRARIES})
add_cov(tests)
add_memcheck(tests)

