#ifndef COUNTERS_SUMEXT_HXX_
# define COUNTERS_SUMEXT_HXX_

#include "SumExt.hh"
#include <boost/foreach.hpp>





namespace xtd {
namespace counters {

template<typename TType>
SumExt<TType>::SumExt(const string& p_name) :
  Value<TType>(p_name),
  m_values()
{
}

template<typename TType>
void
SumExt<TType>::addItem(const TType& p_val)
{
  m_values.push_back(&p_val);
  Value<TType>::m_isNaN = false;
}

template<typename TType>
void
SumExt<TType>::update_safe(void)
{
  TType l_val = 0;

  BOOST_FOREACH(const typename t_items::value_type& c_val, m_values)
    l_val += *c_val;

  Value<TType>::m_value = l_val;
}


}}

#endif // !COUNTERS_SUMEXT_HXX_
