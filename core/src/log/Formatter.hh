#ifndef XTD_CORE_LOG_FORMATTER_HH_
# define XTD_CORE_LOG_FORMATTER_HH_
# include "log/fwd.hh"
# include "log/logtypes.hh"
# include "log/Fields.hh"

class TestFormatter;

namespace xtd {
namespace log {

/**
 ** @brief Formats log record data fields for output
 **
 ** This object is used by Appender to convert raw log Record in FormattedRecord
 ** by converting all available fields to a final printable string.
 **
 ** ### Formats and Layouts
 **
 ** Simple fields :
 ** - **name**     : Logger name. Note: logger name may be different than module
 **   due to logging hierarchy tree
 ** - **pid**      : current process id
 ** - **ppid**     : current parent process id
 ** - **module**   : module name of record
 ** - **threadid** : current thread id
 ** - **slevel**   : string identifier of record's @ref level
 ** - **ilevel**   : integer identifier of record's @ref level
 ** - **message**  : record's message string
 ** - **filename** : record's location file name
 ** - **line**     : record's location line in file
 ** - **function** : record's location function
 **
 ** Compound fields :
 ** - **time** : creating time of the log record
 ** - **location** : location in the code where the record was emitted
 ** - **fulllog** : the final formatted log string
 **
 ** Simple and compound fields has their own display format. In addition,
 ** compound fields has a layout that can refer to other fields.
 **
 ** #### Formatting process
 **
 ** The formatting process words as follow :
 **
 ** - Apply formats to simple fields
 ** - Apply layout  to time and location compound fields
 ** - Apply formats to time and location compound fields
 ** - Apply layout to fulllog compound field
 ** - Apply format to fulllog compound field
 **
 ** #### Layouts
 **
 ** Layout can refer to other fields using a ```%(reference)``` notation.
 **
 ** - **fulllog** :  layout that represent the final generated string. This
 **   layout can refer to any of the other fields names.
 **
 **   Default value is ```%(name) [%(pid)] [%(ppid)] %(time) [%(threadid)]
 **   [%(module)] (%(slevel)) %(message) %(location)```
 **
 ** - **location** : layout that represent the source code location that emitted
 **   the log record. This layout can refer to :
 **   - <b> %(file) </b>: file name
 **   - <b> %(line) </b>: line number in the file
 **   - <b> %(function) </b> : name of the function
 **
 **   Default value is ```at %(filename):%(line) -> %(function)```
 **
 ** - The **time** layout that formats the current date and time when the log
 **   record was triggered. This layout can refer to any of the flags handled by
 **   boost::posix_time. Full description
 **   [here](http://www.boost.org/doc/libs/1_55_0/doc/html/date_time/date_time_io.html)
 **
 **   Default value is ```%d-%m-%Y %H:%M:%s```
 **
 ** #### Formats
 **
 ** The default fields formats are given below :
 ** - **name**     : ```%%s```
 ** - **pid**      : ```%%d```
 ** - **ppid**     : ```%%d```
 ** - **module**   : ```%%s```
 ** - **threadid** : ```%02d```
 ** - **slevel**   : ```%%7s```
 ** - **ilevel**   : ```%%d```
 ** - **message**  : ```%%s```
 ** - **filename** : ```%%s```
 ** - **line**     : ```%%d```
 ** - **function** : ```%%s```
 ** - **time**     : ```%%s```
 ** - **location** : ```%%s```
 ** - **fulllog**  : ```%%s```
 **
 **
 ** ### User methods
 **
 ** - @ref Formatter::setLayout "setLayout" :
 **   - @copybrief Formatter::setLayout
 ** - @ref Formatter::setLayouts "setLayouts" :
 **   - @copybrief Formatter::setLayouts
 ** - @ref Formatter::setFieldFormat "setFieldFormat" :
 **   - @copybrief Formatter::setFieldFormat
 ** - @ref Formatter::setFieldFormats "setFieldFormats" :
 **   - @copybrief Formatter::setFieldFormats
 ** - @ref Formatter::addFormatModifier "addFormatModifier" :
 **   - @copybrief Formatter::addFormatModifier
 ** - @ref Formatter::create "create" :
 **   - @copybrief Formatter::create
 **
 ** ### Child class methods
 ** - @ref Formatter::format "format" :
 **   - @copybrief Formatter::format
 ** - @ref Formatter::createFields "createFields" :
 **   - @copybrief Formatter::createFields
 ** - @ref Formatter::getFulllog "getFulllog" :
 **   - @copybrief Formatter::getFulllog
 ** - @ref Formatter::resolveLayout "resolveLayout" :
 **   - @copybrief Formatter::resolveLayout
 ** - @ref Formatter::resolveTime "resolveTime" :
 **   - @copybrief Formatter::resolveTime
 **
 ** ### Format modifiers
 **
 ** In addition to fields formats, user can register one or more format modifiers.
 ** Their goal is to dynamically modify the field formats depending of actual log
 ** record data.
 **
 ** These modifiers can be any functor compatible with
 ** @ref Formatter::t_format_modifier prototype.
 **
 ** XTD's logging facility provides the some format modifiers described in the
 ** dedicated section.
 **
 ** ### Configuration
 **
 ** For a given name **@<name@>**, the @ref Formatter::create function considers
 ** the following property keys, each describing compound field layout or field
 ** format :
 ** - *log.formatter.@<name@>.layout.fulllog*
 ** - *log.formatter.@<name@>.layout.location*
 ** - *log.formatter.@<name@>.layout.time*
 ** - *log.formatter.@<name@>.format.name*
 ** - *log.formatter.@<name@>.format.pid*
 ** - *log.formatter.@<name@>.format.ppid*
 ** - *log.formatter.@<name@>.format.module*
 ** - *log.formatter.@<name@>.format.threadid*
 ** - *log.formatter.@<name@>.format.slevel*
 ** - *log.formatter.@<name@>.format.ilevel*
 ** - *log.formatter.@<name@>.format.message*
 ** - *log.formatter.@<name@>.format.filename*
 ** - *log.formatter.@<name@>.format.line*
 ** - *log.formatter.@<name@>.format.function*
 ** - *log.formatter.@<name@>.format.time*
 ** - *log.formatter.@<name@>.format.location*
 ** - *log.formatter.@<name@>.format.fulllog*
 **
 ** ### Thread Safety
 **
 ** - Concurrent calls to multiple objects : **YES**
 ** - Concurrent calls to single object    : **YES**
 **
 ** This object is lock-free and thread safe as long as formats, layout and
 ** modifiers aren't modified by other threads.
 **
 ** ### Usage Exemple
 **
 ** @include formatter.cc
 */
class Formatter
{
  friend class ConfLoader;
  friend class ::TestFormatter;

protected:
  /**
   ** @brief Holds format string for each field
   */
  typedef Fields<string> t_formats;

