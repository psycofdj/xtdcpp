#include "Request.hh"
#include <ext/alloc_traits.h>
#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assign/std/vector.hpp>
#include <limits>
#include "types.hh"        // libcore
#include "text.hh"         // libcore
#include "format.hh"       // libcore
#include "log/helpers.hh"  // libcore
#include "log/logtypes.hh" // libcore


using boost::assign::operator+=;


namespace xtd {
namespace network {
namespace http {

Request::Request(void) :
  Message(),
  m_rawUrl(),
  m_method(method::get),
  m_path(),
  m_cgi(),
  m_post(),
  m_regex_tooManyStartingSlash("\\/{2,}"),
  m_regex_tooManyEndingSlash("[^\\/](\\/+)$")
{
}

void
Request::setPath(const string& p_path)
{
  m_path = p_path;
}

void
Request::setMethod(method p_method)
{
  m_method = p_method;
}


/**
 ** @details
 ** 1. on effecture une etape de nornmalisation de la ressource demandee
 */
void
Request::normalizePath(const string& p_path)
{
  std::smatch l_match;

  m_path = text::url::decode_copy(p_path);
  m_path = std::regex_replace(m_path, std::regex("^/+"), "/");
  m_path = std::regex_replace(m_path, std::regex("/+$"), "/");
}


void
Request::addCgi(const string& p_key,
                const string& p_value)
{
  m_cgi.insert(make_pair(p_key, p_value));
}

void
Request::addPost(const string& p_key,
                 const string& p_value)
{
  m_post.insert(make_pair(p_key, p_value));
}



void
Request::removeCgi(const string& p_key)
{
  m_cgi.erase(p_key);
}


void
Request::removePost(const string& p_key)
{
  m_post.erase(p_key);
}


void
Request::removeData(const string& p_key)
{
  removeCgi(p_key);
  removePost(p_key);
}


method
Request::getMethod(void) const
{
  return m_method;
}


string
Request::getQS(void) const
{
  vector<string> l_parts;

  for (const t_param_map::value_type& c_param : m_cgi)
    l_parts += c_param.first + "=" + c_param.second;
  return boost::join(l_parts, "&");
}

const string&
Request::getPath(void) const
{
  return m_path;
}

const Request::t_param_map&
Request::getCgis(void) const
{
  return m_cgi;
}

const Request::t_param_map&
Request::getPosts(void) const
{
  return m_post;
}

Request::t_param_map
Request::getParams(void) const
{
  t_param_map l_map;

  l_map.insert(m_cgi.begin(), m_cgi.end());
  l_map.insert(m_post.begin(), m_post.end());
  return l_map;
}

bool
Request::existsData(const t_param_map& p_src, const string& p_key) const
{
  return (p_src.count(p_key) != 0);
}

bool
Request::existsCgi(const string& p_key) const
{
  return existsData(m_cgi, p_key);
}

bool
Request::existsPost(const string& p_key) const
{
  return existsData(m_post, p_key);
}

bool
Request::existsParam(const string& p_key) const
{
  return existsData(m_cgi, p_key) || existsData(m_post, p_key);
}

void
Request::writeInitial(std::ostream& p_stream) const
{
  p_stream <<
    format::vargs("%s %s HTTP/%s\r\n",
                  str(m_method),
                  m_path,
                  str(m_version));
}

status
Request::readInitial(istream& p_request)
{
  string         l_line;
  vector<string> l_parts;

  getline(p_request, l_line);
  boost::trim_if(l_line, boost::is_any_of(" \n\r\t"));

  if (0 == l_line.size())
  {
    log::err("network.http.request", "initial line error : no initial line", HERE);
    return status::error;
  }

  boost::split(l_parts, l_line, boost::is_any_of(" "));
  if (3 != l_parts.size())
  {
    log::err("network.http.request", "initial line error : invalid initial line format '%s'", l_line, HERE);
    return status::error;
  }

  if (status::ok != readMethod(l_parts[0]))
  {
    log::err("network.http.request", "initial line error : unknown method '%s'", l_parts[0], HERE);
    return status::error;
  }

  if (status::ok != readUrl(l_parts[1]))
  {
    log::err("network.http.request", "initial line error : invalid url '%s'", l_parts[1], HERE);
    return status::error;
  }

  if (status::ok != readVersion(l_parts[2]))
  {
    log::err("network.http.request", "initial line error : invalid version '%s'", l_parts[2], HERE);
    return status::error;
  }

  return status::ok;
}


status
Request::readMethod(const string& p_method)
{
  string l_value = boost::to_lower_copy(p_method);

  if (l_value == "get")
    setMethod(method::get);
  else if (l_value == "post")
    setMethod(method::post);
  else if (l_value == "head")
    setMethod(method::head);
  else if (l_value == "patch")
    setMethod(method::patch);
  else if (l_value == "put")
    setMethod(method::put);
  else if (l_value == "delete")
    setMethod(method::del);
  else
  {
    log::err("network.http.request", "initial line error : unknown method '%s'", l_value, HERE);
    return status::error;
  }
  return status::ok;
}




status
Request::readUrl(const string& p_url)
{
  vector<string>                 l_cgiPairs;
  vector<string>                 l_urlParts;
  vector<string>                 l_pathParts;
  vector<string>::const_iterator cc_cgiPair;
  string                         l_query;

  if (0 == p_url.size())
  {
    log::err("network.http.request", "url error : invalid 0 size", HERE);
    return status::error;
  }
  string::size_type l_pos = p_url.find("?");
  string l_path           = p_url;

  if (string::npos != l_pos)
  {
    l_query  = l_path.substr(l_pos + 1);
    l_path = l_path.substr(0, l_pos);

    boost::split(l_cgiPairs, l_query, boost::is_any_of("&"));
    for (cc_cgiPair = l_cgiPairs.begin(); cc_cgiPair != l_cgiPairs.end(); cc_cgiPair++)
    {
      vector<string> l_cgi;
      string         l_key;
      string         l_value;

      if (cc_cgiPair->size() == 0)
        continue;
      boost::split(l_cgi, *cc_cgiPair, boost::is_any_of("="));
      l_key = l_cgi[0];
      if (l_cgi.size() == 2)
        l_value = l_cgi[1];
      else if (l_cgi.size() > 2)
      {
        for (uint32_t c_cgi = 1; c_cgi < l_cgi.size() - 1; c_cgi++)
        {
          l_value += l_cgi[c_cgi] + "%3D"; // %3D represents =
        }
        l_value += l_cgi[l_cgi.size() - 1];
      }
      addCgi(l_key, text::url::decode_copy(l_value));
    }
  }

  normalizePath(l_path);
  m_rawUrl = p_url;
  return status::ok;
}


string
Request::getRawUrl(void) const
{
  return m_rawUrl;
}


status
Request::readData(istream& p_request)
{
  string l_contentType;

  if (status::ok != Message::readData(p_request))
    return status::error;

  if (m_data.size() == 0)
    return status::ok;

  if (false == getHeader("Content-Type", l_contentType))
  {
    log::err("network.http.request", "read data : unable to get Content-Type", HERE);
    return status::error;
  }

  if (boost::starts_with(l_contentType, "application/x-www-form-urlencoded"))
  {
    return readDataFormEncoded(m_data);
  }
  else
  {
    log::err("network.http.request", "read data : unknown content type '%s'", l_contentType, HERE);
    return status::error;
  }
}


status
Request::readDataFormEncoded(const string& p_data)
{
  vector<string>                 l_postPairs;
  vector<string>::const_iterator cc_postPair;

  boost::split(l_postPairs, p_data, boost::is_any_of("&"));
  for (cc_postPair = l_postPairs.begin(); cc_postPair != l_postPairs.end(); cc_postPair++)
  {
    vector<string> l_post;
    string         l_key;
    string         l_value;

    if (cc_postPair->size() == 0)
      continue;
    boost::split(l_post, *cc_postPair, boost::is_any_of("="));
    l_key = l_post[0];
    if (l_post.size() == 2)
      l_value = l_post[1];
    else if (l_post.size() > 2)
    {
      log::err("network.http.request", "read data form : invalid post", HERE);
      return status::error;
    }

    addPost(l_key, text::url::decode_copy(l_value));
  }
  return status::ok;
}


bool
Request::as(const string& p_src, string& p_dst) const
{
  p_dst = p_src;
  return true;
}

bool
Request::as(const string& p_src, bool& p_dst) const
{
  vector<string> l_valuesYes;
  vector<string> l_valuesNo;

  l_valuesYes += "1", "on",  "oui", "true",  "yes";
  l_valuesNo  += "0", "off", "non", "false", "no";

  if (l_valuesYes.end() != std::find(l_valuesYes.begin(), l_valuesYes.end(), p_src))
  {
    p_dst = true;
    return true;
  }
  if (l_valuesNo.end() != std::find(l_valuesNo.begin(), l_valuesNo.end(), p_src))
  {
    p_dst = false;
    return true;
  }
  return false;
}


bool
Request::as(const string& p_src, uint8_t& p_dst) const
{
  uint64_t l_value;

  if ((false == as(p_src, l_value)) ||
      (l_value > std::numeric_limits<uint8_t>::max()))
    return false;

  p_dst = l_value;
  return true;
}




ostream& operator<<(ostream& p_buf, const Request& p_obj)
{
  Request::t_param_map::const_iterator  cc_cgi;
  Request::t_param_map::const_iterator  cc_post;
  Request::t_header_map::const_iterator cc_header;

  p_buf << "method : " << str(p_obj.getMethod()) << endl;
  p_buf << "path : " << p_obj.getPath() << endl;
  p_buf << "version : " << str(p_obj.getVersion()) << endl;
  p_buf << "headers :" << endl;
  for (cc_header = p_obj.getHeaders().begin();
       cc_header != p_obj.getHeaders().end();
       cc_header++)
    p_buf << " -> " << cc_header->first << ": " << cc_header->second << endl;

  p_buf << "cgi list :" << endl;
  for (cc_cgi = p_obj.getCgis().begin();
       cc_cgi != p_obj.getCgis().end();
       cc_cgi++)
    p_buf << " -> " << cc_cgi->first << " = " << cc_cgi->second << endl;
  p_buf << "post list :" << endl;
  for (cc_post = p_obj.getPosts().begin();
       cc_post != p_obj.getPosts().end();
       cc_post++)
    p_buf << " -> " << cc_post->first << " = " << cc_post->second << endl;

  return p_buf;
}

}}} //end namspaces
