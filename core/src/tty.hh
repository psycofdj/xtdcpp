#ifndef XTD_CORE_TTY_HH_
# define XTD_CORE_TTY_HH_
# include "types.hh"
# include <boost/format.hpp>
# include <boost/algorithm/string/join.hpp>

namespace xtd {
namespace tty {

class Text;

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
  static const attrs unset;
  static const attrs reset;
  static const attrs dim;
  static const attrs bold;
  static const attrs underlined;
  static const attrs blink;
  static const attrs reverse;
  static const attrs hidden;

private:
  attrs(void);
  attrs(s p_attr);

public:
  static bool from_string(const string& p_value, attrs& p_attrs);

public:
  attrs                   operator|(const attrs& p_attrs) const;
  const vector<uint32_t>& getValues(void) const;
  bool                    isSet(void) const;

private:
  vector<uint32_t> m_attrs;
};

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
    unset      = 0,
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
  color(uint32_t p_value);

public:
  static bool from_string(const string& p_value, color& p_attrs);

public:
  bool     isExtended(void) const;
  uint32_t getValue(void) const;
  bool     isSet(void) const;

private:
  uint32_t m_value;
  bool     m_extended;
};


struct style
{
public:
  style(const color& p_fg    = color::unset,
        const color& p_bg    = color::unset,
        const attrs& p_attrs = attrs::unset);

  style(const color& p_fg,
        const attrs& p_attrs);

public:
  const color& getFgColor(void) const;
  const color& getBgColor(void) const;
  const attrs& getAttrs(void)   const;
  void         setFgColor(const color& p_fg);
  void         setBgColor(const color& p_bg);
  void         setAttrs(const attrs& p_attrs);

private:
  color m_fg;
  color m_bg;
  attrs m_attrs;
};

class Text
{
public:
  Text(const string& p_str,
       const color& p_fg    = color::unset,
       const color& p_bg    = color::unset,
       const attrs& p_attrs = attrs::unset);

  Text(const string& p_str,
       const color& p_fg,
       const attrs& p_attrs);

  Text(const string& p_str,
       const style& p_style);

public:
  string str(void) const;
  operator string() const;

private:
  string m_str;
  style  m_style;
};

ostream& operator<<(ostream& p_buf, const Text& p_text);


}}

#endif // !XTD_CORE_TTY_HH_
