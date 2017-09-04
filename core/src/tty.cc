#include "tty.hh"
#include <boost/assign/std/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <iterator>


namespace xtd {
namespace tty {

const color color::unset      = color(c::unset);
const color color::normal     = color(c::normal);
const color color::black      = color(c::black);
const color color::red        = color(c::red);
const color color::green      = color(c::green);
const color color::yellow     = color(c::yellow);
const color color::blue       = color(c::blue);
const color color::magenta    = color(c::magenta);
const color color::cyan       = color(c::cyan);
const color color::gray       = color(c::gray);
const color color::lblack     = color(c::lblack);
const color color::lred       = color(c::lred);
const color color::lgreen     = color(c::lgreen);
const color color::lyellow    = color(c::lyellow);
const color color::lblue      = color(c::lblue);
const color color::lmagenta   = color(c::lmagenta);
const color color::lcyan      = color(c::lcyan);
const color color::lgray      = color(c::lgray);
const color color::white      = color(c::white);
const attrs attrs::unset      = attrs();
const attrs attrs::reset      = attrs(s::reset);
const attrs attrs::dim        = attrs(s::dim);
const attrs attrs::bold       = attrs(s::bold);
const attrs attrs::underlined = attrs(s::underlined);
const attrs attrs::blink      = attrs(s::blink);
const attrs attrs::reverse    = attrs(s::reverse);
const attrs attrs::hidden     = attrs(s::hidden);

/* -------------------------------------------------------------------------- */

bool
attrs::from_string(const string& p_value, attrs& p_attrs)
{
  const static map<string, attrs> l_values = {
    { "",           unset      },
    { "unset",      unset      },
    { "reset",      reset      },
    { "dim",        dim        },
    { "bold",       bold       },
    { "underlined", underlined },
    { "blink",      blink      },
    { "reverse",    reverse    },
    { "hidden",     hidden     }
  };

  auto c_item = l_values.find(p_value);
  if (l_values.end() == c_item)
    return false;

  p_attrs = c_item->second;
  return true;
}

string
attrs::to_string(const attrs& p_attrs)
{
  const static map<typename std::underlying_type<s>::type, string> l_values = {
    { valueof(s::dim),        "dim"        },
    { valueof(s::bold),       "bold"       },
    { valueof(s::underlined), "underlined" },
    { valueof(s::blink),      "blink"      },
    { valueof(s::reverse),    "reverse"    },
    { valueof(s::hidden),     "hidden"     }
  };

  if (0 == p_attrs.m_attrs.size())
    return "unset";

  vector<string> l_parts;
  for (auto c_attr : p_attrs.m_attrs) {
    auto c_item = l_values.find(c_attr);
    if (l_values.end() != c_item)
      l_parts.push_back(c_item->second);
  }
  return boost::join(l_parts, " | ");
}



attrs::attrs(void) :
  m_attrs()
{
}

attrs::attrs(s p_attr) :
  m_attrs(1, valueof(p_attr))
{
}

attrs
attrs::operator|(const attrs& p_o1) const
{
  attrs l_res;
  std::copy(m_attrs.begin(), m_attrs.end(), std::back_inserter(l_res.m_attrs));
  std::copy(p_o1.m_attrs.begin(), p_o1.m_attrs.end(), std::back_inserter(l_res.m_attrs));
  std::sort(l_res.m_attrs.begin(), l_res.m_attrs.end());

  auto c_iter = std::unique(l_res.m_attrs.begin(), l_res.m_attrs.end());
  auto l_size = std::distance(l_res.m_attrs.begin(), c_iter);
  l_res.m_attrs.resize(l_size);
  return l_res;
}

attrs&
attrs::operator|=(const attrs& p_o1)
{
  std::copy(p_o1.m_attrs.begin(), p_o1.m_attrs.end(), std::back_inserter(m_attrs));
  std::sort(m_attrs.begin(), m_attrs.end());

  auto c_iter = std::unique(m_attrs.begin(), m_attrs.end());
  auto l_size = std::distance(m_attrs.begin(), c_iter);
  m_attrs.resize(l_size);
  return *this;
}

bool
attrs::operator==(const attrs& p_obj) const
{
  vector<uint32_t> l_this(m_attrs);
  vector<uint32_t> l_foreign(p_obj.m_attrs);
  std::sort(l_this.begin(), l_this.end());
  std::sort(l_foreign.begin(), l_foreign.end());
  l_this   .resize(std::distance(l_this.begin(),    std::unique(l_this.begin(),    l_this.end())));
  l_foreign.resize(std::distance(l_foreign.begin(), std::unique(l_foreign.begin(), l_foreign.end())));
  return ((l_this.size() == l_foreign.size()) &&
          std::equal(l_this.begin(), l_this.end(), l_foreign.begin()));
}


const vector<uint32_t>&
attrs::getValues(void) const
{
  return m_attrs;
}

bool
attrs::isSet(void) const
{
  return (false == m_attrs.empty());
}

ostream& operator<<(ostream& p_buf, const attrs& p_attrs)
{
  return p_buf << attrs::to_string(p_attrs);
}


/* -------------------------------------------------------------------------- */

color::color(c p_color) :
  m_value(valueof(p_color)),
  m_extended(false)
{
}

color::color(uint32_t p_value) :
  m_value(p_value),
  m_extended(true)
{
}

bool
color::operator==(const color& p_obj) const
{
  return m_value == p_obj.getValue();
}


bool
color::isSet(void) const
{
  return m_value != 0;
}

bool
color::isExtended(void) const
{
  return m_extended;
}

uint32_t
color::getValue(void) const
{
  return m_value;
}

bool
color::from_string(const string& p_value, color& p_color)
{
  const static map<string, color> l_values = {
    { "unset",    unset    },
    { "normal",   normal   },
    { "black",    black    },
    { "red",      red      },
    { "green",    green    },
    { "yellow",   yellow   },
    { "blue",     blue     },
    { "magenta",  magenta  },
    { "cyan",     cyan     },
    { "gray",     gray     },
    { "lblack",   lblack   },
    { "lred",     lred     },
    { "lgreen",   lgreen   },
    { "lyellow",  lyellow  },
    { "lblue",    lblue    },
    { "lmagenta", lmagenta },
    { "lcyan",    lcyan    },
    { "lgray",    lgray    },
    { "white",    white    }
  };

  auto c_item = l_values.find(p_value);
  if (l_values.end() != c_item)
  {
    p_color = c_item->second;
    return true;
  }

  try {
    uint32_t l_value = boost::lexical_cast<uint32_t>(p_value);
    p_color = color(l_value);
    return true;
  } catch (boost::bad_lexical_cast&) {
    return false;
  }
}


string
color::to_string(const color& p_color)
{
  const static map<typename std::underlying_type<c>::type, string> l_values = {
    { valueof(c::unset),    "unset"    },
    { valueof(c::normal),   "normal"   },
    { valueof(c::black),    "black"    },
    { valueof(c::red),      "red"      },
    { valueof(c::green),    "green"    },
    { valueof(c::yellow),   "yellow"   },
    { valueof(c::blue),     "blue"     },
    { valueof(c::magenta),  "magenta"  },
    { valueof(c::cyan),     "cyan"     },
    { valueof(c::gray),     "gray"     },
    { valueof(c::lblack),   "lblack"   },
    { valueof(c::lred),     "lred"     },
    { valueof(c::lgreen),   "lgreen"   },
    { valueof(c::lyellow),  "lyellow"  },
    { valueof(c::lblue),    "lblue"    },
    { valueof(c::lmagenta), "lmagenta" },
    { valueof(c::lcyan),    "lcyan"    },
    { valueof(c::lgray),    "lgray"    },
    { valueof(c::white),    "white"    }
  };

  auto c_item = l_values.find(p_color.getValue());
  if (l_values.end() != c_item)
    return c_item->second;
  return "color<" + to_string(p_color.getValue()) + ">";
}

ostream& operator<<(ostream& p_buf, const color& p_color)
{
  return p_buf << color::to_string(p_color);
}


/* -------------------------------------------------------------------------- */

style::style(const color& p_fg, const color& p_bg, const attrs& p_attrs) :
  m_fg(p_fg),
  m_bg(p_bg),
  m_attrs(p_attrs)
{
}

style::style(const color& p_fg, const attrs& p_attrs) :
  m_fg(p_fg),
  m_bg(color::unset),
  m_attrs(p_attrs)
{
}

const color&
style::getFgColor(void) const
{
  return m_fg;
}

const color&
style::getBgColor(void) const
{
  return m_bg;
}

const attrs&
style::getAttrs(void)   const
{
  return m_attrs;
}

void
style::setFgColor(const color& p_fg)
{
  m_fg = p_fg;
}

void
style::setBgColor(const color& p_bg)
{
  m_bg = p_bg;
}

void
style::setAttrs(const attrs& p_attrs)
{
  m_attrs = p_attrs;
}


/* -------------------------------------------------------------------------- */

Text::Text(const string& p_str,
           const color& p_fg,
           const color& p_bg,
           const attrs& p_attrs) :
  m_str(p_str),
  m_style(p_fg, p_bg, p_attrs)
{
}

Text::Text(const string& p_str,
           const color& p_fg,
           const attrs& p_attrs) :
  m_str(p_str),
  m_style(p_fg, p_attrs)
{
}


Text::Text(const string& p_str,
           const style& p_style) :
  m_str(p_str),
  m_style(p_style)
{
}

Text::operator string() const
{
  return str();
}

string
Text::str(void) const
{
  using boost::assign::operator+=;
  using boost::str;
  using boost::format;

  vector<string> l_escapes;

  if (true == m_style.getFgColor().isSet())
  {
    if (false == m_style.getFgColor().isExtended())
      l_escapes += str(format("\e[%dm") % m_style.getFgColor().getValue());
    else
      l_escapes += str(format("\e[38;5;%dm") % m_style.getFgColor().getValue());
  }

  if (true == m_style.getBgColor().isSet())
  {
    if (false == m_style.getBgColor().isExtended())
      l_escapes += str(format("\e[%dm") % (m_style.getBgColor().getValue() + 10));
    else
      l_escapes += str(format("\e[48;5;%dm") % m_style.getBgColor().getValue());
  }

  for (auto c_attrs : m_style.getAttrs().getValues())
    l_escapes += str(format("\e[%dm") % c_attrs);

  l_escapes += m_str;

  if (l_escapes.size() > 1)
    l_escapes += "\e[0m";

  return boost::join(l_escapes, "");
}

ostream& operator<<(ostream& p_buf, const Text& p_text)
{
  p_buf << p_text.str();
  return p_buf;
}

}}
