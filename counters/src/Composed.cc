#include "Composed.hh"
#include <boost/bind.hpp>
#include <boost/core/ref.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include "types.hh" // libcore

namespace xtd { namespace counters { class Visitor; } }
namespace xtd {
namespace counters {

Composed::Composed(const string& p_name) :
  Base(p_name)
{
}

void
Composed::addItem(Base& p_item)
{
  if (0 != m_name.size())
    p_item.m_name = m_name + "." + p_item.m_name;
  m_items.push_back(&p_item);
}

void
Composed::visit_safe(Visitor& p_visitor) const
{
  boost::for_each(m_items, boost::bind(&Base::visit, _1, boost::ref(p_visitor)));
}

void
Composed::update_safe(void)
{
  boost::for_each(m_items, boost::bind(&Base::update, _1));
}

}}
