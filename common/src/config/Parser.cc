#include <types.hh> // libcommon
#include "config/Grammar.hh"

namespace xtd {
namespace config {

class Parser
{
  // public:
  //   typedef Grammar::error_info error_info;

public:
  Parser(void) {};
  Parser(const string& pFilename) {};
  Parser(const ifstream& pFilename) {};

public:
  // void  parse(const std::string& p_content) throw()
  // {
  // }


  status parse(const std::string& p_content// , error_info& p_error
               )
  {
    types::iterator l_begin(p_content.begin());
    types::iterator l_end(p_content.end());
    types::iterator l_cur(l_begin);
    Grammar<types::iterator, boost::spirit::ascii::space_type>         l_grammar;
    status          l_status;
    bool            l_ret;

    l_ret = phrase_parse(l_cur, l_end, l_grammar, boost::spirit::ascii::space, m_sections);
    if ((false == l_ret) || (l_cur != l_end))
    {
      // p_error = l_grammar.getLastError();
      return status::error;
    }
    return status::ok;
  }

private:
  sections m_sections;
};

}}
