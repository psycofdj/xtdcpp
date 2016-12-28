# ----------
# cmake init
# ----------

cmake_minimum_required (VERSION 2.6)
project (xtdcpp)
enable_testing()

# ---------
# libraries
# ---------

find_package(Boost REQUIRED thread filesystem system regex)
find_package(Threads REQUIRED)
add_definitions(-DBOOST_MAJOR_VERSION=${Boost_MAJOR_VERSION})
add_definitions(-DBOOST_MINOR_VERSION=${Boost_MINOR_VERSION})

# ------------
# xtdmake init
# ------------

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/xtdmake/src")
set(REQUIRED "REQUIRED")
if (NOT CI)
  set(REQUIRED "")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

find_package(XTDMake REQUIRED)

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

xtdmake_init(
  StaticShared    REQUIRED
  DocRule         ${REQUIRED}
  DocCoverageRule ${REQUIRED}
  CppcheckRule    ${REQUIRED}
  CheckRule       ${REQUIRED}
  ClocRule        ${REQUIRED}
  Tracking        ${REQUIRED}
  Cppunit         ${REQUIRED}
  CovRule         ${REQUIRED}
  MemcheckRule    ${REQUIRED}
  Reports         ${REQUIRED})

# --------------------
# compiler options
# --------------------

xtdmake_find_program(Gxx
  NAMES g++
  DOC "Gnu C++ compiler"
  URL "https://gcc.gnu.org"
  VERSION_OPT "-dumpversion"
  VERSION_POS "0"
  REQUIRED 1)

if (Gxx_VERSION VERSION_LESS 4.7)
  message(FATAL_ERROR "xtdcpp requires g++ version >= 4.7 for -std=c++11 option")
endif()

set(CMAKE_CXX_COMPILER         "g++")
set(CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -std=gnu++11 -W -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Werror")


# ------------
# local config
# ------------

set(core_INCLUDE_DIRS       ${PROJECT_SOURCE_DIR}/core/src)
set(counters_INCLUDE_DIRS   ${PROJECT_SOURCE_DIR}/counters/src)
set(serializer_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/serializer/src)
set(network_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/network/src)
set(servers_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/servers/src)
set(tests_INCLUDE_DIRS      ${PROJECT_SOURCE_DIR}/tests/src)

subdirs(core tests counters serializer network servers)
