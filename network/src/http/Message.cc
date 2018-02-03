#include "http/Message.hh"
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <iterator>
#include "format.hh"       // libcore
#include "log/helpers.hh"  // libcore
#include "log/logtypes.hh" // libcore

namespace xtd {
namespace network {
namespace http {


Message::Message(void):
  m_version(version::v1_0),
  m_headers()
{
}

Message::~Message(void)
{
}

void
Message::setVersion(version p_version)
{
  m_version = p_version;
}

version
Message::getVersion(void) const
{
  return m_version;
}


bool
Message::existsHeader(const string& p_name) const
{
  return m_headers.count(p_name) != 0;
}


void
Message::addHeader(const string& p_name,
                   const string& p_value)
{
  m_headers[p_name] = p_value;
}



bool
Message::getHeader(const string& p_name, string& p_value) const
{
  t_header_map::const_iterator c_header = m_headers.find(p_name);

  if (c_header == m_headers.end())
    return false;
  p_value = c_header->second;
  return true;
}

const Message::t_header_map&
Message::getHeaders(void) const
{
  return m_headers;
}


status
Message::readHeaders(istream& p_request)
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
Message::readVersion(const string& p_version)
{
  size_t l_pos = p_version.find_last_of("/");

  if (p_version.substr(0, l_pos) != "HTTP")
  {
    log::err("network.http.request", "version error : invalid", HERE);
    return status::error;
  }

  string l_version = p_version.substr(l_pos + 1);

  if (l_version == "1.0")
    setVersion(version::v1_0);
  else if (l_version == "1.1")
    setVersion(version::v1_1);
  else
  {
    log::err("network.http.request", "initial line error : unknown version '%s'", l_version, HERE);
    return status::error;
  }

  return status::ok;
}


status
Message::read(std::istream& p_request)
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
Message::readHead(std::istream& p_request)
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
Message::readInitial(istream& p_request)
{
  string         l_line;
  vector<string> l_parts;

  getline(p_request, l_line);
  boost::trim_if(l_line, boost::is_any_of(" \n\r\t"));
  return status::ok;
}


status
Message::getDataSize(size_t& p_length) const
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
Message::readData(istream& p_request)
{
  size_t l_length;

  copy(std::istreambuf_iterator<char>(p_request.rdbuf()),
       std::istreambuf_iterator<char>(),
       back_inserter(m_data));

  if (status::error == getDataSize(l_length))
  {
    log::err("network.http.request", "read data : error while gettin data length", HERE);
    return status::error;
  }

  if (l_length < m_data.size())
  {
    log::err("network.http.request", "read data : read more data than expected", HERE);
    return status::error;
  }

  return status::ok;
}

void
Message::write(std::ostream& p_stream) const
{
  writeInitial(p_stream);
  writeHeaders(p_stream);
  p_stream << "\r\n";
  writeData(p_stream);
}

void
Message::writeInitial(std::ostream& /* p_stream */) const
{
}

void
Message::writeData(std::ostream& p_stream) const
{
  p_stream << getData();
}

void
Message::writeHeaders(std::ostream& p_stream) const
{
  p_stream << format::vargs("Content-Length: %d\r\n", getData().size());
  for (auto& c_header : getHeaders())
    p_stream << format::vargs("%s: %s\r\n", c_header.first, c_header.second);
}


const string&
Message::getData(void) const
{
  return m_data;
}




}}}
