#ifndef XTD_COMMON_CONFIG_PARSER_HH_
# define XTD_COMMON_CONFIG_PARSER_HH_
# include <types.hh> // libcommon

namespace xtd {
namespace config { class section;  }
namespace config { class property; }
namespace config {

class Parser
{
public:
  typedef map<string,string>                                   t_data;
  typedef map<string,property>                                 t_refs;

  typedef pair<t_data::const_iterator, t_data::const_iterator> t_result;

public:
  Parser(void);
  Parser(const std::string& pFilename);
  Parser(const std::ifstream& pFilename);

public:
  status get(const string& p_name, string& p_dval) const;
  status get(const string& p_name, const char*& p_dval) const;
  status get(const string& p_name, bool& p_dval) const;
  template<typename T>
  status get(const string& p_name, T& p_val) const;
  template<typename T>
  status get_default(const string& p_key, T& p_dest, const T& p_default) const;

public:
  template<typename Iterator>
  status parse(Iterator p_begin, Iterator p_end);

  t_result search(const string& p_item) const;

private:
  status translate(vector<section>& p_sections,
                   t_data&          p_data) const;

  status resolve_ext(property& p_sections) const;
  status resolve_refs(t_refs& p_refs, t_data& p_data) const;
  std::string extractKey(const string& p_item) const;

private:
  t_data m_data;
  t_data m_params;
};

}}

# include "config/Parser.hxx"

#endif //!XTD_COMMON_CONFIG_PARSER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
