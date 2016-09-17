#include <boost/filesystem.hpp>
#include "MainTestApplication.hh" // libtests
#include "CWrap.hh"               // libtests
#include "ConfParser.hh"          // libcommon
#include "error.hh"               // libcommon
#include <config/Parser.hh>       // libcommon


using namespace std;
using namespace xtd::tests;
using xtd::ConfParser;

class TestConfParser : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestConfParser);
  CPPUNIT_TEST(Constructor);
  CPPUNIT_TEST(parse);
  CPPUNIT_TEST_SUITE_END();
private:

  void Constructor(void);
  void parse(void);
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
}


void
TestConfParser::parse(void)
{
  string l_val;

  {
    // KO : parse error
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : value1
        [subsec]
          kkey1 : vvalue1
          kkey2 : vvalue2
          kkey3 : vvalue3
        }
      })data";

    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // OK : plain values
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : value1
        [subsec]
        {
          kkey1 : vvalue1
          kkey2 : vvalue2
          kkey3 : vvalue3
        }
      })data";

    CPPUNIT_ASSERT(xtd::status::ok == l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue3"), l_val);
  }

  {
    // OK : simple reference
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : value1
        [subsec]
        {
          kkey1 : vvalue1
          kkey2 : vvalue2
          kkey3 : ${sec.subsec.kkey2}
        }
      })data";

    CPPUNIT_ASSERT(xtd::status::ok == l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
  }

  {
    // OK : multiple references in same key
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : value1
        [subsec]
        {
          kkey1 : vvalue1
          kkey2 : vvalue2
          kkey3 : ${sec.subsec.kkey2} ${sec.subsec.kkey1}
        }
      })data";

    CPPUNIT_ASSERT(xtd::status::ok == l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2 vvalue1"), l_val);
  }

  {
    // OK : cascading references
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : value1
        [subsec]
        {
          kkey1 : ${sec.key1}
          kkey2 : vvalue2
          kkey3 : ${sec.subsec.kkey2} ${sec.subsec.kkey1}
        }
      })data";

    CPPUNIT_ASSERT(xtd::status::ok == l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2 value1"), l_val);
  }

  {
    // OK : reverse cascading references
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : ${sec.subsec.kkey1}
        [subsec]
        {
          kkey1 : 1 ${sec.subsec.kkey2}
          kkey2 : 2 ${sec.subsec.kkey3}
          kkey3 : term
        }
      })data";
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("1 2 term"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("1 2 term"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("2 term"), l_val);
    CPPUNIT_ASSERT(xtd::status::ok == l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("term"), l_val);
  }

  {
    // KO : undefinied reference
    xtd::config::Parser l_parser;
    string l_data = R"data(
      [sec]
      {
        key1 : ${sec.subsec.kkey1}
        [subsec]
        {
          kkey1 : 1 ${sec.subsec.kkey2} ${sec.subsec.kkey3}
          kkey2 : 2 ${sec.subsec.kkey3}
          kkey3 : ${i.dont.exist}
        }
      })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }


  {
    // KO : cycling references
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec]
    {
      key1 : ${sec.subsec.kkey1}
      [subsec]
      {
        kkey1 : 1 ${sec.subsec.kkey2} ${sec.subsec.kkey3}
        kkey2 : 2 ${sec.subsec.kkey3}
        kkey3 : ${sec.key1}
      }
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : multiply defined section key
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec]  {
      key1 : ${sec.subsec.kkey1}
      key1 : other value
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }


  {
    // KO : multiply defined dubsection key
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec]  {
      [subsec] {
        key1 : ${sec.subsec.kkey1}
        key1 : other value
      }
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved section PARAM
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec]  {
      key1 : $PARAM{sec.subsec.kkey1}
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved subsection PARAM
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec] {
      [subsec] {
        key1 : $PARAM{value}
      }
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved section ENV
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec]
    {
      key1 : $ENV{sec.subsec.kkey1}
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved subsection ENV
    xtd::config::Parser l_parser;
    string              l_data = R"data(
    [sec] {
      [subsec] {
        key1 : $ENV{sec.subsec.kkey2}
      }
    })data";
    CPPUNIT_ASSERT(xtd::status::error == l_parser.parse(l_data.begin(), l_data.end()));
  }


}

XTD_TEST_MAIN();
