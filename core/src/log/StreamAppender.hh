#ifndef XTD_CORE_LOG_STREAMAPPENDER_HH_
# define XTD_CORE_LOG_STREAMAPPENDER_HH_
# include "log/fwd.hh"
# include "log/Appender.hh"
# include "log/Stream.hh"

namespace xtd {
namespace log {

class StreamAppender : public Appender
{
  friend class ConfLoader;

public:
  StreamAppender(const sptr<Stream>&    p_stream    = std::make_shared<Stream>(std::cerr),
                 const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());

  StreamAppender(ostream&               p_stream    = std::cerr,
                 const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());

  virtual ~StreamAppender(void);

protected:
  static sptr<Appender> create(const string& p_name, const map<string,string>& p_properties);

protected:
  void print(const FormattedRecord& p_rec) const override;

private:
  sptr<Stream> m_stream;
};

}}

#endif // !XTD_CORE_LOG_STREAMAPPENDER_HH_
