#include "Response.hh"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // IWYU pragma: keep
#include "format.hh" // libcore
#include "http/codes.hh"


namespace xtd {
namespace network {
namespace http {


Response::Response(void) :
  m_status(valueof(code::bad_request)),
  m_cachable(false)
{
}

void
Response::setCachable(bool p_status)
{
  m_cachable = p_status;
}

void
Response::setStatus(uint32_t p_status)
{
  m_status = p_status;
}


void
Response::setStatus(code p_status)
{
  setStatus(valueof(p_status));
}

void
Response::setData(const string& p_data)
{
  m_data = p_data;
}


void
Response::appendData(const string& p_data)
{
  m_data += p_data;
}

uint32_t
Response::getStatus(void) const
{
  return m_status;
}


void
Response::writeHeaders(std::ostream& p_stream) const
{
  Message::writeHeaders(p_stream);

  if (true == m_cachable)
  {
    std::stringstream        l_out;
    uint64_t                 l_cacheTTLSec = 365 * 24 * 60 * 60;
    boost::posix_time::ptime l_time        =
      boost::posix_time::second_clock::universal_time() +
      boost::posix_time::seconds(l_cacheTTLSec);
    l_out.imbue(std::locale(l_out.getloc(), new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT")));
    l_out << l_time;
    p_stream << format::vargs("Expires: %d\r\n", l_out.str());
    p_stream << format::vargs("Cache-Control: private,maxage=%d\r\n", l_cacheTTLSec);
  }
  else
  {
    p_stream << "Pragma: no-cache\r\n";
    p_stream << "Cache-Control: no-cache\r\n";
  }
}

void
Response::writeInitial(std::ostream& p_stream) const
{
  const string& l_msg = codes::getMessage(getStatus());
  p_stream << format::vargs("HTTP/%s %d %s\r\n", str(getVersion()) , getStatus(), l_msg);
}



ostream& operator<<(ostream& p_buf, const Response& p_obj)
{
  p_buf << "code : "    << p_obj.getStatus() << codes::getMessage(p_obj.getStatus())  << endl;
  p_buf << "version : " << str(p_obj.getVersion()) << endl;
  p_buf << "headers :"  << endl;
  for (auto& cc_header : p_obj.getHeaders())
    p_buf << " -> " << cc_header.first << ": " << cc_header.second << endl;
  return p_buf;
}

}}} //end namespaces
