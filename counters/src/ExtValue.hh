#ifndef COUNTERS_EXTVALUE_HH_
# define COUNTERS_EXTVALUE_HH_
# include <string>
# include "Base.hh"


namespace xtd {
namespace counters {

/** */

template<typename TType>
class ExtValue : public Base
{
  friend class Composed;

public:
  ExtValue(const string& p_name, const TType& p_value);
  virtual ~ExtValue(void) {};

protected:
  void visit_safe(Visitor& p_visitor) const;

private:
  const TType& m_value;
};

typedef ExtValue<uint32_t> ExtValue32;
typedef ExtValue<uint64_t> ExtValue64;

}}

#endif // !COUNTERS_EXTVALUE_HH_
