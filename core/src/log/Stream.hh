#ifndef XTD_CORE_LOG_STREAM_HH_
# define XTD_CORE_LOG_STREAM_HH_
# include "types.hh"
# include <mutex>

namespace xtd {
namespace log {

class Stream
{
public:
  Stream(std::ostream& p_stream);

public:
  void append(const string& p_value);

private:
  std::ostream&      m_stream;
  mutable std::mutex m_mutex;
};

}}

#endif // !XTD_CORE_LOG_STREAM_HH_
