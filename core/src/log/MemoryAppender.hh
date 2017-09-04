#ifndef XTD_CORE_LOG_MEMORYAPPENDER_HH_
# define XTD_CORE_LOG_MEMORYAPPENDER_HH_
# include <boost/circular_buffer.hpp>
# include <mutex>
# include "log/Appender.hh"
# include "log/Formatter.hh"
# include "log/logtypes.hh"
# include "types.hh"

class TestMemoryAppender;

namespace xtd {
namespace log {

/**
 ** @brief Stores the Nth last log records in memory
 ** @details
 **
 ** This object stores the last N received log records into the memory. When N is
 ** reached, the oldest record is replaced by the new record.
 **
 ** The underlying storage mechanism allocates the memory for the N elements at
 ** creation.
 **
 ** ### Configuration
 **
 ** For a given name **@<name@>**, the @ref MemoryAppender::create function considers
 ** the following property key :
 ** - *log.appender.@<name@>.size* : (optional) gives the maximum number of log
 **   records to store, default is 5000. Given valid must be a valid positive number.
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** Adding a new log record is protected by an instance local mutex lock. Records
 ** returned by @ref MemoryAppender::copyRecords is a copy on underlying data.
 */
class MemoryAppender : public Appender
{
  friend class ConfLoader;
  friend class ::TestMemoryAppender;

public:
  /**
   ** @brief Constructor
   ** @param p_maxRecords Maximum number of record to keep in memory
   ** @param p_formatter log record formatter, default is Formatter
   */
  MemoryAppender(size_t                 p_maxRecords = 5000,
                 const sptr<Formatter>& p_formatter  = std::make_shared<Formatter>());

  /**
   ** @brief Destructor
   */
  virtual ~MemoryAppender(void);

public:
  /**
   ** @brief Returns stored log records
   ** @return Stored log records
   ** @details
   ** A copy of record is returned to ensure thread safeness
   */
  vector<FormattedRecord> copyRecords(void) const;

protected:
  /**
   ** @brief Stores log record to list
   ** @param p_rec log record
   */
  virtual void print(const FormattedRecord& p_rec) const override;

public:
  /**
   ** @brief Create a new MemoryAppender instance from given configuration map
   ** @param p_name Appender name
   ** @param p_properties configuration map
   ** @details
   ** Full configuration specification described in @ref MemoryAppender detailed
   ** section.
   */
  static sptr<Appender> create(const string&             p_name,
                               const map<string,string>& p_properties);

private:
  size_t                                          m_maxRecords;
  mutable boost::circular_buffer<FormattedRecord> m_records;
  mutable std::mutex                              m_mutex;
};


}}

#endif // !XTD_CORE_LOG_MEMORYAPPENDER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
