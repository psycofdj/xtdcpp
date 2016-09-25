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

