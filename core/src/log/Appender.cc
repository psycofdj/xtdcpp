#include "log/Appender.hh"
#include "log/Formatter.hh"

namespace xtd {
namespace log {

Appender::Appender(const sptr<Formatter>& p_formatter) :
  m_formatter(p_formatter)
{
}

Appender::~Appender(void)
{
}

void
Appender::setFormatter(const sptr<Formatter>& p_formatter)
{
  m_formatter = p_formatter;
}

void
Appender::addFilter(t_filter p_filter)
{
  m_filters.push_back(p_filter);
}

void
Appender::clearFilters(void)
{
  m_filters.clear();
}

void
Appender::clearFormatter(void)
{
  m_formatter = std::make_shared<Formatter>();
}


}}
