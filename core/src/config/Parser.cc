#include "config/Parser.hh"
#include <stdlib.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <fstream>
#include <iterator>
#include "config/Parser.hxx"
#include "config/types.hh"
#include "log/helpers.hxx"
#include "log/logtypes.hh"

namespace xtd {
namespace config {


Parser::Parser(const t_data& p_params) :
  m_data(),
  m_params(p_params)
{
}

Parser::Parser(const string& p_file,
               const t_data& p_params) :
  Parser(p_params)
{
  if (status::ok != parse(p_file))
    log::raise<parse_error>("core.config", "unable to parse file '%s'", p_file, HERE);
}

Parser::Parser(istream&      p_file,
               const t_data& p_params) :
  Parser(p_params)

{
  if (status::ok != parse(p_file))
    log::raise<parse_error>("core.config", "unable to parse stream", HERE);
}


status
Parser::get(const string& p_name, string& p_dval) const
{
  t_data::const_iterator c_item;
  if (m_data.end() == (c_item = m_data.find(p_name)))
  {
    log::err("core.config", "key '%s' not found", p_name, HERE);
    return status::notfound;
  }
  p_dval = c_item->second;
  return status::ok;
}

status
Parser::get(const string& p_name, const char*& p_dval) const
{
  t_data::const_iterator c_item;
  if (m_data.end() == (c_item = m_data.find(p_name)))
  {
    log::err("core.config", "key '%s' not found", p_name, HERE);
    return status::notfound;
  }
  p_dval = c_item->second.c_str();
  return status::ok;
}

status
Parser::get(const string& p_name, bool& p_dval) const
{
  static const vector<string> ls_true({"yes", "true", "1", "on"});
  static const vector<string> ls_false({"no", "false", "0", "off"});
  status                      l_ret = status::ok;

  t_data::const_iterator c_item;
  if (m_data.end() == (c_item = m_data.find(p_name)))
  {
    log::err("core.config", "key '%s' not found", p_name, HERE);
    return status::notfound;
  }

  auto l_pred = [&c_item](const string& p_bool) {
    return boost::iequals(p_bool, c_item->second);
  };

  if (ls_true.end() != boost::find_if(ls_true, l_pred))
    p_dval = true;
  else if (ls_false.end() != boost::find_if(ls_false, l_pred))
    p_dval = false;
  else {
    log::err("core.config", "key '%s' found but not lexicaly castable to bool", p_name, HERE);
    l_ret = status::error;
  }
  return l_ret;
}

Parser::t_result
Parser::search(const string& p_item) const
{
  t_data::const_iterator c_start = m_data.lower_bound(p_item);
  t_data::const_iterator c_stop  = c_start;

  while ((m_data.end() != c_stop) &&
         (true == boost::starts_with(c_stop->first, p_item)))
    c_stop++;
  return make_pair(c_start, c_stop);
}

status
Parser::translate(vector<impl::section>& p_sections,
                  t_data&                p_data) const
{
  string l_name;
  t_refs l_refs;

  for (auto& c_sec : p_sections)
  {
    for (auto& c_ssec : c_sec.m_subsections)
    {
      for (auto& c_prop : c_ssec.m_properties)
      {
        string l_name = c_sec.m_name + "." + c_ssec.m_name + "." + c_prop.m_name;

        if (0 != m_data.count(l_name))
        {
          log::err("core.config", "multiply defined key '%s'", l_name, HERE);
          return status::error;
        }

        if (status::ok != resolveExt(c_prop))
          return status::error;

        p_data[l_name] = c_prop.m_value;
        if (false == c_prop.m_vars.empty())
          l_refs[l_name] = c_prop;
      }
    }

    for (auto& c_prop : c_sec.m_properties)
    {
      string l_name = c_sec.m_name + "." + c_prop.m_name;
      if (0 != m_data.count(l_name))
      {
        log::err("core.config", "multiply defined key '%s'", l_name, HERE);
        return status::error;
      }
      if (status::ok != resolveExt(c_prop))
        return status::error;

      p_data[l_name] = c_prop.m_value;
      if (false == c_prop.m_vars.empty())
        l_refs[l_name] = c_prop;
    }
  }

  return resolveRefs(l_refs, p_data);
}

string
Parser::extractKey(const string& p_item) const
{
  size_t c_start = p_item.find_first_of('{');
  size_t c_end   = p_item.find_first_of('}');
  return string(p_item, c_start + 1, c_end - (c_start + 1));
}

status
Parser::resolveExt(impl::property& p_prop) const
{
  for (auto& c_item : p_prop.m_envs)
  {
    char*  l_val;
    string l_key = extractKey(c_item);
    if (0 == (l_val = getenv(l_key.c_str())))
    {
      log::err("core.config", "unresolved environment variable $ENV{%s}", l_key, HERE);
      return status::error;
    }
    boost::replace_all(p_prop.m_value, "$ENV{" + l_key + "}", string(l_val));
  }

  for (auto& c_item : p_prop.m_params)
  {
    string                 l_key = extractKey(c_item);
    t_data::const_iterator c_param;

    if (m_params.end() == (c_param = m_params.find(l_key)))
    {
      log::err("core.config", "unresolved parameter $PARAM{%s}", l_key, HERE);
      return status::error;
    }
    boost::replace_all(p_prop.m_value, "$PARAM{" + l_key + "}", c_param->second);
  }

  return status::ok;
}

status
Parser::resolveRefs(t_refs& p_refs, t_data& p_data) const
{
  size_t l_count = p_refs.size();

  while (l_count != 0)
  {
    l_count = 0;
    for (auto& c_ref_ref : p_refs)
    {
      vector<string>   l_addRefs;
      t_data::iterator c_ref_data = p_data.find(c_ref_ref.first);
      for (auto& c_var : c_ref_ref.second.m_vars)
      {
        string                 l_refKey      = extractKey(c_var);
        t_refs::const_iterator c_target_ref  = p_refs.find(l_refKey);
        t_data::const_iterator c_target_data = p_data.find(l_refKey);

        if (p_data.end() == c_target_data)
        {
          log::err("core.config", "undefined reference to key '%s'", l_refKey, HERE);
          return status::error;
        }

        if (l_refKey == c_ref_data->first)
        {
          log::err("core.config", "circular reference detected to key '%s'", c_ref_data->first, HERE);
          return status::error;
        }

        if (c_target_ref != p_refs.end())
          boost::copy(c_target_ref->second.m_vars, std::back_inserter(l_addRefs));
        boost::replace_all(c_ref_data->second, c_var, c_target_data->second);
      }
      c_ref_ref.second.m_vars.swap(l_addRefs);
      l_count += c_ref_ref.second.m_vars.size();
    }
  }

  return status::ok;
}

status
Parser::parse(istream& p_stream)
{
  if (false == p_stream.good())
  {
    log::err("core.config", "bad input stream", HERE);
    return status::error;
  }

  p_stream >> std::noskipws;
  return parse(std::istream_iterator<char>(p_stream), std::istream_iterator<char>());
}


status
Parser::parse(const string& p_file)
{
  ifstream l_file(p_file.c_str());

  if (false == l_file.is_open())
  {
    log::err("core.config", "unable to open file '%s'", p_file, HERE);
    return status::error;
  }

  return parse(l_file);
}


}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
