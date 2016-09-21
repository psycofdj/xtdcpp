#ifndef XTD_CORE_LOG_MEMORYAPPENDER_HH_
# define XTD_CORE_LOG_MEMORYAPPENDER_HH_
# include "log/fwd.hh"
# include "log/Appender.hh"


namespace xtd {
namespace log {

class MemoryAppender : public Appender
{
  friend class ConfLoader;

public:
  MemoryAppender(const sptr<Formatter>& p_formatter = std::make_shared<Formatter>());
  virtual ~MemoryAppender(void);

public:
  inline const vector<FormattedRecord>& getRecords(void) const;

private:
  virtual void           print(const FormattedRecord& p_rec) const override;
  static  sptr<Appender> create(const string&             p_name,
                                const map<string,string>& p_properties);

private:
  mutable vector<FormattedRecord> m_records;
};


const vector<FormattedRecord>&
MemoryAppender::getRecords(void) const
{
  return m_records;
}

}}

#endif // !XTD_CORE_LOG_MEMORYAPPENDER_HH_
