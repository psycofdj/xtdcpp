#ifndef XTD_CORE_LOG_FORMATMODIFIERS_HH_
# define XTD_CORE_LOG_FORMATMODIFIERS_HH_
# include <regex>
# include "types.hh"
# include "log/Fields.hh"
# include "log/logtypes.hh"
# include "tty.hh"


namespace xtd {
namespace log {

/**
 ** @brief Dynamically change format to pad fields to maximum known size
 */
struct AutoWidth
{
  /**
   ** @brief Constructor
   ** @param p_fields Field list to pad automatically, see @ref Formatter for names
   */
  AutoWidth(const vector<string>& p_fields);

  /**
   ** @brief Constructor
   ** @param p_field Name of field to pad automatically, see @ref Formatter for names
   */
  AutoWidth(const string& p_field);

  /**
   ** @brief Call operator
   ** @param p_rec current log record
   ** @param p_formats fields format to modify
   ** @details
   ** Compare current log record fields width against maximum know width and modify
   ** p_formats accordingly
   */
  void operator()(const FormattedRecord& p_rec, Fields<string>&  p_formats);

private:
  Fields<size_t> m_widths;
  vector<string> m_fields;
};

/**
 ** @brief Dynamically set slevel field style depending of record level
 */
struct StyleByLevel
{
  /**
   ** @brief Constructor
   ** @param p_conds Configuration map of style to apply for each level
   */
  StyleByLevel(const map<level, tty::style>& p_conds);

  /**
   ** @brief Call operator
   ** @param p_rec current log record
   ** @param p_styles fields styles to modify
   ** @details
   ** Sets slevel field style to value found in configuration map for level of
   ** current log record.
   */
  void operator()(const FormattedRecord& p_rec, Fields<tty::style>& p_styles) const;

private:
  map<level, tty::style> m_conds;
};


/**
 ** @brief Dynamically set message field style depending of log record content
 */
struct StyleMatch
{
  /**
   ** @brief Constructor
   ** @param p_match Regexp matching log record message
   ** @param p_style Style to apply to field message if regexp matches
   */
  StyleMatch(const std::regex& p_match, const tty::style& p_style);

  /**
   ** @brief Call operator
   ** @param p_rec current log record
   ** @param p_styles fields styles to modify
   ** @details
   ** Sets message field style to value given at construction if current log message
   ** matches construction regexp.
   */
  void operator()(const FormattedRecord& p_rec, Fields<tty::style>& p_styles) const;

private:
  std::regex m_match;
  tty::style m_style;
};




}}

#endif // !XTD_CORE_LOG_FORMATMODIFIERS_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
