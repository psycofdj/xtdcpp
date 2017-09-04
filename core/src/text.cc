#include "text.hh"
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/assign/list_of.hpp>
#include <exception>
#include <iterator>
#include <regex>
#include "format.hh"

namespace xtd {
namespace text {

vector<string>
algorithm::regex::split_copy(const string& p_input, const std::regex& p_re)
{
  vector<string> l_res;

  split(l_res, p_input, p_re);
  return l_res;
}

void
algorithm::regex::split(vector<string>& p_output, const string& p_input, const std::regex& p_re)
{
  std::sregex_token_iterator c_start(p_input.begin(), p_input.end(), p_re, -1);
  std::sregex_token_iterator c_end;
  std::copy(c_start, c_end, std::back_inserter(p_output));
}


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
  string l_value;
  size_t l_pos = 0;
  long   l_hex;

  while (l_pos < p_value.size())
  {
    char l_char = p_value[l_pos];
    if ((l_char == '%') && ((l_pos + 2) < p_value.size()))
    {
      try {
        string::const_iterator c_start = p_value.begin() + l_pos + 1;
        string::const_iterator c_end   = c_start + 1;
        l_hex    = std::stoul(string(c_start, c_end), 0, 16);
        l_value += static_cast<char>(l_hex);
        l_pos   += 3;
        continue;
      }
      catch (std::exception&) { }
    }

    if (l_char == '+')
      l_value += ' ';
    else
      l_value += l_char;
    l_pos++;
  }

  return l_value;
}


}}
