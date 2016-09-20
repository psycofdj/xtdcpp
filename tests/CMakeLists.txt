include_directories(
  ${Boost_INCLUDE_DIRS}
  ${core_INCLUDE_DIRS}
  ${Cppunit_INCLUDE_DIR}
  )

add_shared_static_library(tests
  src/MainTestApplication.cc
  src/ProgressListener.cc
)

add_shared_static_library(cwrap
  src/CWrap.cc
)

