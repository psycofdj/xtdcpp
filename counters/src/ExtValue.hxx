#ifndef COUNTERS_EXTVALUE_HXX_
# define COUNTERS_EXTVALUE_HXX_

# include "ExtValue.hh"




namespace xtd {
namespace counters {


template<typename TType>
ExtValue<TType>::ExtValue(const string& p_name,
                          const TType&  p_value) :
  Base(p_name),
  m_value(p_value)
{
}

template<typename TType>
void
ExtValue<TType>::visit_safe(Visitor& p_visitor) const
{
  p_visitor(m_name, m_value);
}



}}


#endif // !COUNTERS_EXTVALUE_HXX_
