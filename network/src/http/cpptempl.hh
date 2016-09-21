/*
  cpptempl
  =================
  This is a template engine for C++.

  Syntax
  =================
  Variables: {$variable_name}
  Loops: {% for person in people %}Name: {$person.name}{% endfor %}
  If: {% for person.name == "Bob" %}Full name: Robert{% endif %}

  Copyright
  ==================
  Author: Ryan Ginstrom
  MIT License

  Usage
  =======================
  string text = "{% if item %}{$item}{% endif %}\n"
  "{% if thing %}{$thing}{% endif %}" ;
  cpptempl::data_map data ;
  data["item"] = cpptempl::make_data("aaa") ;
  data["thing"] = cpptempl::make_data("bbb") ;

  string result = cpptempl::parse(text, data) ;

  Handy Functions
  ========================
  make_data() : Feed it a string, data_map, or data_list to create a data entry.
  Example:
  data_map person ;
  person["name"] = make_data("Bob") ;
  person["occupation"] = make_data("Plumber") ;
  data_map data ;
  data["person"] = make_data(person) ;
  string result = parse(templ_text, data) ;

*/
#ifndef NETWORK_HTTP_CPPTEMPL_HH_
# define NETWORK_HTTP_CPPTEMPL_HH_

#include <exception>
#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <types.hh> //libcore

namespace xtd {
namespace network {

namespace http {
namespace cpptempl {

// various typedefs
class Data;
typedef std::shared_ptr<Data>   data_ptr;
typedef vector<data_ptr>          data_list;
typedef map<string, data_ptr>     data_map;

class Token;
typedef std::shared_ptr<Token>  token_ptr;
typedef vector<token_ptr>         token_vector;


// Custom exception class for library errors
class TemplateException : public std::exception
{
public:
  TemplateException(string reason) :
    m_message(reason)
  { }
  ~TemplateException() throw()
  { }

public:
  const char* what(void) const throw()
  {
    return m_message.c_str();
  }
private:
  string m_message;
};


// Data types used in templates
class Data
{
public:
  virtual ~Data(void) {}
  virtual bool        empty() = 0;
  virtual string getvalue();
  virtual data_list&  getlist();
  virtual data_map&   getmap();
};


class DataValue : public Data
{
  string m_value;
public:
  DataValue(string value) : m_value(value){}
  string getvalue();
  bool empty();
};


class DataList : public Data
{
  data_list m_items;
public:
  DataList(data_list &items) : m_items(items){}
  data_list& getlist();
  bool empty();
};


class DataMap : public Data
{
  data_map m_items;
public:
  DataMap(data_map &items) : m_items(items){}
  data_map& getmap();
  bool empty();
};


// convenience functions for making data objects
inline data_ptr make_data(string p_val)
{
  return data_ptr(new DataValue(p_val));
}

inline data_ptr make_data(data_list& p_val)
{
  return data_ptr(new DataList(p_val));
}

inline data_ptr make_data(data_map& p_val)
{
  return data_ptr(new DataMap(p_val));
}

template<typename T>
inline data_ptr make_data(T& p_val)
{
  return make_data(boost::lexical_cast<string>(p_val));
}

template<typename T>
inline data_ptr make_data(vector<T>& p_val)
{
  data_list                         l_list;
  typename vector<T>::iterator c_val;

  for (c_val = p_val.begin(); c_val != p_val.end(); c_val++)
    l_list.push_back(make_data(*c_val));
  return make_data(l_list);
}

template<typename T>
inline data_ptr make_data(map<string, T>& p_val)
{
  data_map                                    l_map;
  typename map<string, T>::iterator c_val;

  for (c_val = p_val.begin(); c_val != p_val.end(); c_val++)
    l_map[c_val->first] = make_data(c_val->second);

  return make_data(l_map);
}

// get a data value from a data map
// e.g. foo.bar => data["foo"]["bar"]
data_ptr parse_val(string key, data_map &data);

typedef enum
  {
    TOKEN_TYPE_NONE,
    TOKEN_TYPE_TEXT,
    TOKEN_TYPE_VAR,
    TOKEN_TYPE_IF,
    TOKEN_TYPE_FOR,
    TOKEN_TYPE_ENDIF,
    TOKEN_TYPE_ENDFOR,
  } TokenType;


// Template tokens
class Token
{
public:
  virtual ~Token(void) {}
  virtual TokenType gettype() = 0;
  virtual string gettext(data_map &data) = 0;
  virtual void set_children(token_vector &children);
  virtual token_vector & get_children();
};


class TokenText : public Token
{
  string m_text;
public:
  TokenText(string text) : m_text(text){}
  TokenType gettype();
  string gettext(data_map &data);
};

class TokenVar : public Token
{
  string m_key;
public:
  TokenVar(string key) : m_key(key){}
  TokenType gettype();
  string gettext(data_map &data);
};

class TokenFor : public Token
{
public:
  string m_key;
  string m_val;
  token_vector m_children;
  TokenFor(string expr);
  TokenType gettype();
  string gettext(data_map &data);
  void set_children(token_vector &children);
  token_vector &get_children();
};

class TokenIf : public Token
{
public:
  string m_expr;
  token_vector m_children;
  TokenIf(string expr) : m_expr(expr){}
  TokenType gettype();
  string gettext(data_map &data);
  bool is_true(string expr, data_map &data);
  void set_children(token_vector &children);
  token_vector &get_children();
};

class TokenEnd : public Token // end of control block
{
  string m_type;
public:
  TokenEnd(string text) : m_type(text){}
  TokenType gettype();
  string gettext(data_map &data);
};

void          parse_tree(token_vector &tokens, token_vector &tree, TokenType until=TOKEN_TYPE_NONE);
token_vector& tokenize(string text, token_vector &tokens);

// The big daddy. Pass in the template and data,
// and get out a completed doc.
string parse(string templ_text, data_map &data);

}}}} //end namespaces

#endif // !NETWORK_HTTP_CPPTEMPL_HH_
