#ifndef COMMON_CONFPARSER_HH
# define COMMON_CONFPARSER_HH

# include <string>
# include <sstream>
# include <map>
# include <vector>
# include <utility> // needed for pair<>
# include <iostream> // must be before the next one to prevent a bug with KCC
# include <cstring>
# include <boost/bind.hpp>
# include <algorithm>
# include <boost/shared_ptr.hpp>
# include "types.hh"


namespace xtd {
/**
 ** this class read a file whose format is describe in ???
 **  information can then be recovered via use of get() member
 **  function
 */
class ConfParser
{
private:
  typedef map<string, string>       t_value_map;
  typedef std::pair<string, string> t_section_data;
  typedef vector<t_section_data >   t_section_list;

public:
  typedef map<string, string>           t_param_map;
  typedef boost::shared_ptr<ConfParser> t_sptr;

  class const_iterator
  {
  private:
    typedef std::pair<const char*, const char*> t_result;

  public:
    inline const_iterator(void);
    inline const_iterator(const const_iterator& p_obj);
    inline const_iterator(const t_section_list::const_iterator& p_iter);
    inline const_iterator& operator=(const const_iterator& p_obj);

  public:
    inline void               operator++(void);
    inline void               operator++(int);
    inline bool               operator==(const const_iterator& p_obj) const;
    inline bool               operator!=(const const_iterator& p_obj) const;
    inline const t_result     operator*(void) const;
    inline const t_result     operator->(void) const;

  private:
    t_section_list::const_iterator m_iter;
  };

public:
  ConfParser(const string& p_path);
  ConfParser(const string& p_path, const t_param_map& p_params);

public:
  inline const string&      getFilePath(void) const;
  inline const_iterator     find(const char* p_name) const;
  inline const_iterator     find(const string& p_key) const;
  inline const_iterator     end(void) const;
  inline const_iterator     begin(void) const;

public:
  status get(const string& p_name, string&        p_dval) const;
  status get(const string& p_name, const char*&        p_dval) const;
  status get(const string& p_name, char&               p_ival) const;
  status get(const string& p_name, unsigned char&      p_ival) const;
  status get(const string& p_name, unsigned short&     p_ival) const;
  status get(const string& p_name, int&                p_ival) const;
  status get(const string& p_name, long&               p_ival) const;
  status get(const string& p_name, unsigned long long& p_ival) const;
  status get(const string& p_name, unsigned long&      p_ival) const;
  status get(const string& p_name, unsigned int&       p_ival) const;
  status get(const string& p_name, double&             p_dval) const;
  status get(const string& p_name, float&              p_dval) const;
  status get(const string& p_name, bool&               p_dval) const;

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

  /**
   ** @brief Indique si la clef p_name existe dans le fichier de conf
   ** @param p_name clef à chercher
   ** @return vrai si p_name existe, faux sinon
   */
  bool exists(const string& p_name) const;

  /**
   ** dump all keys found in conf file
   ** @param p_stream output stream
   */
  void dumpKeys(std::ostream& p_stream) const;

  const t_param_map& getParamMap(void) const;

private:
  status initialize(const string& p_fileName);
  status parseSection(char* p_line,
                      int&  p_readOffset,
                      char  p_prefix[255]);
  status parseFile(char* p_line);
  status loadFile(const string& p_filename);
  status evaluateVal(const string& p_keyName,
                     char*         p_pLine,
                     char*         p_dest,
                     std::size_t   p_maxChar);

public:
  inline const t_value_map& getValueMap(void) const;

protected:
  t_value_map    m_values;
  t_section_list m_sections;
  int            m_nblines;
  string         m_filename;
  t_param_map    m_paramMap;
};


const ConfParser::t_value_map&
ConfParser::getValueMap(void) const
{
  return m_values;
}


ConfParser::const_iterator::const_iterator(void)
{
}

ConfParser::const_iterator::const_iterator(const const_iterator& p_obj)
{
  m_iter = p_obj.m_iter;
}

ConfParser::const_iterator::const_iterator(const t_section_list::const_iterator& p_iter)
{
  m_iter = p_iter;
}

ConfParser::const_iterator&
ConfParser::const_iterator::operator=(const const_iterator& p_obj)
{
  m_iter = p_obj.m_iter;
  return *this;
}


void
ConfParser::const_iterator::operator++(int)
{
  m_iter++;
}

void
ConfParser::const_iterator::operator++(void)
{
  ++m_iter;
}

const ConfParser::const_iterator::t_result
ConfParser::const_iterator::operator*(void) const
{
  return std::make_pair(m_iter->first.c_str(),
                        m_iter->second.c_str());
}


const ConfParser::const_iterator::t_result
ConfParser::const_iterator::operator->(void) const
{
  return *(*this);
}


bool
ConfParser::const_iterator::operator==(const const_iterator& p_obj) const
{
  return m_iter == p_obj.m_iter;
}

bool
ConfParser::const_iterator::operator!=(const const_iterator& p_obj) const
{
  return ! operator==(p_obj);
}


const string&
ConfParser::getFilePath(void) const
{
  return m_filename;
}

ConfParser::const_iterator
ConfParser::find(const string& p_key) const
{
  const_iterator cc_result =
    std::find_if(m_sections.begin(), m_sections.end(),
                 boost::bind(&t_section_data::first, _1) == p_key);
  return cc_result;
}

ConfParser::const_iterator
ConfParser::find(const char * const p_name) const
{
  return find(string(p_name));
}

ConfParser::const_iterator
ConfParser::begin(void) const
{
  return const_iterator(m_sections.begin());
}

ConfParser::const_iterator
ConfParser::end(void) const
{
  return const_iterator(m_sections.end());
}

}

#endif // !COMMON_CONF_HH
