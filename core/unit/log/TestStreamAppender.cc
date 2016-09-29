#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include "log/StreamAppender.hh"  //libtests
#include <boost/filesystem.hpp>


using namespace xtd;
using namespace xtd::log;

class TestStreamAppender : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestStreamAppender);
  CPPUNIT_TEST(constructor);
  CPPUNIT_TEST(print);
  CPPUNIT_TEST(create);

  CPPUNIT_TEST_SUITE_END();

private:
  void constructor(void);
  void create(void);
  void print(void);

private:
  FormattedRecord rec(const std::string& p_content) const;
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestStreamAppender);

FormattedRecord
TestStreamAppender::rec(const std::string& p_content) const
{
  Record          l_rec;
  FormattedRecord l_frec(l_rec);

  l_frec.m_fulllog = p_content;
  return l_frec;
}

void
TestStreamAppender::constructor(void)
{
  CPPUNIT_ASSERT_NO_THROW(StreamAppender(std::cout));

  sptr<Stream> l_stream(new Stream(std::cout));
  CPPUNIT_ASSERT_NO_THROW(StreamAppender(l_stream));
}


void
TestStreamAppender::print(void)
{
  stringstream    l_str;
  StreamAppender  l_appender(l_str);

  l_appender.print(rec("test str"));

  CPPUNIT_ASSERT_EQUAL(string("test str\n"), l_str.str());
}


void
TestStreamAppender::create(void)
{
  namespace bfs = boost::filesystem;

  auto   l_upath = bfs::temp_directory_path() / bfs::unique_path();
  string l_name  = l_upath.native();

  {
    // OK, target file, default
    auto l_app = StreamAppender::create("A1", {
        { "log.appender.A1.fd",   l_name }
      });
    std::static_pointer_cast<StreamAppender>(l_app)->print(rec("1"));
    CPPUNIT_ASSERT_EQUAL(string("1\n"), getFileContent(l_name, XTD_TESTS_LOC));
  }

  {
    // OK, target file, append mode
    auto l_app = StreamAppender::create("A1", {
        { "log.appender.A1.fd",   l_name },
        { "log.appender.A1.mode", "append" }
      });
    std::static_pointer_cast<StreamAppender>(l_app)->print(rec("2"));
    CPPUNIT_ASSERT_EQUAL(string("1\n2\n"), getFileContent(l_name, XTD_TESTS_LOC));
  }

  {
    // OK, target file, truncate mode
    auto l_app = StreamAppender::create("A1", {
        { "log.appender.A1.fd",   l_name },
        { "log.appender.A1.mode", "truncate" }
      });
    std::static_pointer_cast<StreamAppender>(l_app)->print(rec("3"));
    CPPUNIT_ASSERT_EQUAL(string("3\n"), getFileContent(l_name, XTD_TESTS_LOC));
  }



  {
    // OK, target std::cout
    auto l_app = StreamAppender::create("A1", {
        { "log.appender.A1.fd",   "cout" }
      });
  }

  {
    // OK, target std::err
    auto l_app = StreamAppender::create("A1", {
        { "log.appender.A1.fd",   "cerr" }
      });
  }

  {
    // KO, appender not found
    CPPUNIT_ASSERT_THROW(StreamAppender::create("A1", {}), log_error);
  }

  {
    // KO, target file, invalid mode
    CPPUNIT_ASSERT_THROW(StreamAppender::create("A1", {
          { "log.appender.A1.fd",   l_name    },
          { "log.appender.A1.mode", "unknown" },
            }), log_error);
  }

  {
    // KO, target writable file
    CPPUNIT_ASSERT_THROW(StreamAppender::create("A1", {
          { "log.appender.A1.fd",   "/" }
        }), log_error);
  }

  boost::filesystem::remove_all(l_upath);
}



XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
