#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/Formatter.hh"       //libcore

using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestFormatter : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestFormatter);
  CPPUNIT_TEST(addFormatModifier);
  CPPUNIT_TEST(format);
  CPPUNIT_TEST(create);
  CPPUNIT_TEST_SUITE_END();


  struct testfunctor
  {
    testfunctor(bool& p_target) :
      m_target(p_target) { }
    void operator()(const FormattedRecord&, Formatter::t_formats&)
    {
      m_target = true;
    }
    bool& m_target;
  };

private:
  void addFormatModifier(void);
  void format(void);
  void create(void);

private:
  Record rec(level p_level, const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestFormatter);

Record
TestFormatter::rec(level p_level, const std::string& p_content) const
{
  Record          l_rec;
  l_rec.m_format = p_content;
  l_rec.m_level  = p_level;

  return l_rec;
}


void
TestFormatter::format(void)
{

  {
    // format with default location
    Formatter l_f1;
    l_f1.setLayout("fulllog", "%(location)");
    auto l_frec = l_f1.format(rec(level::debug, "message"), "main()", "myfile", 10);
    CPPUNIT_ASSERT_EQUAL(string("at myfile:10 -> main()"), l_frec.m_fulllog);
  }

  {
    // format with default location
    Formatter l_f1;
    l_f1.setLayout("fulllog", "%(location)");
    l_f1.setLayout("location", "%(line) -> %(filename)");
    auto l_frec = l_f1.format(rec(level::debug, "message"), "main()", "myfile", 10);
    CPPUNIT_ASSERT_EQUAL(string("10 -> myfile"), l_frec.m_fulllog);
  }

  {
    // format with wrong number of parameters
    Formatter       l_f1;
    FormattedRecord l_frec;

    l_f1.setLayout("fulllog", "%(message)");
    CPPUNIT_ASSERT_NO_THROW(l_frec = l_f1.format(rec(level::debug, "message"), "main()", "myfile"));
    CPPUNIT_ASSERT_EQUAL(true, boost::contains(l_frec.m_fulllog, "wrong number of arguments"));
  }

  {
    // format with bad format
    Formatter l_f2;
    Formatter l_f1(l_f2);
    l_f1.setLayout("fulllog", "%(message)");
    auto l_frec = l_f1.format(rec(level::debug, "my message is %1% %2"), string("string"));
    CPPUNIT_ASSERT_EQUAL(true, boost::contains(l_frec.m_fulllog, "unable to render format"));
  }
}



void
TestFormatter::addFormatModifier(void)
{
  bool l_called;
  Record l_rec = rec(level::debug, "message");

  auto l_fn = [&](const FormattedRecord&, Formatter::t_formats& p_fmt) {
    CPPUNIT_ASSERT_EQUAL(true, p_fmt.set("message", "%-10s"));
    l_called = true;
  };

  {
    Formatter l_f1;
    l_f1.setLayout("fulllog", "%(message)");
    CPPUNIT_ASSERT_EQUAL(0ul, l_f1.m_formatModifiers.size());
    l_f1.addFormatModifier(l_fn);
    CPPUNIT_ASSERT_EQUAL(1ul, l_f1.m_formatModifiers.size());
    l_called = false;
    auto l_frec = l_f1.format(l_rec);
    CPPUNIT_ASSERT_EQUAL(string("message   "), l_frec.m_fulllog);
    CPPUNIT_ASSERT_EQUAL(true, l_called);
  }


  {
    Formatter l_f1;
    CPPUNIT_ASSERT_EQUAL(0ul, l_f1.m_formatModifiers.size());
    l_f1.addFormatModifier(testfunctor(l_called));
    CPPUNIT_ASSERT_EQUAL(1ul, l_f1.m_formatModifiers.size());
    l_called = false;
    l_f1.format(l_rec);
    CPPUNIT_ASSERT_EQUAL(true, l_called);
  }
}


void
TestFormatter::create(void)
{
  map<string,string> l_props;

  {
    // OK no params
    l_props = {};
    CPPUNIT_ASSERT_NO_THROW(auto l_f1 = Formatter::create("f1", l_props));
  }

  {
    // OK override layout
    l_props = {
      { "log.formatter.f1.layout.fulllog", "%(message)" }
    };
    sptr<Formatter> l_f1;
    CPPUNIT_ASSERT_NO_THROW(l_f1 = Formatter::create("f1", l_props));
    CPPUNIT_ASSERT_EQUAL(string("%(message)"), l_f1->m_fmt);
  }


  {
    // OK override formats
    l_props = {
      { "log.formatter.f1.format.message",  "%10s" },
      { "log.formatter.f1.format.function", "%30s" }
    };
    sptr<Formatter> l_f1;
    CPPUNIT_ASSERT_NO_THROW(l_f1 = Formatter::create("f1", l_props));
    CPPUNIT_ASSERT_EQUAL(string("%10s"), l_f1->m_fieldFormats.m_message);
    CPPUNIT_ASSERT_EQUAL(string("%30s"), l_f1->m_fieldFormats.m_function);
  }
}

XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
