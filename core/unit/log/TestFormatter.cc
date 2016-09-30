#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/Formatter.hh"  //libcore
#include <boost/filesystem.hpp>
#include <boost/any.hpp>


using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestFormatter : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestFormatter);
  CPPUNIT_TEST(addFormatModifier);
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



XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
