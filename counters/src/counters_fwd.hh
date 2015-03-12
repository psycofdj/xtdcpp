#ifndef COUNTERS_LIBCOUNTERS_FWD_HH_
# define COUNTERS_LIBCOUNTERS_FWD_HH_


namespace xtd {
namespace counters {

class Base;
template<typename TType> class Value;
template<typename TType> class ExtValue;
template<typename TType> class AvgValue;
template<typename TType> class SumExt;
template<typename TType> class AvgExt;
class Freq;
class InstantFreq;
class AvgTimedValue;
class SumAvgTimedValue;
class Perf;
class Cache;

class JsonVisitor;
class FileVisitor;
class CounterManager;

}}

#endif // !COUNTERS_LIBCOUNTERS_FWD_HH_
