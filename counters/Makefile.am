include $(XTDMAKE_DIR)/make/include.mk

noinst_LIBRARIES    = libcounters.a

AM_CXXFLAGS =                           \
  -std=c++11                            \
  -I$(srcdir)/src                       \
  -I$(XTD_INCLUDE)/common/src        \
  -I$(BOOST_INCLUDE)

libcounters_a_SOURCES =         \
  ./src/Base.cc                 \
  ./src/Value.cc                \
  ./src/AvgValue.cc             \
  ./src/AvgTimedValue.cc        \
  ./src/ExtValue.cc             \
  ./src/SumExt.cc               \
  ./src/Freq.cc                 \
  ./src/InstantFreq.cc          \
  ./src/Perf.cc                 \
  ./src/Composed.cc             \
  ./src/Cache.cc                \
  ./src/CounterManager.cc
