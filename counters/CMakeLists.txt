include_directories(
  ${Boost_INCLUDE_DIRS}
  ${core_INCLUDE_DIRS}
  ${counters_INCLUDE_DIRS}
  )


add_shared_static_library(counters
  src/Base.cc
  src/Value.cc
  src/AvgValue.cc
  src/AvgTimedValue.cc
  src/ExtValue.cc
  src/SumExt.cc
  src/Freq.cc
  src/InstantFreq.cc
  src/Perf.cc
  src/Composed.cc
  src/Cache.cc
  src/CounterManager.cc
)


add_doc(counters)
add_doc_coverage(counters)
add_cloc(counters)
add_cppcheck(counters)
add_check(counters
  ARGS     -p -o compiler
  INCLUDES ${core_INCLUDE_DIRS} ${counters_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    core_s tests_s counters_s ${Boost_LIBRARIES})
add_cov(counters)
add_memcheck(counters)
