#ifndef XTD_CORE_LOG_STREAM_HH_
# define XTD_CORE_LOG_STREAM_HH_
# include <mutex>
# include "types.hh"

namespace xtd {
namespace log {

/**
 ** @brief Protects underlying ostream operations with a mutex lock
 **
 */
class Stream
{
public:
  /**
   ** @brief Constructor
   ** @param p_stream target ostream
   */
  Stream(std::ostream& p_stream);

  /**
   ** @brief Constructor
   ** @param p_stream target ostream
   */
  Stream(const sptr<std::ostream>& p_stream);

public:
  /**
   ** @brief Flush underlying stream
   */
  void flush(void);

  /**
   ** @brief Write given value to underlying stream
   ** @param p_value value to write
   */
  template<class T>
  Stream& operator<<(const T& p_value);

private:
  sptr<std::ostream> m_ref;
  std::ostream&      m_stream;
  static std::mutex  ms_mutex;
};



template<class T>
Stream& Stream::operator<<(const T& p_value)
{
  std::lock_guard<std::mutex> l_lock(ms_mutex);
  m_stream << p_value;
  return *this;
}


}}

#endif // !XTD_CORE_LOG_STREAM_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