  /**
   ** @brief Format modifier functor prototype
   */
  typedef fn<void(const FormattedRecord&, t_formats&)> t_format_modifier;

public:
  /**
   ** @brief Constructor
   */
  Formatter(void);

  /**
   ** @brief Destructor
   */
  virtual ~Formatter(void);

public:
  /**
   ** @brief Create a new Formatter instance from given configuration map
   ** @param p_name Formatter name
   ** @param p_properties configuration map
   ** @throw log_error when invalid keys/value given in p_properties
   ** @details
   ** Full configuration specification described in @ref Formatter detailed
   ** section.
   */
  static sptr<Formatter> create(const string&             p_name,
                                const map<string,string>& p_properties);

public:
  /**
   ** @brief Set layout for a particular compound field
   ** @param p_name name of the field
   ** @param p_layout new layout
   ** @return self, used for chaining calls
   ** @details
   ** The function doesn't check if given p_name is a valid compound field. If not,
   ** new layout will have no effect.
   */
  Formatter& setLayout(const string& p_name, const string& p_layout);

  /**
   ** @brief Set layout for all compound fields
   ** @param p_layouts layout map. Key is field name, value is new layout
   ** @return self, used for chaining calls
   ** @details
   ** The function doesn't check if given p_name is a valid compound field. If not,
   ** new layouts will have no effect.
   */
  Formatter& setLayouts(const map<string, string>& p_layouts);

  /**
   ** @brief Set format for a particular field
   ** @param p_fieldName field name
   ** @param p_format new format
   ** @return self, used for chaining calls
   ** @details
   ** The function doesn't check if given p_name is a valid field. If not,
   ** new format will have no effect.
   */
  Formatter& setFieldFormat(const string& p_fieldName, const string& p_format);

  /**
   ** @brief Set format for all fields
   ** @param p_formats format map, Key is field name, value is new format
   ** @return self, used for chaining
   ** @details
   ** The function doesn't check if given p_name is a valid field. If not,
   ** new formats will have no effect.
   */
  Formatter& setFieldFormats(const map<string, string>& p_formats);

  /**
   ** @brief Adds a format modifier functor
   ** @param p_functor functor compatible with @ref t_format_modifier prototype
   ** @return self, used for chaining
   */
  template<typename Handler>
  Formatter& addFormatModifier(Handler p_functor);

  /**
   ** @brief Execute formatting process
   ** @tparam Arguments Variadic log message format arguments
   ** @param p_rec active log record
   ** @param p_args arguments to apply to log format
   ** @return FormattedRecord object
   */
  template<typename... Arguments>
  FormattedRecord format(const Record& p_rec, Arguments&&... p_args) const;

protected:
  /**
   ** @brief Apply formats and layouts to all fields but fulllog
   ** @param p_rec active log record
   ** @param p_args Holds formatting result for each fields
   */
  virtual void createFields(const FormattedRecord& p_rec,
                            Fields<string>&        p_args) const;

  /**
   ** @brief Creates final log message
   ** @param p_rec active log record
   */
  virtual string getFulllog(const FormattedRecord& p_rec) const;

  /**
   ** @brief Resolve given layout from given fields data
   ** @param p_layout layout to resolve
   ** @param p_args fields data
   */
  string resolveLayout(const string& p_layout, const Fields<string>& p_args) const;

  /**
   ** @brief Apply time layout to given boost::posix_time::ptime argument
   ** @param p_layout boost's compatible time layout string
   ** @param p_time ptime argument
   */
  string resolveTime(const string&                   p_layout,
                     const boost::posix_time::ptime& p_time) const;

protected:
  string                    m_fmt;             ///< fulllog layout
  string                    m_locFmt;          ///< location layout
  string                    m_timeFmt;         ///< time layout
  Fields<string>            m_fieldFormats;    ///< Fields formats
  vector<t_format_modifier> m_formatModifiers; ///< List of format modifiers
};

}}

# include "log/Formatter.hxx"

#endif // !XTD_CORE_LOG_FORMATTER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
