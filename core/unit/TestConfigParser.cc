#include <boost/filesystem.hpp>
#include "MainTestApplication.hh" // libtests
#include "CWrap.hh"               // libtests
#include <config/Parser.hh>       // libcore


using namespace std;
using namespace xtd::tests;

class TestConfParser : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestConfParser);
  CPPUNIT_TEST(Constructor);
  CPPUNIT_TEST(parse);
  CPPUNIT_TEST(get);
  CPPUNIT_TEST(search);
  CPPUNIT_TEST(setParams);
  CPPUNIT_TEST(parseFile);
  CPPUNIT_TEST_SUITE_END();
private:

  void Constructor(void);
  void parse(void);
  void get(void);
  void search(void);
  void setParams(void);
  void parseFile(void);

};


CPPUNIT_TEST_SUITE_REGISTRATION(TestConfParser);

void
TestConfParser::Constructor(void)
{
  {
    // OK empty construct
    CPPUNIT_ASSERT_NO_THROW(xtd::config::Parser l_obj);
  }

  {
    // OK construct from file
    string l_conf = Globals::get().testDir() + "/data/simple.conf";
    CPPUNIT_ASSERT_NO_THROW(xtd::config::Parser l_obj(l_conf));
  }

  {
    // KO construct from unreaddable file
    CPPUNIT_ASSERT_THROW(xtd::config::Parser l_obj("i/m/not/here"), xtd::config::parse_error);
  }

  {
    // OK construct from openned file
    string   l_conf = Globals::get().testDir() + "/data/simple.conf";
    ifstream l_file(l_conf.c_str());
    CPPUNIT_ASSERT_NO_THROW(xtd::config::Parser l_obj(l_file));
  }

  {
    // OK construct from closed file
    ifstream l_file;
    ofstream l_null("/dev/null");
    string   l_conf = Globals::get().testDir() + "/data/simple.conf";
    std::copy(std::istream_iterator<char>(l_file),
              std::istream_iterator<char>(),
              std::ostream_iterator<char>(l_null));
    CPPUNIT_ASSERT_THROW(xtd::config::Parser l_obj(l_file), xtd::config::parse_error);
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

    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // OK : plain values
    xtd::config::Parser l_parser;
    string l_data = R"data(
                           [sec] {
                           key1 : value1
                               [subsec] {
                             kkey1 : vvalue1
                                 kkey2 : vvalue2
                               kkey3 : vvalue3
                               }
                           })data";

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey3", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue3"), l_val);
  }


  {
    // OK : grammar
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

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
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

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey3", l_val));
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

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey3", l_val));
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

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("vvalue2"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey3", l_val));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("1 2 term"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey1", l_val));
    CPPUNIT_ASSERT_EQUAL(string("1 2 term"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey2", l_val));
    CPPUNIT_ASSERT_EQUAL(string("2 term"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.subsec.kkey3", l_val));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : multiply defined section key
    xtd::config::Parser l_parser;
    string              l_data = R"data(
                                        [sec]  {
                                        key1 : ${sec.subsec.kkey1}
                                        key1 : other value
                                            })data";
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved section PARAM
    xtd::config::Parser l_parser;
    string              l_data = R"data(
                                        [sec]  {
                                        key1 : $PARAM{sec.subsec.kkey1}
                                        })data";
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresolved section ENV
    xtd::config::Parser l_parser;
    string              l_data = R"data(
                                        [sec]
                                        {
                                        key1 : $ENV{sec.subsec.kkey1}
                                        })data";
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
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
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // OK : resolved ENV
    setenv("env1", "value1", 1);
    string              l_value;
    xtd::config::Parser l_parser;
    string              l_data = R"data(
                                        [sec]
                                        {
                                        key1 : $ENV{env1}
                                        })data";
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_value));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_value);

    // env are resolved at parse time
    setenv("env1", "newval", 1);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec.key1", l_value));
    CPPUNIT_ASSERT_EQUAL(string("value1"), l_value);
  }

}


