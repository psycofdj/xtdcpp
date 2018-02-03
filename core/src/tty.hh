#ifndef XTD_CORE_TTY_HH_
# define XTD_CORE_TTY_HH_
# include <cstdint>
# include <iosfwd>
# include <string>
# include <vector>
# include "types.hh"

namespace xtd {


/**
 ** @brief Manipulate text on tty environment
 */
namespace tty {

/**
 ** @brief ANSI codes for terminal output options
 ** @details
 **
 ** This object holds one or more terminal output option.
 **
 ** It cannot be constructed and can only be used through pre-defined constants.
 ** Available constants are :
 ** - attrs::unset       : @copybrief attrs::unset
 ** - attrs::reset       : @copybrief attrs::reset
 ** - attrs::dim         : @copybrief attrs::dim
 ** - attrs::bold        : @copybrief attrs::bold
 ** - attrs::underlined  : @copybrief attrs::underlined
 ** - attrs::blink       : @copybrief attrs::blink
 ** - attrs::reverse     : @copybrief attrs::reverse
 ** - attrs::hidden      : @copybrief attrs::hidden
 **
 ** Multiple constants can be combined together with
 ** @ref attrs::operator| "operator|" or @ref attrs::operator|= "operator|=".
 **  Example :
 ** @code
 ** attrs l_val = attrs::unset;
 **
 ** l_val  = attrs::bold;
 ** l_val  = attrs::bold | attrs::underlined;
 ** l_val |= reset;
 ** @endcode
 **
 ** Available methods :
 ** - @ref attrs::getValues "getValues" : @copybrief attrs::getValues
 ** - @ref attrs::isSet "isSet"         : @copybrief attrs::isSet
 */
class attrs
{
private:
  enum class s : uint32_t {
    reset      = 0,
    bold       = 1,
    dim        = 2,
    underlined = 4,
    blink      = 5,
    reverse    = 7,
    hidden     = 8
  };

public:
  static const attrs unset;      ///< Default output attribute, no effect
  static const attrs reset;      ///< Remove all previously defined attribute
  static const attrs dim;        ///< Reduced weight effect
  static const attrs bold;       ///< Increased weight effect
  static const attrs underlined; ///< Underlined effect
  static const attrs blink;      ///< Blinking effect
  static const attrs reverse;    ///< brief Revered foreground and background effect
  static const attrs hidden;     ///< Hidden font effect

private:
  attrs(void);
  attrs(s p_attr);

public:
  /**
   ** @brief Constructs attrs from string
   ** @param p_value string value
   ** @param p_attrs result attrs reference
   ** @return bool is p_value is a valid attribute string value
   */
  static bool from_string(const string& p_value, attrs& p_attrs);

  /**
   ** @brief Convert attrs to string
   ** @param p_attrs attrs to convert
   */
  static string to_string(const attrs& p_attrs);

public:
  /**
   ** @brief Creates the bitwise or self and given attrs
   ** @param p_o1 attributes to add
   */
  attrs operator|(const attrs& p_o1) const;

  /**
   ** @brief Merge given attributes into self
   ** @param p_o1 attributes to merge
   */
  attrs& operator|=(const attrs& p_o1);

  /**
   ** @brief Tells if given attrs is equivalent to current object
   ** @param p_obj Object to compare
   */
  bool operator==(const attrs& p_obj) const;

  /**
   ** @brief Returns internal ANSI code values
   ** @return vector of each effect id
   */
  const vector<uint32_t>& getValues(void) const;

  /**
   ** @brief Tells if at least one effect is registered
   */
  bool isSet(void) const;

private:
  vector<uint32_t> m_attrs;
};


/**
 ** @brief ANSI code for terminal output color
 ** @details
 ** There's two types of TTY colors, standard predefined and extended.
 **
 ** Predefined colors are built from constants and can be displayed on almost every
 ** terminals. Available colors are :
 ** - @ref color::unset    : @copybrief color::unset
 ** - @ref color::normal   : @copybrief color::normal
 ** - @ref color::black    : @copybrief color::black
 ** - @ref color::red      : @copybrief color::red
 ** - @ref color::green    : @copybrief color::green
 ** - @ref color::yellow   : @copybrief color::yellow
 ** - @ref color::blue     : @copybrief color::blue
 ** - @ref color::magenta  : @copybrief color::magenta
 ** - @ref color::cyan     : @copybrief color::cyan
 ** - @ref color::gray     : @copybrief color::gray
 ** - @ref color::lblack   : @copybrief color::lblack
 ** - @ref color::lred     : @copybrief color::lred
 ** - @ref color::lgreen   : @copybrief color::lgreen
 ** - @ref color::lyellow  : @copybrief color::lyellow
 ** - @ref color::lblue    : @copybrief color::lblue
 ** - @ref color::lmagenta : @copybrief color::lmagenta
 ** - @ref color::lcyan    : @copybrief color::lcyan
 ** - @ref color::lgray    : @copybrief color::lgray
 ** - @ref color::white    : @copybrief color::white
 **
 **
 ** Extended colors are direclty from their ANSI color code using the
 ** *uint32_t* constructor. Their compatibility depends on the terminal
 ** capabilities.
 */
class color
{
public:
  static const color unset;    ///< no color directive
  static const color normal;   ///< default color
  static const color black;    ///< black
  static const color red;      ///< red
  static const color green;    ///< green
  static const color yellow;   ///< yellow
  static const color blue;     ///< blue
  static const color magenta;  ///< magenta
  static const color cyan;     ///< cyan
  static const color gray;     ///< gray
  static const color lblack;   ///< light black
  static const color lred;     ///< light red
  static const color lgreen;   ///< light green
  static const color lyellow;  ///< light yellow
  static const color lblue;    ///< light blue
  static const color lmagenta; ///< light magenta
  static const color lcyan;    ///< light cyan
  static const color lgray;    ///< light gray
  static const color white;    ///< white

private:
  enum class c : uint32_t {
    unset    = 0,
    black    = 30,
    red      = 31,
    green    = 32,
    yellow   = 33,
    blue     = 34,
    magenta  = 35,
    cyan     = 36,
    gray     = 37,
    normal   = 39,
    lblack   = 90,
    lred     = 91,
    lgreen   = 92,
    lyellow  = 93,
    lblue    = 94,
    lmagenta = 95,
    lcyan    = 96,
    lgray    = 97,
    white    = 97
  };

private:
  color(c p_color);

public:
  /**
   ** @brief Constructor for extended color
   ** @param p_value terminal color code
   */
  color(uint32_t p_value);

public:
  /**
   ** @brief Construct color from string value
   ** @param p_value string value
   ** @param p_attrs ouput result reference
   ** @return true is p_value is a valid color name
   */
  static bool from_string(const string& p_value, color& p_attrs);

