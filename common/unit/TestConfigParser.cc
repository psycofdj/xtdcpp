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




  xtd::ConfigParser2::sections l_result;
  string l_data = R"data(
  [s1]
  {
    s1key1 : s1key1val
    [s1s2]
    {
      s1s2key1 : s1s2key1val
      s1s2key2 : s1s2key2val
    }
  })data";



  xtd::ConfigParser2::Iterator l_start(l_data.begin());
  xtd::ConfigParser2::Iterator l_cur(l_data.begin());
  xtd::ConfigParser2::Iterator l_end(l_data.end());


  xtd::ConfigParser2::config_grammar l_grammar;



  // //  CPPUNIT_ASSERT_EQUAL(true,
  // phrase_parse(l_cur, l_end, l_grammar, l_skipper, l_result);
  // //);
  // std::cout << std::endl << std::endl;


  // auto l_error = l_grammar.getLastError();

  // std::cout
  //   << "error at line "      << l_error.line
  //   << ", column "           << l_error.col
  //   << " : expecting token " << l_error.token
  //   << ", near '"            << l_error.preview
  //   << std::endl;
  // CPPUNIT_ASSERT(l_cur == l_end);
  // CPPUNIT_ASSERT(false);

  // l_start = t_iterator(l_data.begin() + 5);
  // l_cur   = t_iterator(l_data.begin() + 5);
  // l_end   = t_iterator(l_data.end());
  // CPPUNIT_ASSERT_EQUAL(true, phrase_parse(l_cur, l_end, l_grammar, l_skipper, l_result));
  // CPPUNIT_ASSERT(l_cur != l_end);


  // string::const_iterator l_start = l_data.begin();
  // string::const_iterator l_end   = l_data.end();
  // CPPUNIT_ASSERT_EQUAL(true, phrase_parse(l_start, l_end, l_grammar, boost::spirit::ascii::space, l_result));
  // CPPUNIT_ASSERT_EQUAL(l_start, l_end);
}

XTD_TEST_MAIN();
