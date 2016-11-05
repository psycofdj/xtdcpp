cmake_minimum_required (VERSION 2.6)
project (xtdcpp)

include(xtdmake/loader.cmake)

set(REQUIRED "REQUIRED")
if (NOT CI)
  set(REQUIRED "")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

set(CMAKE_CXX_COMPILER         "g++")
set(CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -std=gnu++11 -W -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Werror")


find_package(StaticShared REQUIRED)
find_package(Boost REQUIRED thread filesystem system regex)
find_package(Threads REQUIRED)
find_package(DocRule ${REQUIRED})
find_package(DocCoverageRule ${REQUIRED})
find_package(CppcheckRule ${REQUIRED})
find_package(ClocRule ${REQUIRED})
find_package(Tracking ${REQUIRED})
find_package(Cppunit ${REQUIRED})
find_package(CheckRule ${REQUIRED})
find_package(CovRule ${REQUIRED})
find_package(MemcheckRule ${REQUIRED})
find_package(Reports ${REQUIRED})


# configure test default parameters
if (CheckRule_FOUND)
  list(APPEND CheckRule_DEFAULT_ARGS     "--srcdir=\${CMAKE_CURRENT_SOURCE_DIR}")
  list(APPEND CheckRule_DEFAULT_ARGS     "--top-srcdir=\${PROJECT_SOURCE_DIR}")
  list(APPEND CheckRule_DEFAULT_ARGS     "--top-builddir=\${PROJECT_BINARY_DIR}")
  list(APPEND CheckRule_DEFAULT_ARGS     "--testdir=\${CheckRule_DIRECTORY}")
  list(APPEND CheckRule_DEFAULT_ARGS     "--outputter=compiler")
  list(APPEND CheckRule_DEFAULT_ARGS     "-p")
  list(APPEND CheckRule_DEFAULT_ARGS     "-e")
  list(APPEND CheckRule_DEFAULT_ARGS     "7")
  list(APPEND CheckRule_DEFAULT_INCLUDES "${Boost_INCLUDE_DIRS}")
  list(APPEND CheckRule_DEFAULT_LINKS    "${Boost_LIBRARIES}")
endif()

set(core_INCLUDE_DIRS       ${PROJECT_SOURCE_DIR}/core/src)
set(counters_INCLUDE_DIRS   ${PROJECT_SOURCE_DIR}/counters/src)
set(serializer_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/serializer/src)
set(network_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/network/src)
set(servers_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/servers/src)
set(tests_INCLUDE_DIRS      ${PROJECT_SOURCE_DIR}/tests/src)

subdirs(core tests counters serializer network servers)
