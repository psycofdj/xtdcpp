#ifndef XTD_CORE_TTY_HH_
# define XTD_CORE_TTY_HH_
# include "types.hh"
# include <boost/format.hpp>
# include <boost/algorithm/string/join.hpp>

namespace xtd {


/**
 ** @brief Manipulate text on tty environment
 */
namespace tty {

/**
 ** @brief Holds a TTY display attributes
 ** @details
 ** Available options are :
 ** - attrs::unset       : @copybrief attrs::unset
 ** - attrs::reset       : @copybrief attrs::reset
 ** - attrs::dim         : @copybrief attrs::dim
 ** - attrs::bold        : @copybrief attrs::bold
 ** - attrs::underlined  : @copybrief attrs::underlined
 ** - attrs::blink       : @copybrief attrs::blink
 ** - attrs::reverse     : @copybrief attrs::reverse
 ** - attrs::hidden      : @copybrief attrs::hidden
 **
 ** These constants can be combined with attrs::operator|.
 ** @code
 ** attrs l_val;
 **
 ** l_val  = attrs::bold;
 ** l_val  = attrs::bold | attrs::underlined;
 ** l_val |= reset;
 ** @endcode
 */
class attrs
{
private:
  enum class s : uint32_t {
    reset       = 0,
    bold       = 1,
    dim        = 2,
    underlined = 4,
    blink      = 5,
    reverse    = 7,
    hidden     = 8
  };

public:
  /*** @brief Default attribute, no effect */
  static const attrs unset;
  /*** @brief Remove all other effects */
  static const attrs reset;
  /*** @brief Reduced weight effect */
  static const attrs dim;
  /*** @brief Increased weight effect */
  static const attrs bold;
  /*** @brief Underlined effect */
  static const attrs underlined;
  /*** @brief Blinking effect */
  static const attrs blink;
  /*** @brief Revered foreground and background effect */
  static const attrs reverse;
  /*** @brief Hidden font effect */
  static const attrs hidden;

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

public:
  /**
   ** @brief Add given attributes to current object
   ** @param p_attrs attributes to add
   */
  attrs operator|(const attrs& p_attrs) const;

  /**
   ** @brief Returns internal values
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
 ** @brief Represents a TTY color
 ** @details
 ** There's two types of TTY colors, standard predefined and extended.
 ** - Predefined colors are built from constants and can be displayed on 16-colors terminals.
 ** - Extended colors are build from color(uint32_t) constructor and their compatibility
 **   depends on terminal capabilities.
 */
class color
{
public:
  static const color unset;
  static const color normal;
  static const color black;
  static const color red;
  static const color green;
  static const color yellow;
  static const color blue;
  static const color magenta;
  static const color cyan;
  static const color gray;
  static const color lblack;
  static const color lred;
  static const color lgreen;
  static const color lyellow;
  static const color lblue;
  static const color lmagenta;
  static const color lcyan;
  static const color lgray;
  static const color white;

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

public:
  /**
   ** @brief Tells if color is extended
   */
  bool     isExtended(void) const;

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
 ** @brief TTY Style information for a chunk of text
 ** @details
 ** - Foreground color
 ** - Background color
 ** - Effects attributes
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
 ** @brief Chunk of TTY text
 ** @details
 ** Generates the TTY colors codes depending of its style and string content.
 ** Defines an implicit "to string" cast operator.
 **
 ** Usage example :
 ** @code
 ** Text l_msg = Text("some text in red", color::red);
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
 ** @brief Outputs Text to ostream
 ** @param p_buf input stream
 ** @param p_text input Text
 */
ostream& operator<<(ostream& p_buf, const Text& p_text);


}}

#endif // !XTD_CORE_TTY_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
