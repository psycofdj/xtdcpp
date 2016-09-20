#include "log/StreamAppender.hh"
#include "log.hh"

namespace xtd {
namespace log {

StreamAppender::StreamAppender(const sptr<Stream>&    p_stream,
                               const sptr<Formatter>& p_formatter) :
  Appender(p_formatter),
  m_stream(p_stream)
{
}

StreamAppender::StreamAppender(ostream&               p_stream,
                               const sptr<Formatter>& p_formatter) :
  StreamAppender(std::make_shared<Stream>(p_stream), p_formatter)
{
}

StreamAppender::~StreamAppender(void)
{
}

void
StreamAppender::print(const FormattedRecord& p_rec) const
{
  m_stream->append(p_rec.m_fulllog + "\n");
};


sptr<Appender>
StreamAppender::create(const string& p_name, const map<string,string>& p_properties)
{
  auto c_stream = p_properties.find("log.appender." + p_name + ".fd");

  if (p_properties.end() == c_stream)
    log::raise<log_error>("unable to find key '%s'", "log.appender." + p_name + ".fd", HERE);

  if (c_stream->second == "cout")
    return std::make_shared<StreamAppender>(std::cout);
  if (c_stream->second == "cerr")
    return std::make_shared<StreamAppender>(std::cerr);

  log::raise<log_error>("invalid fd '%s' in key '%s'", c_stream->second, c_stream->first);
  return sptr<Appender>();
}

}}
