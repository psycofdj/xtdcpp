cmake_minimum_required (VERSION 2.6)
project (xtdcpp)

include(xtdmake/loader.cmake)
find_package(StaticShared REQUIRED)
find_package(Doc REQUIRED)
find_package(DocCoverage REQUIRED)
find_package(Cppcheck REQUIRED)
find_package(Cloc REQUIRED)
find_package(Reports REQUIRED)
find_package(Tracking REQUIRED)

find_package(Boost REQUIRED thread filesystem system)
find_package(Threads REQUIRED)

set(common_INCLUDE_DIRS     ${PROJECT_SOURCE_DIR}/common/src)
set(counters_INCLUDE_DIRS   ${PROJECT_SOURCE_DIR}/counters/src)
set(serializer_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/serializer/src)
set(network_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/network/src)
set(servers_INCLUDE_DIRS    ${PROJECT_SOURCE_DIR}/servers/src)

subdirs(common counters serializer network servers)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++11 -fprofile-arcs -ftest-coverage")
