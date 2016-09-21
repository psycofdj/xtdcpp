#include "Request.hh"
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>
#include <istream>
#include <iterator>
#include <boost/format.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>
#include <log.hh> // libcore


using boost::assign::operator+=;


namespace xtd {
namespace network {
namespace http {

Request::Request(bool p_cgiToLower) :
  m_rawUrl(),
  m_method(METHOD_GET),
  m_path(),
  m_cgi(),
  m_post(),
  m_headers(),
  m_versionStr("1.0"),
  m_version(Request::VERSION_1_0),
  m_cgiToLower(p_cgiToLower),
  m_regex_tooManyStartingSlash("\\/{2,}"),
  m_regex_tooManyEndingSlash("[^\\/](\\/+)$")
{
}

status
Request::readQS(const string& p_qs)
{
  return readUrl(p_qs);
}

void
Request::setMethod(t_method p_method)
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
  boost::smatch l_match;


  m_path = paramValueDecode(p_path);
  m_path = boost::regex_replace(m_path, boost::regex("^/+"), "/");
  m_path = boost::regex_replace(m_path, boost::regex("/+$"), "/");

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
Request::addHeader(const string& p_name,
                   const string& p_value)
{
  m_headers[p_name] = p_value;
}

void
Request::setVersion(const string& p_version)
{
  m_versionStr = p_version;
}

void
Request::setVersion(t_version p_version)
{
  m_version = p_version;
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


Request::t_method
Request::getMethod(void) const
{
  return m_method;
}


string
Request::getQS(void) const
{
  vector<string> l_parts;

  BOOST_FOREACH(const t_param_map::value_type& c_param, m_cgi)
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

bool
Request::existsHeader(const string& p_name) const
{
  return m_headers.count(p_name) != 0;
}


bool
Request::getHeader(const string& p_name, string& p_value) const
{
  t_header_map::const_iterator c_header = m_headers.find(p_name);

  if (c_header == m_headers.end())
    return false;
  p_value = c_header->second;
  return true;
}

const Request::t_header_map&
Request::getHeaders(void) const
{
  return m_headers;
}


const string&
Request::getVersionStr(void) const
{
  return m_versionStr;
}

Request::t_version
Request::getVersion(void) const
{
  return m_version;
}

status
Request::read(std::istream& p_request)
{
  if (status::ok != readHead(p_request))
  {
    log::err("network.http.request", "protocol error : bad initial line", HERE);
    return status::error;
  }

  if (status::ok != readData(p_request))
  {
    log::err("network.http.request", "protocol error : invalid data", HERE);
    return status::error;
  }

  return status::ok;
}

status
Request::readHead(std::istream& p_request)
{
  if (status::ok != readInitial(p_request))
  {
    log::err("network.http.request", "protocol error : bad initial line", HERE);
    return status::error;
  }

  if (status::ok != readHeaders(p_request))
  {
    log::err("network.http.request", "protocol error : bad headers lines", HERE);
    return status::error;
  }

  return status::ok;
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
    setMethod(Request::METHOD_GET);
  else if (l_value == "post")
    setMethod(Request::METHOD_POST);
  else if (l_value == "head")
    setMethod(Request::METHOD_HEAD);
  else
  {
    log::err("network.http.request", "initial line error : unknown method '%s'", l_value, HERE);
    return status::error;
  }
  return status::ok;
}

// Assume url is encoded with iso-latin-1 characters
// will convert to ascii
// -> same process as in ke_texttools.cc ISO8859_Normalizer
struct hex_to_string
{
  template<typename T>
  string operator()(const T& p_match) const
  {
    stringstream  l_value;
    int           l_intVal;
    unsigned char l_result;
    l_value << std::hex << p_match.match_results().str(1);
    l_value >> l_intVal;
    l_result = l_intVal;
    return boost::str(boost::format("%c") % l_result);
  }
};



string
Request::paramValueDecode(const string& p_value)
{
  string       l_value(p_value);
  boost::regex l_hexRegex("%([0-9a-fA-F]{2})");
  boost::regex l_spaceRegex("[ \t]{2,}");

  // replace all + by space
  boost::replace_all(l_value, "+", " ");

  // replace hex value by corresponding character
  boost::smatch l_match;
  //CDE while used for %252520
  //maybe a future feature
  //while (boost::regex_search(l_value, l_match, l_hexRegex))
  boost::algorithm::find_format_all(l_value,
                                    boost::algorithm::regex_finder(l_hexRegex),
                                    hex_to_string());

  // remove duplicate spaces
  boost::algorithm::find_format_all(l_value,
                                    boost::algorithm::regex_finder(l_spaceRegex),
                                    boost::algorithm::const_formatter(" "));
  boost::trim(l_value);

  if (m_cgiToLower) {
    boost::to_lower(l_value);
  }

  return l_value;
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
      addCgi(l_key, paramValueDecode(l_value));
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
Request::readVersion(const string& p_version)
{
  size_t l_pos = p_version.find_last_of("/");

  if (p_version.substr(0, l_pos) != "HTTP")
  {
    log::err("network.http.request", "version error : invalid", HERE);
    return status::error;
  }

  string l_version = p_version.substr(l_pos + 1);
  setVersion(l_version);

  if (l_version == "1.0")
    setVersion(Request::VERSION_1_0);
  else if (l_version == "1.1")
    setVersion(Request::VERSION_1_1);
  else
  {
    log::err("network.http.request", "initial line error : unknown version '%s'", l_version, HERE);
    return status::error;
  }

  return status::ok;
}


status
Request::readHeaders(istream& p_request)
{
  while (false == p_request.eof())
  {
    vector<string> l_parts;
    string         l_line;

    getline(p_request, l_line);
    boost::trim_if(l_line, boost::is_any_of("\t\r\n "));
    if (0 == l_line.size())
      break;

    l_parts.push_back(l_line.substr(0, l_line.find_first_of(":")));
    l_parts.push_back(l_line.substr(l_line.find_first_of(":") + 1));
    boost::trim(l_parts[0]);
    boost::trim(l_parts[1]);
    addHeader(l_parts[0], l_parts[1]);
    log::info("network.http.request", "received header %s: %s", l_parts[0], l_parts[1], HERE);
  }

  return status::ok;
}


status
Request::getDataSize(size_t& p_length) const
{
  string l_strVal;

  if (false == getHeader("Content-Length", l_strVal))
  {
    p_length = 0;
    return status::ok;
  }

  try {
    p_length = boost::lexical_cast<size_t>(l_strVal);
  }
  catch (boost::bad_lexical_cast)
  {

    log::err("network.http.request", "read data : unable to interpret content-length '%s' as int", l_strVal, HERE);
    return status::error;
  }

  return status::ok;
}

/**
 ** @details
 ** on lit jusqu'a eof et on append dans m_data chaque chunk
 */
status
Request::readData(istream& p_request)
{
  string l_data;
  string l_contentType;
  string l_contentLength = "0";
  size_t l_length;

  copy(std::istreambuf_iterator<char>(p_request.rdbuf()),
       std::istreambuf_iterator<char>(),
       back_inserter(l_data));

  if (status::error == getDataSize(l_length))
  {
    log::err("network.http.request", "read data : error while gettin data length", HERE);
    return status::error;
  }

  if (l_length < l_data.size())
  {
    log::err("network.http.request", "read data : read more data than expected", HERE);
    return status::error;
  }

  if (l_length == 0)
    return status::ok;

  if (false == getHeader("Content-Type", l_contentType))
  {
    log::err("network.http.request", "read data : unable to get Content-Type", HERE);
    return status::error;
  }

  if (boost::starts_with(l_contentType, "application/x-www-form-urlencoded"))
  {
    return readDataFormEncoded(l_data);
  }
  else
  {
    log::err("network.http.request", "read data : unknown content type '%s'", l_contentType, HERE);
    return status::error;
  }
  return status::ok;
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

    addPost(l_key, paramValueDecode(l_value));
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

  switch (p_obj.getMethod())
  {
  case Request::METHOD_GET:
    p_buf << "method : GET" << endl;
    break;
  case Request::METHOD_POST:
    p_buf << "method : POST" << endl;
    break;
  case Request::METHOD_HEAD:
    p_buf << "method : HEAD" << endl;
    break;
  }

  p_buf << "path : " << p_obj.getPath() << endl;
  p_buf << "version : " << p_obj.getVersionStr() << endl;
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
