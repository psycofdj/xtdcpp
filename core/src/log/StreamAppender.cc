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
  *m_stream << (p_rec.m_fulllog + "\n");
  m_stream->flush();
};


sptr<Appender>
StreamAppender::create(const string& p_name, const map<string,string>& p_properties)
{
  string                  l_key     = "log.appender." + p_name;
  auto                    c_stream  = p_properties.find(l_key + ".fd");
  auto                    c_modeKey = p_properties.find(l_key + ".mode");
  std::ios_base::openmode l_mode    = std::ios_base::out | std::ios_base::binary;

  if (p_properties.end() == c_stream)
    log::raise<log_error>("unable to find key '%s'", "log.appender." + p_name + ".fd", HERE);

  if (c_stream->second == "cout")
    return std::make_shared<StreamAppender>(std::cout);
  if (c_stream->second == "cerr")
    return std::make_shared<StreamAppender>(std::cerr);

  if (p_properties.end() == c_modeKey)
    l_mode |= std::ios_base::app;
  else if ("append" == c_modeKey->second)
    l_mode |= std::ios_base::app;
  else if ("truncate" == c_modeKey->second)
    l_mode |= std::ios_base::trunc;
  else
  {
    log::raise<log_error>("invalid mode value '%s' for key '%s'",
                          c_modeKey->second, c_modeKey->first);
  }

  ofstream*          l_pFile = new ofstream(c_stream->second.c_str(), l_mode);
  sptr<std::ostream> l_file(l_pFile);

  if (true == l_pFile->is_open())
  {
    sptr<Stream> l_stream = std::make_shared<Stream>(l_file);
    return std::make_shared<StreamAppender>(l_stream);
  }

  log::raise<log_error>("could not open file '%s' given by key '%s'",
                        c_stream->second, c_stream->first);

  // LCOV_EXCL_START
  return sptr<Appender>();
  // LCOV_EXCL_STOP
}

}}
