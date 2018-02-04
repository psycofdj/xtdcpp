#include "param/Visitor.hh"
#include <http/Template.hh>

namespace xtd {
namespace servers {
namespace param {

JsonVisitor::JsonVisitor(network::http::Json& p_tmpl) :
  Visitor(),
  m_tmpl(p_tmpl)
{
}

void JsonVisitor::operator()(const POD<bool>&     p_obj) { write(p_obj); }
void JsonVisitor::operator()(const POD<int>&      p_obj) { write(p_obj); }
void JsonVisitor::operator()(const POD<uint8_t>&  p_obj) { write(p_obj); }
void JsonVisitor::operator()(const POD<uint32_t>& p_obj) { write(p_obj); }
void JsonVisitor::operator()(const POD<uint64_t>& p_obj) { write(p_obj); }
void JsonVisitor::operator()(const POD<string>&   p_obj) { write(p_obj); }

void
JsonVisitor::writeExtraInfo(const string& p_key, const string& p_value)
{
  m_tmpl.add(p_key, "", p_value);
}

}}}
