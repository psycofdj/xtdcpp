#ifndef XTD_CORE_LOG_APPENDER_HH_
# define XTD_CORE_LOG_APPENDER_HH_
# include "log/logtypes.hh"
# include "log/fwd.hh"
# include "log/Formatter.hh"

namespace xtd {
namespace log {

/**
 ** @brief Displays log record to a particular output
 ** @details
 **
 ** This abstract object has two purposes :
 ** - use assigned Formatter object to generate a FormatterRecord
 ** - test generated FormatterRecord against registered filters
 ** - forward to @ref Appender::print "print" virtual callback if none of filters discarded
 **   the record
 **
 ** #### Inheriting
 **
 ** To implement your own output behavior, you must create a class that
 ** inherits from Appender and override the following callback :
 ** - @ref Appender::print "Appender::print(const FormatterRecord&)"
 **   - @copybrief Appender::print
 **
 **
 ** #### Filtering
 **
 ** Filters can be anything callable compatible with the following signature :
 ** @code
 ** std::function<bool(const FormatterRecord&)>
 ** @endcode
 **
 ** Use the following methods to use filters :
 ** - @ref Appender::addFilter "addFilter(functor)"
 **   - @copybrief Appender::addFilter
 ** - @ref Appender::clearFilters "clearFilters()"
 **   - @copybrief Appender::clearFilters
 **
 */
class Appender
{
private:
  typedef fn<bool(const FormattedRecord&)> t_filter;
  typedef vector<t_filter>                 t_filters;

public:
  /**
   ** @brief Default constructor
   */
  Appender(void);

  /**
   ** @brief Constructor with associated formatter
   ** @param p_formatter associated Formatter
   */
  Appender(const sptr<Formatter>& p_formatter);

  /**
   ** @brief Destructor
   */
  virtual ~Appender(void);

public:
  /**
   ** @brief Set associated Formatter
   ** @param p_formatter new Formatter object
   */
  void setFormatter(const sptr<Formatter>& p_formatter);

  /**
   ** @brief Add filter functor
   ** @param p_filter filter to add
   */
  void addFilter(t_filter p_filter);

  /**
   ** @brief Removes all registered filters
   */
  void clearFilters(void);

  /**
   ** @brief Reset associated to default Formatter class
   */
  void clearFormatter(void);

protected:
  /**
   ** @brief (Abstract) Actually output the given log record
   ** @param p_rec initialized FormattedRecord object
   */
  virtual void print(const FormattedRecord& p_rec ) const = 0;

public:
  /**
   ** @brief Process log Record with given parameters
   ** @param p_record Log record
   ** @param p_args message format parameters
   */
  template<typename... Arguments>
  void log(Record& p_record, Arguments&&... p_args) const;

private:
  sptr<Formatter> m_formatter;
  t_filters       m_filters;
};


template<typename... Arguments>
void
Appender::log(Record& p_record, Arguments&&... p_args) const
{
  FormattedRecord l_rec = m_formatter->format(p_record, p_args...);

  bool l_accepted = true;
  for (auto c_filter : m_filters)
    l_accepted = l_accepted && c_filter(l_rec);

  if (true == l_accepted)
    print(l_rec);
}

}}

#endif // !XTD_CORE_LOG_APPENDER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
