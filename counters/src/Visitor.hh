#ifndef COUNTERS_VISITOR_HH_
# define COUNTERS_VISITOR_HH_

# include <types.hh> // libcore

namespace xtd {
namespace counters {

class Visitor
{
public:
  virtual void operator()(const string&, const string&) = 0;
  virtual void operator()(const string&, uint8_t)        = 0;
  virtual void operator()(const string&, uint16_t)       = 0;
  virtual void operator()(const string&, uint32_t)       = 0;
  virtual void operator()(const string&, uint64_t)       = 0;
};

}}

#endif // !COUNTERS_VISITOR_HH_
