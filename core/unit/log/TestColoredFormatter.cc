#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh>  //libtests
#include <TestFixture.hh>          //libtests
#include <CWrap.hh>                //libtests
#include "log/ColoredFormatter.hh" //libcore
#include "tty.hh"                  //libcore

using namespace xtd;
using namespace xtd::log;
using namespace xtd::tty;
using namespace xtd::tests;

class TestColoredFormatter : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestColoredFormatter);
  CPPUNIT_TEST(format);
  CPPUNIT_TEST(create);
  CPPUNIT_TEST_SUITE_END();

private:
  void format(void);
  void create(void);

private:
  Record rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestColoredFormatter);

Record
TestColoredFormatter::rec(level p_level, const std::string& p_content) const
{
  Record          l_rec;
  l_rec.m_format = p_content;
  l_rec.m_level  = p_level;

  return l_rec;
}


void
TestColoredFormatter::format(void)
{
  {
    // format with full field spec
    ColoredFormatter l_f1;
    l_f1.setLayout("fulllog", "%(message)");
    l_f1.setStyle("message", color::red, color::green, attrs::bold | attrs::blink);

    auto l_frec = l_f1.format(rec(level::debug, "content"));
    CPPUNIT_ASSERT_EQUAL(string("\e[31m\e[42m\e[1m\e[5mcontent\e[0m"), l_frec.m_fulllog);
  }

  {
    // format with mutiple field spec
    ColoredFormatter l_f1;
    l_f1.setLayout("fulllog", "%(message)");
    l_f1.setFgColor("message", color::red);
    l_f1.setBgColor("message", color::green);
    l_f1.setAttrs("message", attrs::bold | attrs::blink);
    auto l_frec = l_f1.format(rec(level::debug, "content"));
    CPPUNIT_ASSERT_EQUAL(string("\e[31m\e[42m\e[1m\e[5mcontent\e[0m"), l_frec.m_fulllog);
  }

  {
    // format with modifier
    ColoredFormatter l_f1;
    l_f1.setLayout("fulllog", "%(message)");
    l_f1.setStyle("message", color::red, color::green, attrs::bold | attrs::blink);
    l_f1.setStyle("fulllog", color::unset, color::unset, attrs::unset);

    auto l_fn = [](const FormattedRecord& p_rec, Fields<style>& p_styles) {
      if (level::crit == p_rec.m_level) {
        p_styles.get("message").setFgColor(color::blue);
        p_styles.get("message").setBgColor(color::red);
      }
    };

    l_f1.addStyleModifier(l_fn);
    auto l_frec = l_f1.format(rec(level::debug, "content"));
    CPPUNIT_ASSERT_EQUAL(string("\e[31m\e[42m\e[1m\e[5mcontent\e[0m"), l_frec.m_fulllog);
    l_frec = l_f1.format(rec(level::crit, "content"));
    CPPUNIT_ASSERT_EQUAL(string("\e[34m\e[41m\e[1m\e[5mcontent\e[0m"), l_frec.m_fulllog);
  }
}


void
TestColoredFormatter::create(void)
{
  map<string,string> l_props;

  {
    // OK no params
    l_props = {};
    CPPUNIT_ASSERT_NO_THROW(auto l_f1 = ColoredFormatter::create("f1", l_props));
  }

  {
    // OK override style
    l_props = {
      { "log.formatter.f1.style.message.fgColor",  "red"},
      { "log.formatter.f1.style.message.bgColor",  "black"},
      { "log.formatter.f1.style.message.attrs",    "bold | blink"}
    };

    sptr<Formatter>        l_f1;
    sptr<ColoredFormatter> l_cf1;
    CPPUNIT_ASSERT_NO_THROW(l_f1 = ColoredFormatter::create("f1", l_props));
    CPPUNIT_ASSERT_NO_THROW(l_cf1 = std::static_pointer_cast<ColoredFormatter>(l_f1));
    CPPUNIT_ASSERT_EQUAL(color::red,                 l_cf1->m_fieldStyles.m_message.getFgColor());
    CPPUNIT_ASSERT_EQUAL(color::black,               l_cf1->m_fieldStyles.m_message.getBgColor());
    CPPUNIT_ASSERT_EQUAL(attrs::bold | attrs::blink, l_cf1->m_fieldStyles.m_message.getAttrs());
  }

  {
    // KO invalid foreground color
    l_props = {
      { "log.formatter.f1.style.message.fgColor",  "unknown"}
    };
    CPPUNIT_ASSERT_THROW(ColoredFormatter::create("f1", l_props), log_error);
  }


  {
    // KO invalid background color
    l_props = {
      { "log.formatter.f1.style.message.bgColor",  "unknown"}
    };
    CPPUNIT_ASSERT_THROW(ColoredFormatter::create("f1", l_props), log_error);
  }

  {
    // KO invalid attribute
    l_props = {
      { "log.formatter.f1.style.message.attrs",  "unknown"}
    };
    CPPUNIT_ASSERT_THROW(ColoredFormatter::create("f1", l_props), log_error);
  }
}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
