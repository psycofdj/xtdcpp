#include "Value.hh"
#include "Value.hxx"


namespace xtd {
namespace counters {


Atomic::Atomic(void) :
  m_mutex()
{
}

void
Atomic::inc(uint32_t& p_val)
{
  atom::atomic_inc32(&p_val);
}

void
Atomic::dec(uint32_t& p_val)
{
  atom::atomic_dec32(&p_val);
}

void
Atomic::write(uint32_t& p_val, const uint32_t& p_src)
{
  atom::atomic_write32(&p_val, p_src);
}

template class Value<uint32_t>;
template class Value<uint64_t>;

}}
