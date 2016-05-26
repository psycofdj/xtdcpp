cmake_minimum_required (VERSION 2.6)
project (xtd)
find_package(Boost)
subdirs(common counters serializer network)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS -std=c++11)
