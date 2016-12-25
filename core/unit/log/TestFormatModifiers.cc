#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <MainTestApplication.hh>  //libtests
#include <TestFixture.hh>          //libtests
#include <CWrap.hh>                //libtests
#include "log/Formatter.hh"        //libcore
#include "log/ColoredFormatter.hh" //libcore
#include "log/FormatModifiers.hh"  //libcore
#include "tty.hh"                  //libcore

using namespace xtd;
using namespace xtd::log;
using namespace xtd::tty;
using namespace xtd::tests;

class TestFormatModifiers : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestFormatModifiers);
  CPPUNIT_TEST(autoWitdh);
  CPPUNIT_TEST(styleByLevel);
  CPPUNIT_TEST(styleMatch);
  CPPUNIT_TEST_SUITE_END();

private:
  void autoWitdh(void);
  void styleByLevel(void);
  void styleMatch(void);

private:
  Record rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestFormatModifiers);

Record
TestFormatModifiers::rec(level p_level, const std::string& p_content) const
{
  Record          l_rec;
  l_rec.m_format = p_content;
  l_rec.m_level  = p_level;

  return l_rec;
}

void
TestFormatModifiers::autoWitdh(void)
{
  {
    // format with full field spec
    Formatter l_f1;
    l_f1.setLayout("fulllog", "%(message)");
    l_f1.addFormatModifier(AutoWidth("message"));

    // -> size 1
    auto l_frec = l_f1.format(rec(level::debug, "x"));
    CPPUNIT_ASSERT_EQUAL(string("x"), l_frec.m_fulllog);
    // -> size 4
    l_frec = l_f1.format(rec(level::debug, "xxxx"));
    CPPUNIT_ASSERT_EQUAL(string("xxxx"), l_frec.m_fulllog);
    // -> size 1, print as 4
    l_frec = l_f1.format(rec(level::debug, "x"));
    CPPUNIT_ASSERT_EQUAL(string("x   "), l_frec.m_fulllog);
  }
}

void
TestFormatModifiers::styleByLevel(void)
{
  using namespace xtd::tty;

  {
    // format with full field spec
    ColoredFormatter l_f1;
    StyleByLevel     l_styler({
        { level::info,   style(color::red)   },
        { level::notice, style(color::green) }
      });

    l_f1.setLayout("fulllog", "%(slevel)");
    l_f1.setStyle("slevel",   style());
    l_f1.setFieldFormat("slevel",   "%s");
    l_f1.addStyleModifier(l_styler);

    // -> no match
    auto l_frec = l_f1.format(rec(level::debug, "msg"));
    CPPUNIT_ASSERT_EQUAL(string("debug"), l_frec.m_fulllog);

    // -> match info, should be red
    l_frec = l_f1.format(rec(level::info, "msg"));
    CPPUNIT_ASSERT_EQUAL(string("\e[31minfo\e[0m"), l_frec.m_fulllog);

    // -> match notice, should be green
    l_frec = l_f1.format(rec(level::notice, "msg"));
    CPPUNIT_ASSERT_EQUAL(string("\e[32mnotice\e[0m"), l_frec.m_fulllog);
  }
}

void
TestFormatModifiers::styleMatch(void)
{

  {
    // format with full field spec
    ColoredFormatter l_f1;
    StyleMatch       l_styler(boost::regex(".*tag.*"), style(color::red));

    l_f1.setLayout("fulllog", "%(message)");
    l_f1.setStyle("message",  style());
    l_f1.addStyleModifier(l_styler);

    // -> no match
    auto l_frec = l_f1.format(rec(level::debug, "msg"));
    CPPUNIT_ASSERT_EQUAL(string("msg"), l_frec.m_fulllog);

    // -> match info, should be red
    l_frec = l_f1.format(rec(level::info, "got tag in msg"));
    CPPUNIT_ASSERT_EQUAL(string("\e[31mgot tag in msg\e[0m"), l_frec.m_fulllog);
  }
}


XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
