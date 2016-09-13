#include <boost/filesystem.hpp>
#include "MainTestApplication.hh" // libtests
#include "CWrap.hh"               // libtests
#include "ConfParser.hh"          // libcommon
#include "error.hh"               // libcommon


using namespace std;
using namespace xtd::tests;
using xtd::ConfParser;

class TestConfParser : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestConfParser);
  CPPUNIT_TEST(Constructor);
  CPPUNIT_TEST_SUITE_END();
private:

  void Constructor(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestConfParser);

void
TestConfParser::Constructor(void)
{
  {
    // KO file not exsist
    CPPUNIT_ASSERT_THROW(ConfParser l_obj("doesnotexist"), xtd::error);
  }

  {
    // KO unreadable existing file
    CPPUNIT_ASSERT_THROW(ConfParser l_obj("/dev/console"), xtd::error);
  }

  {
    // KO read error
    CWrap::state l_state("fread", {{ "error", 1 }});

    CPPUNIT_ASSERT_THROW(ConfParser l_obj("/dev/random"), xtd::error);
  }

  {
    // OK read
    string l_config = Globals::get().testDir() + "/data/simple.conf";
    CPPUNIT_ASSERT_NO_THROW(ConfParser l_obj(l_config));
  }


  xtd::config_parser<std::string::const_iterator> l_grammar;
  sections l_result;
  string l_data = R"data(
  [s1]
  {
    s1key1 : s1key1val
    [s1s2]
    {
      s1s2key1 : s1s2key1val
      s1s2key2 : s1s2key2val
    }
  }
  )data";

  string::const_iterator l_start = l_data.begin();
  string::const_iterator l_end   = l_data.end();
  CPPUNIT_ASSERT_EQUAL(true, phrase_parse(l_start, l_end, l_grammar, boost::spirit::ascii::space, l_result));
  CPPUNIT_ASSERT(l_start == l_end);

  l_start = l_data.begin() + 5;
  l_end   = l_data.end();
  CPPUNIT_ASSERT_EQUAL(true, phrase_parse(l_start, l_end, l_grammar, boost::spirit::ascii::space, l_result));
  CPPUNIT_ASSERT(l_start != l_end);


  // string::const_iterator l_start = l_data.begin();
  // string::const_iterator l_end   = l_data.end();
  // CPPUNIT_ASSERT_EQUAL(true, phrase_parse(l_start, l_end, l_grammar, boost::spirit::ascii::space, l_result));
  // CPPUNIT_ASSERT_EQUAL(l_start, l_end);
}

XTD_TEST_MAIN();
