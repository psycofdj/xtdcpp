include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  ${Cppunit_INCLUDE_DIR}
  )


add_shared_static_library(tests
  src/MainTestApplication.cc
  src/ProgressListener.cc
)

add_shared_static_library(libc_test_wrapper
  src/libc_wrapper.cc
)

