#ifndef COUNTERS_VALUE_HXX_
# define COUNTERS_VALUE_HXX_
# include <boost/interprocess/sync/spin/condition.hpp>
# include <boost/thread.hpp>
# include "types.hh" // libcore
# include "Base.hh"
# include "Value.hh"
# include "Visitor.hh" // IWYU pragma: keep

namespace atom = boost::interprocess::ipcdetail;
namespace xtd {
namespace counters {

template<typename TType>
void
Atomic::inc(TType& p_val)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  ++p_val;
}

template<typename TType>
void
Atomic::dec(TType& p_val)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  --p_val;
}

template<typename TType>
void
Atomic::write(TType& p_val, const TType& p_src)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  p_val = p_src;
}


template<typename TType>
Value<TType>::Value(const string& p_name) :
  Base(p_name),
  m_isNaN(false),
  m_value(0),
  m_atom()
{
}

template<typename TType>
Value<TType>&
Value<TType>::NaN(void)
{
  m_isNaN = true;
  return *this;
}


template<typename TType>
bool
Value<TType>::isNaN(void) const
{
  return m_isNaN;
}

template<typename TType>
const TType&
Value<TType>::getValue(void) const
{
  return m_value;
}

template<typename TType>
Value<TType>&
Value<TType>::operator=(const TType& p_value)
{
  m_isNaN = false;
  m_atom.write(m_value, p_value);
  return *this;
}

template<typename TType>
Value<TType>&
Value<TType>::operator++(void)
{
  m_atom.inc(m_value);
  return *this;
}

template<typename TType>
Value<TType>&
Value<TType>::operator++(int)
{
  m_atom.inc(m_value);
  return *this;
}

template<typename TType>
Value<TType>&
Value<TType>::operator--(void)
{
  m_atom.dec(m_value);
  return *this;
}

template<typename TType>
Value<TType>&
Value<TType>::operator--(int)
{
  m_atom.dec(m_value);
  return *this;
}

template<typename TType>
void
Value<TType>::visit_safe(Visitor& p_visitor) const
{
  if (true == m_isNaN)
    p_visitor(m_name, "NaN");
  else
    p_visitor(m_name, m_value);
}

}}

#endif // !COUNTERS_VALUE_HXX_
