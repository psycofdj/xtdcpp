#include "text.hh"
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/regex.hpp>
#include "types.hh"
#include "format.hh"

namespace xtd {
namespace text {

xml::t_data xml::ms_entityList = xml::initialize();

xml::t_data
xml::initialize(void)
{
  return boost::assign::tuple_list_of
    ("&",  "&amp;")    // & (must be first)
    ("\"", "&quot;")   // "
    ("'",  "&apos;")   // '
    ("<",  "&lt;")     // <
    (">",  "&gt;");    // >
}


void
xml::encode(string& p_str)
{
  t_data::const_iterator c_tuple;

  for (auto& c_tuple : ms_entityList)
    boost::replace_all(p_str, c_tuple.get<0>(), c_tuple.get<1>());
}

string
xml::encode_copy(const string& p_str)
{
  string l_value = p_str;
  encode(l_value);
  return l_value;
}



template<typename T>
string
url::hex_to_string::operator()(const T& p_match) const
{
  stringstream  l_value;
  int           l_intVal;
  unsigned char l_result;
  l_value << std::hex << p_match.match_results().str(1);
  l_value >> l_intVal;
  l_result = l_intVal;
  return format::vargs("%c", l_result);
}


string
url::decode_copy(const string& p_value)
{
  string       l_value(p_value);
  boost::regex l_hexRegex("%([0-9a-fA-F]{2})");
  boost::regex l_spaceRegex("[ \t]{2,}");

  // replace all + by space
  boost::replace_all(l_value, "+", " ");

  // replace hex value by corresponding character
  boost::smatch l_match;
  boost::algorithm::find_format_all(l_value,
                                    boost::algorithm::regex_finder(l_hexRegex),
                                    hex_to_string());

  // remove duplicate spaces
  boost::algorithm::find_format_all(l_value,
                                    boost::algorithm::regex_finder(l_spaceRegex),
                                    boost::algorithm::const_formatter(" "));
  boost::trim(l_value);

  return l_value;
}


}}
