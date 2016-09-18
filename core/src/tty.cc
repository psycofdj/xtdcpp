#include "tty.hh"
#include <boost/assign/std/vector.hpp>
#include <boost/lexical_cast.hpp>

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

attrs::attrs(void) :
  m_attrs()
{
}

attrs::attrs(s p_attr) :
  m_attrs(1, static_cast<uint32_t>(p_attr))
{
}

attrs
attrs::operator|(const attrs& p_attrs) const
{
  attrs l_res;
  std::copy(m_attrs.begin(), m_attrs.end(), std::back_inserter(l_res.m_attrs));
  std::copy(p_attrs.m_attrs.begin(), p_attrs.m_attrs.end(), std::back_inserter(l_res.m_attrs));
  return l_res;
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

/* -------------------------------------------------------------------------- */

color::color(c p_color) :
  m_value(static_cast<uint32_t>(p_color)),
  m_extended(false)
{
}

color::color(uint32_t p_value) :
  m_value(p_value),
  m_extended(true)
{
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
