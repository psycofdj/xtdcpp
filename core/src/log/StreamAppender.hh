#ifndef XTD_CORE_LOG_STREAMAPPENDER_HH_
# define XTD_CORE_LOG_STREAMAPPENDER_HH_
# include <iostream>
# include <map>
# include <memory>
# include <string>
# include "log/Appender.hh"
# include "log/Formatter.hh"
# include "log/logtypes.hh"
# include "types.hh"
class TestStreamAppender;
namespace xtd { namespace log { class Stream; } }
namespace xtd {
namespace log {

/**
 ** @brief Prints log records on streams
 ** @details
 **
 ** Given log records are printed to given output stream parameter.
 **
 ** ### Configuration
 **
 ** For a given name **@<name@>**, the @ref StreamAppender::create function considers
 ** the following property key :
 ** - *log.appender.@<name@>.fd* : (mandatory) target file path for stream output.
 **   Special values :
 **   - *cout* : output records on stdout
 **   - *cerr* : output records on stderr
 **
 ** - *log.appender.@<name@>.mode* : (optional) When *fd* is a file, specify write
 **   mode. Possible values :
 **   - *append* : Append records output file. Default when property is not specified
 **   - *truncate* : Truncate output file
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** Underlying stream is wrapped by @ref Stream class. This class acquire a mutex
 ** lock before writing to stream. The mutex is global, which mean that concurrent
 ** writes on different streams will occur sequentially.
 */
class StreamAppender : public Appender
{
  friend class ConfLoader;
  friend class ::TestStreamAppender;

public:
  /**
   ** @brief Constructor with ostream
   ** @param p_stream generic output stream
   ** @param p_formatter log record formatter, default is Formatter
   */
  StreamAppender(ostream&               p_stream    = std::cerr,
                 const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());

  /**
   ** @brief Constructor with wrapper ostream
   ** @param p_stream generic output stream wrapped by Stream object
   ** @param p_formatter log record formatter, default is Formatter
   */
  StreamAppender(const sptr<Stream>&    p_stream,
                 const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());

  /**
   ** @brief Destructor
   */
  virtual ~StreamAppender(void);

public:
  /**
   ** @brief Create a new StreamAppender instance from given configuration map
   ** @param p_name Appender name
   ** @param p_properties configuration map
   ** @details
   ** Full configuration specification described in @ref StreamAppender detailed
   ** section.
   */
  static sptr<Appender> create(const string&             p_name,
                               const map<string,string>& p_properties);

protected:
  /**
   ** @brief Actually outputs given record to underlying stream
   ** @param p_rec log record to send
   */
  void print(const FormattedRecord& p_rec) const override;

private:
  sptr<Stream> m_stream;
};

}}

#endif // !XTD_CORE_LOG_STREAMAPPENDER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
