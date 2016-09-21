#ifndef CORE_LOG_COLOREDFORMATTER_HH_
# define CORE_LOG_COLOREDFORMATTER_HH_
# include "log/fwd.hh"
# include "log/logtypes.hh"
# include "log/Formatter.hh"
# include "tty.hh"
# include "log/Fields.hh"

namespace xtd {
namespace log {


class ColoredFormatter : public Formatter
{
  friend class ConfLoader;

public:
  typedef Fields<tty::style> t_styles;

protected:
  typedef fn<void(const FormattedRecord&, t_styles&)> t_style_modifier;

public:
  ColoredFormatter(void);
  ColoredFormatter(const Formatter& p_base);
  virtual ~ColoredFormatter(void);

protected:
  static sptr<Formatter> create(const string&             p_name,
                                const map<string,string>& p_properties);

public:
  ColoredFormatter& setAttrs(const string&     p_field,
                             const tty::attrs& p_attrs);

  ColoredFormatter& setFgColor(const string&     p_field,
                               const tty::color& p_fg);

  ColoredFormatter& setBgColor(const string&     p_field,
                               const tty::color& p_bg);

  ColoredFormatter& setStyle(const string&     p_field,
                             const tty::style& p_style);

  ColoredFormatter& setStyle(const string&     p_field,
                             const tty::color& p_fg,
                             const tty::color& p_bg,
                             const tty::attrs& p_attrs);

  ColoredFormatter& setStyles(const map<string, tty::style>& p_styles);

  template<typename Handler>
  ColoredFormatter& addStyleModifier(Handler p_modifier);

protected:
  virtual void   createFields(const FormattedRecord& p_rec, Fields<string>& p_args) const override;
  virtual string getFulllog(const FormattedRecord& p_rec)                           const override;

protected:
  t_styles                  m_fieldStyles;
  vector<t_style_modifier>  m_styleModifiers;
};


template<typename Handler>
ColoredFormatter&
ColoredFormatter::addStyleModifier(Handler p_functor)
{
  t_style_modifier l_modifier = [=](const FormattedRecord& p_rec, t_styles& p_format) mutable {
    p_functor(p_rec, p_format);
  };
  m_styleModifiers.push_back(l_modifier);
  return *this;
}

}}

#endif // !CORE_LOG_COLOREDFORMATTER_HH_
