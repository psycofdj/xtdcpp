#include <boost/type_index.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <types.hh> // libcommon
#include "config/types.hh"
#include "config/Grammar.hh"
#include "error.hh"
#include "logger.hh"


namespace xtd {
namespace config {

REGISTER_ERROR(parse_error, xtd::error);

class Parser
{
public:
  typedef map<string,string> t_data;

public:
  Parser(void) {};
  Parser(const std::string& pFilename) {};
  Parser(const std::ifstream& pFilename) {};

public:
  template<typename T>
  status get(const string& p_name, T& p_val) const
  {
    t_data::const_iterator c_item;
    if (m_data.end() == (c_item = m_data.find(p_name)))
    {
      logger::info("common.config", "key '%s' not found", p_name, HERE);
      return status::notfound;
    }

    try {
      p_val = boost::lexical_cast<T>(c_item->second);
    } catch (boost::bad_lexical_cast&) {
      logger::info("common.config", "key '%s' found but not lexicaly castable to %s",
                   p_name,  boost::typeindex::type_id<T>().pretty_name(), HERE);
      return status::error;
    }
    return status::ok;
  }

  template<typename T>
  status get_default(const string& p_key, T& p_dest, const T& p_default)
  {
    if (status::ok != get(p_key, p_dest))
    {
      p_dest = p_default;
      return status::notfound;
    }
    return status::ok;
  }

  status get(const string& p_name, string& p_dval) const
  {
    t_data::const_iterator c_item;
    if (m_data.end() == (c_item = m_data.find(p_name)))
    {
      logger::info("common.config", "key '%s' not found", p_name, HERE);
      return status::notfound;
    }
    p_dval = c_item->second;
  }

  status get(const string& p_name, const char*& p_dval) const
  {
    t_data::const_iterator c_item;
    if (m_data.end() == (c_item = m_data.find(p_name)))
    {
      logger::info("common.config", "key '%s' not found", p_name, HERE);
      return status::notfound;
    }
    p_dval = c_item->second.c_str();
  }

  status get(const string& p_name, bool& p_dval) const
  {
    static const vector<string> ls_true({"yes", "true", "1", "on"});
    static const vector<string> ls_false({"no", "false", "0", "off"});
    status                      l_ret = status::ok;

    t_data::const_iterator c_item;
    if (m_data.end() == (c_item = m_data.find(p_name)))
    {
      logger::info("common.config", "key '%s' not found", p_name, HERE);
      return status::notfound;
    }

    auto l_functor = [&c_item](const std::string& p_item) {
      return boost::is_iequal()(c_item->second, p_item);
    };

    if (ls_true.end() != boost::find_if(ls_true, l_functor))
      p_dval = true;
    else if (ls_false.end() != boost::find_if(ls_false, l_functor))
      p_dval = false;
    else {
      logger::info("common.config", "key '%s' found but not lexicaly castable to bool", p_name, HERE);
      l_ret = status::error;
    }
    return l_ret;
  }

public:
  template<typename Iterator>
  status parse(Iterator p_begin, Iterator p_end)
  {
    typedef Grammar<Iterator> TGrammar;

    auto       l_cur  = TGrammar::wrap(p_begin);
    auto       l_end = TGrammar::wrap(p_end);
    TGrammar   l_grammar;
    error_info l_error;
    bool       l_ret;

    l_ret = phrase_parse(l_cur, l_end, l_grammar, boost::spirit::ascii::blank, m_sections);
    if ((false == l_ret) || (l_cur != l_end))
    {
      l_error = l_grammar.getLastError();
      logger::err("common.config", "parse error at (line %d, column %d). expecting token '%s' near '%s'",
                  l_error.line, l_error.col, l_error.token, l_error.preview, HERE);
      return status::error;
    }

    return translate();
  }

  void dump(void)
  {
    for (auto& c_sec : m_sections)
      for (auto& c_ssec : c_sec.m_subsections)
        for (auto& c_prop : c_ssec.m_properties)
        {
          std::cout << c_sec.m_name
                    << ":"
                    << c_ssec.m_name
                    << ":"
                    << c_prop.m_name
                    << "="
                    << c_prop.m_value
                    << std::endl;

          for (auto& c_var : c_prop.m_vars)
            std::cout << "(var)[" << c_var << "]" << std::endl;
          for (auto& c_var : c_prop.m_envs)
            std::cout << "(env)[" << c_var << "]" << std::endl;
          for (auto& c_var : c_prop.m_params)
            std::cout << "(params)[" << c_var << "]" << std::endl;
        }

    for (auto& c_sec : m_sections)
      for (auto& c_prop : c_sec.m_properties)
      {
        std::cout << c_sec.m_name
                  << ":"
                  << c_prop.m_name
                  << "="
                  << c_prop.m_value
                  << std::endl;

        for (auto& c_var : c_prop.m_vars)
          std::cout << "(var)[" << c_var << "]" << std::endl;
        for (auto& c_var : c_prop.m_envs)
          std::cout << "(env)[" << c_var << "]" << std::endl;
        for (auto& c_var : c_prop.m_params)
          std::cout << "(params)[" << c_var << "]" << std::endl;
      }
  }
  status translate(void)
  {
    string l_name;

    for (auto& c_sec : m_sections)
    {
      for (auto& c_ssec : c_sec.m_subsections)
      {
        for (auto& c_prop : c_ssec.m_properties)
        {
          string l_name = c_sec.m_name + ":" + c_ssec.m_name + ":" + c_prop.m_name;
          if (0 != m_data.count(l_name))
          {
            logger::err("common.config", "multiply defined key '%s'", l_name, HERE);
            return status::error;
          }
          m_data[l_name] = c_prop.m_value;
        }
      }

      for (auto& c_prop : c_sec.m_properties)
      {
        string l_name = c_sec.m_name + ":" + c_prop.m_name;
        if (0 != m_data.count(l_name))
        {
          logger::err("common.config", "multiply defined key '%s'", l_name, HERE);
          return status::error;
        }
        m_data[l_name] = c_prop.m_value;
      }
    }

    return status::ok;
  }


public:
  status test(void)
  {
    std::string   l_data;
    std::ifstream l_file("toto");
    error_info    l_error;

    std::string::const_iterator c_iter = l_data.begin();
    std::string::const_iterator c_iter2 = l_data.end();
    parse(std::istream_iterator<char>(l_file), std::istream_iterator<char>());
    parse(c_iter, c_iter2);
    parse(l_data.begin(), l_data.end());
    return status::ok;
  }

private:
  vector<section>     m_sections;
  map<string, string> m_data;
};

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
