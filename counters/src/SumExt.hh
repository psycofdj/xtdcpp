#ifndef COUNTERS_SUMEXT_HH_
# define COUNTERS_SUMEXT_HH_
# include <boost/core/addressof.hpp>
# include "Value.hh"
# include "types.hh" // libcore


namespace xtd {
namespace counters {

template<typename TType>
class SumExt : public Value<TType>
{
  friend class Composed;

private:
  typedef vector<const TType*> t_items;

public:
  SumExt(const string& p_name);
  virtual ~SumExt(void) {};

public:
  void addItem(const TType& p_val);

protected:
  virtual void update_safe(void);

private:
  t_items m_values;
};

typedef SumExt<uint32_t>   SumExt32;
typedef SumExt<uint64_t>   SumExt64;

}}

#endif // !COUNTERS_SUMEXT_HH_