void
TestConfParser::get(void)
{
  // OK : plain values
  xtd::config::Parser l_parser;
  string l_data = R"data(
                         [bool] {
                         key1 : yes
                             key2 : on
                           key3 : 1
                           key4 : true
                           key5 : no
                           key6 : off
                           key7 : 0
                           key8 : false
                           key9 : not a bool
                           }

                         [cast] {
                           int : 789
                             not_int1 : fdsfdsfkjlds
                             not_int2 : 123.123
                             not_int3 : 789456124378945612437894561243
                             double : 2.5
                             not_double : fdsfds
                             })data";

  CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));


  {
    // KO not found
    string      l_val1;
    bool        l_val2;
    const char* l_val3;
    double      l_val4;

    CPPUNIT_ASSERT_EQUAL(xtd::status::notfound, l_parser.get("dosent exist", l_val1));
    CPPUNIT_ASSERT_EQUAL(xtd::status::notfound, l_parser.get("dosent exist", l_val2));
    CPPUNIT_ASSERT_EQUAL(xtd::status::notfound, l_parser.get("dosent exist", l_val3));
    CPPUNIT_ASSERT_EQUAL(xtd::status::notfound, l_parser.get("dosent exist", l_val4));
  }


  {
    // bool values
    const char* l_val;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(0, strcmp(l_val, "yes"));
  }

  {
    // bool values
    bool l_val;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key1", l_val));
    CPPUNIT_ASSERT_EQUAL(true, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key2", l_val));
    CPPUNIT_ASSERT_EQUAL(true, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key3", l_val));
    CPPUNIT_ASSERT_EQUAL(true, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key4", l_val));
    CPPUNIT_ASSERT_EQUAL(true, l_val);

    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key5", l_val));
    CPPUNIT_ASSERT_EQUAL(false, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key6", l_val));
    CPPUNIT_ASSERT_EQUAL(false, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key7", l_val));
    CPPUNIT_ASSERT_EQUAL(false, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("bool.key8", l_val));
    CPPUNIT_ASSERT_EQUAL(false, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.get("bool.key9", l_val));
  }

  {
    // casted int
    int l_val;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("cast.int", l_val));
    CPPUNIT_ASSERT_EQUAL(789, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.get("cast.not_int1", l_val));
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.get("cast.not_int2", l_val));
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.get("cast.not_int3", l_val));
  }

  {
    // casted double
    double l_val;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("cast.double", l_val));
    CPPUNIT_ASSERT_EQUAL(2.5, l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.get("cast.not_double", l_val));
  }

  {
    // default values
    string l_val;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get_default("bool.key1", l_val, string("default")));
    CPPUNIT_ASSERT_EQUAL(string("yes"), l_val);
    CPPUNIT_ASSERT_EQUAL(xtd::status::notfound, l_parser.get_default("bool.notfound", l_val, string("other")));
    CPPUNIT_ASSERT_EQUAL(string("other"), l_val);
  }
}



void
TestConfParser::search(void)
{
  // OK : plain values
  xtd::config::Parser l_parser;
  string              l_data = R"data(
                                      [sec1] {
                                      key1 : key1
                                          key2 : key2
                                        key3 : key3
                                        [subsec1] {
                                      key1 : key1
                                          key2 : key2
                                        key3 : key3
                                        }
                                      [subsec2] {
                                      key1 : key1
                                          }
                                      })data";


  CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));

  {
    // no result
    auto l_range = l_parser.search("sec1.subsec1.toto");
    CPPUNIT_ASSERT(l_range.first == l_range.second);
  }


  {
    // only subsec1
    auto l_range = l_parser.search("sec1.subsec1");
    CPPUNIT_ASSERT(l_range.second != l_range.first);

    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key1"), l_range.first->first);
    l_range.first++;
    CPPUNIT_ASSERT(l_range.second != l_range.first);

    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key2"), l_range.first->first);
    l_range.first++;
    CPPUNIT_ASSERT(l_range.second != l_range.first);

    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key3"), l_range.first->first);
    l_range.first++;
    CPPUNIT_ASSERT(l_range.second == l_range.first);
  }


  {
    // both subsections
    auto l_range = l_parser.search("sec1.sub");

    CPPUNIT_ASSERT(l_range.second != l_range.first);
    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key1"), l_range.first->first);
    l_range.first++;

    CPPUNIT_ASSERT(l_range.second != l_range.first);
    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key2"), l_range.first->first);
    l_range.first++;

    CPPUNIT_ASSERT(l_range.second != l_range.first);
    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec1.key3"), l_range.first->first);
    l_range.first++;

    CPPUNIT_ASSERT(l_range.second != l_range.first);
    CPPUNIT_ASSERT_EQUAL(string("sec1.subsec2.key1"), l_range.first->first);
    l_range.first++;

    CPPUNIT_ASSERT(l_range.second == l_range.first);
  }
}


void
TestConfParser::setParams(void)
{
  // OK : plain values
  string l_value;
  string l_data = R"data(
    [sec1] {
    key1 : key1
        [subsec1] {
      key1 : $PARAM{param1}
      }
    })data";

  {
    // KO : unresovled param
    xtd::config::Parser l_parser;
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse(l_data.begin(), l_data.end()));
  }

  {
    // KO : unresovled param
    xtd::config::Parser l_parser(xtd::config::Parser::t_data{{ "param1", "test1" }});
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(l_data.begin(), l_data.end()));
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.get("sec1.subsec1.key1", l_value));
    CPPUNIT_ASSERT_EQUAL(string("test1"), l_value);
  }
}


void
TestConfParser::parseFile(void)
{
  {
    // OK : config from file
    xtd::config::Parser l_parser;
    CPPUNIT_ASSERT_EQUAL(xtd::status::ok, l_parser.parse(Globals::get().testDir() + "/data/simple.conf"));
  }

  {
    // OK : file not found
    xtd::config::Parser l_parser;
    CPPUNIT_ASSERT_EQUAL(xtd::status::error, l_parser.parse("/i/dont/exist"));
  }
}


XTD_TEST_MAIN();
