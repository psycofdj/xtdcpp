#include "cpptempl.hh"
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace xtd {
namespace network {
namespace http {
namespace cpptempl {

//////////////////////////////////////////////////////////////////////////
// Data classes
//////////////////////////////////////////////////////////////////////////
string Data::getvalue()
{
  throw TemplateException("Data item is not a value");
}

data_list& Data::getlist()
{
  throw TemplateException("Data item is not a list");
}
data_map& Data::getmap()
{
  throw TemplateException("Data item is not a dictionary");
}
string DataValue::getvalue()
{
  return m_value;
}

bool DataValue::empty()
{
  return m_value.empty();
}
data_list& DataList::getlist()
{
  return m_items;
}

bool DataList::empty()
{
  return m_items.empty();
}
data_map& DataMap:: getmap()
{
  return m_items;
}

bool DataMap::empty()
{
  return m_items.empty();
}
//////////////////////////////////////////////////////////////////////////
// parse_val
//////////////////////////////////////////////////////////////////////////
data_ptr parse_val(string key, data_map &data)
{
  data_map::const_iterator cc_value;

  // quoted string
  if (key[0] == L'\"')
  {
    return make_data(boost::trim_copy_if(key, boost::is_any_of("\"")));
  }
  // check for dotted notation, i.e [foo.bar]
  size_t index = key.find(".");
  if (index == string::npos)
  {
    if (data.end() == (cc_value = data.find(key)))
      throw TemplateException(boost::str(boost::format("unresolved key : %s") % key));
    return cc_value->second;
  }

  if (data.end() == (cc_value = data.find(key.substr(0, index))))
    throw TemplateException(boost::str(boost::format("unresolved key : %s") % key.substr(0, index)));

  data_ptr item = cc_value->second;
  return parse_val(key.substr(index+1), item->getmap());
}

//////////////////////////////////////////////////////////////////////////
// Token classes
//////////////////////////////////////////////////////////////////////////

void
Token::set_children(token_vector& /* children */)
{
  throw TemplateException("This token type cannot have children");
}

token_vector&
Token::get_children(void)
{
  throw TemplateException("This token type cannot have children");
}
TokenType TokenText::gettype()
{
  return TOKEN_TYPE_TEXT;
}
string TokenText::gettext(data_map & )
{
  return m_text;
}

TokenType TokenVar::gettype()
{
  return TOKEN_TYPE_VAR;
}
string TokenVar::gettext(data_map &data )
{
  return parse_val(m_key, data)->getvalue();
}

TokenFor::TokenFor(string expr)
{
  vector<string> elements;
  boost::split(elements, expr, boost::is_space(), boost::token_compress_on);
  if (elements.size() != 4u)
  {
    throw TemplateException("Invalid syntax in for statement");
  }
  m_val = elements[1];
  m_key = elements[3];
}

TokenType TokenFor::gettype()
{
  return TOKEN_TYPE_FOR;
}

string TokenFor::gettext(data_map &data )
{
  vector<string> elements;
  data_ptr value = parse_val(m_key, data);
  data_list &items = value->getlist();
  for (size_t i = 0; i < items.size(); ++i)
  {
    data["loop"] = make_data(boost::lexical_cast<string>(i+1));
    data["loop0"] = make_data(boost::lexical_cast<string>(i));
    data[m_val] = items[i];
    for(size_t j = 0; j < m_children.size(); ++j)
    {
      elements.push_back(m_children[j]->gettext(data));
    }
  }
  return boost::join(elements, "");
}

void TokenFor::set_children( token_vector &children )
{
  m_children.clear();
  std::copy(children.begin(), children.end(), std::back_inserter(m_children));
}

token_vector & TokenFor::get_children()
{
  return m_children;
}
TokenType TokenIf::gettype()
{
  return TOKEN_TYPE_IF;
}

string TokenIf::gettext(data_map& data)
{
  vector<string> elements;
  if (is_true(m_expr, data))
  {
    for(size_t j = 0; j < m_children.size(); ++j)
    {
      elements.push_back(m_children[j]->gettext(data));
    }
  }
  return boost::join(elements, "");
}

bool TokenIf::is_true( string expr, data_map &data )
{
  vector<string> elements;
  boost::split(elements, expr, boost::is_space(), boost::token_compress_on);

  if (elements[1] == "def")
  {
    try {
      parse_val(elements[2], data);
    }
    catch (TemplateException&) {
      return false;
    }
    return true;
  }

  if (elements[1] == "ndef")
  {
    try {
      parse_val(elements[2], data);
    }
    catch (TemplateException&) {
      return true;
    }
    return false;
  }

  if (elements[1] == "not")
  {
    return parse_val(elements[2], data)->empty();
  }
  if (elements.size() == 2)
  {
    return !parse_val(elements[1], data)->empty();
  }
  data_ptr lhs = parse_val(elements[1], data);
  data_ptr rhs = parse_val(elements[3], data);
  if (elements[2] == "==")
  {
    return lhs->getvalue() == rhs->getvalue();
  }
  return lhs->getvalue() != rhs->getvalue();
}

void TokenIf::set_children(token_vector& children)
{
  m_children.clear();
  std::copy(children.begin(), children.end(), std::back_inserter(m_children));
}

token_vector& TokenIf::get_children()
{
  return m_children;
}

TokenType TokenEnd::gettype()
{
  return m_type == "endfor" ? TOKEN_TYPE_ENDFOR : TOKEN_TYPE_ENDIF;
}

string TokenEnd::gettext(data_map& /* data */ )
{
  return "";
}

//////////////////////////////////////////////////////////////////////////
// parse_tree
//////////////////////////////////////////////////////////////////////////
void parse_tree(token_vector &tokens, token_vector &tree, TokenType until)
{
  while(! tokens.empty())
  {
    token_ptr token = tokens[0];
    tokens.erase(tokens.begin());
    if (token->gettype() == TOKEN_TYPE_FOR)
    {
      token_vector children;
      parse_tree(tokens, children, TOKEN_TYPE_ENDFOR);
      token->set_children(children);
    }
    else if (token->gettype() == TOKEN_TYPE_IF)
    {
      token_vector children;
      parse_tree(tokens, children, TOKEN_TYPE_ENDIF);
      token->set_children(children);
    }
    else if (token->gettype() == until)
    {
      return;
    }
    tree.push_back(token);
  }
}
//////////////////////////////////////////////////////////////////////////
// tokenize
//////////////////////////////////////////////////////////////////////////
token_vector & tokenize(string text, token_vector &tokens)
{
  while(true)
  {
    size_t pos = text.find("{");
    if (pos == string::npos)
    {
      if (! text.empty())
      {
        tokens.push_back(token_ptr(new TokenText(text)));
      }
      return tokens;
    }
    string pre_text = text.substr(0, pos);
    if (! pre_text.empty())
    {
      tokens.push_back(token_ptr(new TokenText(pre_text)));
    }
    text = text.substr(pos+1);
    if (text.empty())
    {
      tokens.push_back(token_ptr(new TokenText("{")));
      return tokens;
    }

    // variable
    if (text[0] == L'$')
    {
      pos = text.find("}");
      if (pos != string::npos)
      {
        tokens.push_back(token_ptr (new TokenVar(text.substr(1, pos-1))));
        text = text.substr(pos+1);
      }
    }
    // control statement
    else if (text[0] == L'%')
    {
      pos = text.find("}");
      if (pos != string::npos)
      {
        string expression = boost::trim_copy(text.substr(1, pos-2));
        text = text.substr(pos+1);
        if (boost::starts_with(expression, "for"))
        {
          tokens.push_back(token_ptr (new TokenFor(expression)));
        }
        else if (boost::starts_with(expression, "if"))
        {
          tokens.push_back(token_ptr (new TokenIf(expression)));
        }
        else
        {
          tokens.push_back(token_ptr (new TokenEnd(boost::trim_copy(expression))));
        }
      }
    }
    else
    {
      tokens.push_back(token_ptr(new TokenText("{")));
    }
  }
  return tokens;
}

//////////////////////////////////////////////////////////////////////////
// parse
//////////////////////////////////////////////////////////////////////////
string parse(string templ_text, data_map &data)
{
  token_vector tokens;
  tokenize(templ_text, tokens);
  token_vector tree;
  parse_tree(tokens, tree);
  vector<string> nodes;
  for (size_t i = 0; i < tree.size(); ++i)
  {
    nodes.push_back(tree[i]->gettext(data));
  }

  return boost::join(nodes, "");
}


}}}} //end namespaces
