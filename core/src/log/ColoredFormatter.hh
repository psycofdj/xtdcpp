#ifndef CORE_LOG_COLOREDFORMATTER_HH_
# define CORE_LOG_COLOREDFORMATTER_HH_
# include <functional>
# include <map>
# include <memory>
# include <string>
# include <vector>
# include "log/Fields.hh"
# include "log/Formatter.hh"
# include "log/logtypes.hh"
# include "tty.hh"
# include "types.hh"
class TestColoredFormatter;

namespace xtd {
namespace log {

/**
 ** @brief Formats log record with @ref tty styles
 ** @details
 ** This object inherit from Formatter and adds the possibility to apply
 ** @ref tty styles to fields and dynamic style modifiers.
 **
 ** ### Styles
 **
 ** The default fields styles (fg-color, bg-color, attributes) are given below :
 ** - name      : green,   unset, unset
 ** - threadid  : yellow,  unset, unset
 ** - message   : white,   unset, unset
 ** - module    : lyellow, unset, unset
 ** - time      : cyan,    unset, unset
 ** - slevel    : lred,    unset, bold
 ** - location  : lblack,  unset, unset
 ** - pid       : lblue,   unset, unset
 ** - ppid      : lblue,   unset, bold
 ** - all other : unset, unset, unset
 **
 ** ### User methods
 **
 ** In addition to parent class methods, this objects provides :
 **
 ** - @ref ColoredFormatter::setAttrs "setAttrs" :
 **   - @copybrief ColoredFormatter::setAttrs
 ** - @ref ColoredFormatter::setFgColor "setFgColor" :
 **   - @copybrief ColoredFormatter::setFgColor
 ** - @ref ColoredFormatter::setBgColor "setBgColor" :
 **   - @copybrief ColoredFormatter::setBgColor
 ** - @ref ColoredFormatter::setStyle "setStyle" :
 **   - @copybrief ColoredFormatter::setStyle
 ** - @ref ColoredFormatter::setStyles "setStyles" :
 **   - @copybrief ColoredFormatter::setStyles
 ** - @ref ColoredFormatter::addStyleModifier "addStyleModifier" :
 **   - @copybrief ColoredFormatter::addStyleModifier
 **
 ** ### Configuration
 **
 ** In addition of the property keys used in @ref ColoredFormatter configuration,
 ** this objects considers **for each fields** the following triplet :
 ** - *log.formatter.@<name@>.style.@<field@>.fgColor*
 ** - *log.formatter.@<name@>.style.@<field@>.bgColor*
 ** - *log.formatter.@<name@>.style.@<field@>.attrs*
 **
 **
 ** - **fgColor** and **bgColor** : name matching one of the @ref tty::color constants
 ** or a number coding for extended terminal color code.
 **
 ** - **attrs** : a comma or pipe separated list of names matching one of the
 ** @ref tty::attrs constants
 */
class ColoredFormatter : public Formatter
{
  friend class ConfLoader;
  friend class ::TestColoredFormatter;

protected:
  /**
   ** @brief Holds style for each field
   */
  typedef Fields<tty::style> t_styles;

protected:
  /**
   ** @brief Style modifier functor prototype
   */
  typedef fn<void(const FormattedRecord&, t_styles&)> t_style_modifier;

public:
  /**
   ** @brief Constructor
   */
  ColoredFormatter(void);

  /**
   ** @brief Constructor from parent Formatter
   */
  ColoredFormatter(const Formatter& p_base);

  /**
   ** @brief Destructor
   */
  virtual ~ColoredFormatter(void);

public:
  /**
   ** @brief Create a new ColoredFormatter instance from given configuration map
   ** @param p_name Formatter name
   ** @param p_properties configuration map
   ** @throw log_error when invalid keys/value given in p_properties
   */
  static sptr<Formatter> create(const string&             p_name,
                                const map<string,string>& p_properties);

public:
  /**
   ** @brief Sets tty attributes for given field
   ** @param p_field field name to modify
   ** @param p_attrs new tty::attrs
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setAttrs(const string&     p_field,
                             const tty::attrs& p_attrs);

  /**
   ** @brief Sets foreground tty color for given field
   ** @param p_field field name to modify
   ** @param p_fg new tty::color
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setFgColor(const string&     p_field,
                               const tty::color& p_fg);

  /**
   ** @brief Sets background tty color for given field
   ** @param p_field field name to modify
   ** @param p_bg new tty::color
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setBgColor(const string&     p_field,
                               const tty::color& p_bg);

  /**
   ** @brief Sets tty style for given field (fgColor, bgColor + attributes)
   ** @param p_field field name to modify
   ** @param p_style new tty::style
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setStyle(const string&     p_field,
                             const tty::style& p_style);

  /**
   ** @brief Sets tty style, split version
   ** @param p_field field name to modify
   ** @param p_fg new foreground tty::color
   ** @param p_bg new background tty::color
   ** @param p_attrs new tty attributes
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setStyle(const string&     p_field,
                             const tty::color& p_fg,
                             const tty::color& p_bg,
                             const tty::attrs& p_attrs);

  /**
   ** @brief Sets styles for all given fields
   ** @param p_styles map where key is field name and value is new tty::style
   ** @return self for chaining calls
   ** @details
   ** If given p_field is unknown, the method has no effect
   */
  ColoredFormatter& setStyles(const map<string, tty::style>& p_styles);


  /**
   ** @brief Adds a style modifier functor
   ** @param p_functor functor compatible with @ref t_style_modifier prototype
   ** @return self, used for chaining
   */
  template<typename Handler>
  ColoredFormatter& addStyleModifier(Handler p_functor);

protected:
  /**
   ** @brief Same as @ref Formatter::createFields but also applies styles
   */
  virtual void createFields(const FormattedRecord& p_rec, Fields<string>& p_args) const override;

  /**
   ** @brief Same as @ref Formatter::getFulllog but also applies styles
   */
  virtual string getFulllog(const FormattedRecord& p_rec) const override;

private:
  void defaultStyles(void);

protected:
  t_styles                  m_fieldStyles;    ///< Fields style
  vector<t_style_modifier>  m_styleModifiers; ///< List of style modifiers
};


template<typename Handler>
ColoredFormatter&
ColoredFormatter::addStyleModifier(Handler p_functor)
{
  t_style_modifier l_modifier = [p_functor](const FormattedRecord& p_rec, t_styles& p_format) {
    p_functor(p_rec, p_format);
  };
  m_styleModifiers.push_back(l_modifier);
  return *this;
}

}}

#endif // !CORE_LOG_COLOREDFORMATTER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
