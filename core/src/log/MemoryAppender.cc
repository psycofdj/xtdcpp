#include "log/MemoryAppender.hh"

namespace xtd {
namespace log {

MemoryAppender::MemoryAppender(const sptr<Formatter>& p_formatter) :
  Appender(p_formatter)
{
}


MemoryAppender::~MemoryAppender(void)
{
}

void
MemoryAppender::print(const FormattedRecord& p_rec) const
{
  m_records.push_back(p_rec);
}

std::shared_ptr<Appender>
MemoryAppender::create(const string& /* p_name */, const map<string,string>& /* p_properties */)
{
  return std::make_shared<MemoryAppender>();
}

}}
