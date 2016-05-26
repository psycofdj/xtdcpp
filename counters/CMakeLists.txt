include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/common/src)

add_library(counters
  ./src/Base.cc
  ./src/Value.cc
  ./src/AvgValue.cc
  ./src/AvgTimedValue.cc
  ./src/ExtValue.cc
  ./src/SumExt.cc
  ./src/Freq.cc
  ./src/InstantFreq.cc
  ./src/Perf.cc
  ./src/Composed.cc
  ./src/Cache.cc
  ./src/CounterManager.cc
)
