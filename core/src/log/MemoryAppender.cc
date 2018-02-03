#include "log/MemoryAppender.hh"
#include <boost/lexical_cast.hpp>
#include "types.hh"
#include "log/helpers.hh"

namespace xtd {
namespace log {

MemoryAppender::MemoryAppender(size_t                 p_maxRecords,
                               const sptr<Formatter>& p_formatter) :
  Appender(p_formatter),
  m_records(p_maxRecords),
  m_mutex()
{
}


MemoryAppender::~MemoryAppender(void)
{
}

void
MemoryAppender::print(const FormattedRecord& p_rec) const
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  m_records.push_back(p_rec);
}

std::shared_ptr<Appender>
MemoryAppender::create(const string& p_name, const map<string,string>& p_properties)
{
  int64_t l_size    = 5000;
  auto    c_sizeKey = p_properties.find("log.appender." + p_name + ".size");

  if (p_properties.end() != c_sizeKey)
  {
    try {
      l_size = boost::lexical_cast<int64_t>(c_sizeKey->second);
    } catch (boost::bad_lexical_cast&) {
      l_size = 0;
    }
    if (0 >= l_size)
    {
      string l_fmt = "invalid value '%s' key '%s', must be valid positive number";
      log::raise<log_error>("core.log", l_fmt, c_sizeKey->second, c_sizeKey->first, HERE);
    }
  }

  return std::make_shared<MemoryAppender>(l_size);
}


vector<FormattedRecord>
MemoryAppender::copyRecords(void) const
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  return vector<FormattedRecord>(m_records.begin(), m_records.end());
}


}}