  /**
   ** @brief Converts color to string
   */
  static string to_string(const color& p_color);

public:
  /**
   ** @brief Tells if given color is equivalent to current object
   ** @param p_obj Object to compare
   */
  bool operator==(const color& p_obj) const;

  /**
   ** @brief Tells if color is extended
   */
  bool isExtended(void) const;

  /**
   ** @brief Get terminal color code
   */
  uint32_t getValue(void) const;

  /**
   ** @brief Tells is color is not color::unset
   */
  bool isSet(void) const;

private:
  uint32_t m_value;
  bool     m_extended;
};


/**
 ** @brief Aggregates foreground color, background color and display options
 ** @details
 **
 ** Usage example :
 ** @code
 ** // Full style specification
 ** style l_val(color::red, color::black, attrs::underlined | attrs::bold);
 **
 ** // Foreground and background specification
 ** style l_val(color::red, color::black);
 **
 ** // Foreground effects specification
 ** style l_val(color::red, attrs::bold);
 **
 ** // Foreground specification
 ** style l_val(color::red);
 **
 ** // Default style
 ** style l_val;
 ** @endcode
 */
struct style
{
public:
  /**
   ** @brief Full specification constructor with default values
   ** @param p_fg foreground color (default unset)
   ** @param p_bg background color (default unset)
   ** @param p_attrs effect attributes (default unset)
   */
  style(const color& p_fg    = color::unset,
        const color& p_bg    = color::unset,
        const attrs& p_attrs = attrs::unset);

  /**
   ** @brief Foreground and effect specification constructor
   ** @param p_fg foreground color
   ** @param p_attrs effect attributes
   */
  style(const color& p_fg,
        const attrs& p_attrs);

public:
  /**
   ** @brief Returns foreground color
   */
  const color& getFgColor(void) const;

  /**
   ** @brief Returns background color
   */
  const color& getBgColor(void) const;

  /**
   ** @brief Returns effect attributes
   */
  const attrs& getAttrs(void)   const;

  /**
   ** @brief Sets forground color
   ** @param p_fg foreground color
   */
  void setFgColor(const color& p_fg);

  /**
   ** @brief Sets background color
   ** @param p_bg background color
   */
  void setBgColor(const color& p_bg);

  /**
   ** @brief Sets effect attributes
   ** @param p_attrs effect attributes
   */
  void setAttrs(const attrs& p_attrs);

private:
  color m_fg;
  color m_bg;
  attrs m_attrs;
};

/**
 ** @brief Generates ANSI codes for given text and style
 ** @details
 **
 ** This object defines several constructors allowing to define only foreground
 ** colors, foreground and background colors, colors and attributes or only
 ** foreground color and attributes.
 **
 ** The method @ref Text::str return the full ANSI espace sequence for given text
 ** and style options.
 **
 ** It also defines an implicit *string* cast operator allowing to use this object
 ** as a genuine string.
 **
 ** Example :
 ** @code
 ** Text   l_msg = Text("some text in red", color::red);
 ** string l_val = l_msg;
 ** @endcode
 */
class Text
{
public:
  /**
   ** @brief Full specification constructor
   ** @param p_str Text string content
   ** @param p_fg TTY foreground color
   ** @param p_bg TTY background color
   ** @param p_attrs TTY attributes
   */
  Text(const string& p_str,
       const color& p_fg    = color::unset,
       const color& p_bg    = color::unset,
       const attrs& p_attrs = attrs::unset);

  /**
   ** @brief Partial specification constructor
   ** @param p_str Text string content
   ** @param p_fg TTY foreground color
   ** @param p_attrs TTY attributes
   */
  Text(const string& p_str,
       const color& p_fg,
       const attrs& p_attrs);

  /**
   ** @brief Construct from text and style object
   ** @param p_str Text string content
   ** @param p_style TTY style object
   */
  Text(const string& p_str,
       const style& p_style);

public:
  /**
   ** @brief Generates terminal color codes
   */
  string str(void) const;

  /**
   ** @brief Implicit string cast operator
   */
  operator string() const;

private:
  string m_str;
  style  m_style;
};

/**
 ** @brief Outputs @ref Text to ostream
 ** @param p_buf input stream
 ** @param p_text input Text
 */
ostream& operator<<(ostream& p_buf, const Text& p_text);


/**
 ** @brief Outputs @ref color to ostream
 ** @param p_buf input stream
 ** @param p_color input color
 */
ostream& operator<<(ostream& p_buf, const color& p_color);


/**
 ** @brief Outputs @ref attrs to ostream
 ** @param p_buf input stream
 ** @param p_attrs input attrs
 */
ostream& operator<<(ostream& p_buf, const attrs& p_attrs);



}}

#endif // !XTD_CORE_TTY_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
