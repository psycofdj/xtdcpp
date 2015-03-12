#include "text.hh"
#include <boost/assign.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace xtd {
namespace text {

xml::t_data xml::ms_entityList = xml::initialize();

xml::t_data
xml::initialize(void)
{
  return boost::assign::tuple_list_of
    ("&",    "&",  "&amp;")    // & (must be first)
    ("\"",   "\"", "&quot;")   // "
    ("'",    "'",  "&apos;")   // '
    ("<",    "<",  "&lt;")     // <
    (">",    ">",  "&gt;");    // >
}


void
xml::encode(string& p_str)
{
  t_data::const_iterator c_tuple;

  for (c_tuple  = ms_entityList.begin();
       c_tuple != ms_entityList.end();
       c_tuple++)
    boost::replace_all(p_str, c_tuple->get<0>(), c_tuple->get<2>());
}

string
xml::encode_copy(const string& p_str)
{
  string l_value = p_str;
  encode(l_value);
  return l_value;
}


}}
