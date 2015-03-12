#include "Base.hh"


namespace xtd {
namespace counters {

Base::Base(const string& p_name) :
  m_name(p_name)
{
}

Base::~Base(void)
{
}


void
Base::visit(Visitor& p_visitor) const
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  visit_safe(p_visitor);
}

void
Base::update(void)
{
  boost::mutex::scoped_lock l_lock(m_mutex);
  update_safe();
}

void
Base::update_safe(void)
{
}

const string&
Base::getName(void) const
{
  return m_name;
}

}}
