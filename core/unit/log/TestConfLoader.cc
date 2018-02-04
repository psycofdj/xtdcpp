#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <MainTestApplication.hh> //libtests
#include <TestFixture.hh>         //libtests
#include <CWrap.hh>               //libtests
#include "log/ConfLoader.hh"      //libcore
#include "log/MemoryAppender.hh"
#include "log/ColoredFormatter.hh"
#include <boost/filesystem.hpp>
#include <boost/any.hpp>



using namespace xtd;
using namespace xtd::log;
using namespace xtd::tests;

class TestConfLoader : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestConfLoader);
  CPPUNIT_TEST(configure);
  CPPUNIT_TEST_SUITE_END();


private:
  void configure(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(TestConfLoader);

void
TestConfLoader::configure(void)
{
  map<string,string> l_props;

  {
    // OK, empty def
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    RootLogger  l_r1;

    CPPUNIT_ASSERT_NO_THROW(l_loader.configure(l_props, l_r1));
    l_loader.configure(l_props, l_r1);
    CPPUNIT_ASSERT_EQUAL(0ul,  l_loader.m_appenders.size());
    CPPUNIT_ASSERT_EQUAL(0ul,  l_loader.m_formatters.size());
  }

  {
    // OK, single def
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",       "debug, a1" },
      { "log.appender.a1.class", "MemoryAppender" }
    };
    CPPUNIT_ASSERT_NO_THROW(l_loader.configure(l_props, l_r1));
    CPPUNIT_ASSERT_EQUAL(1ul,  l_loader.m_appenders.size());
    CPPUNIT_ASSERT_EQUAL(0ul,  l_loader.m_formatters.size());
  }

  {
    // OK, multiple def
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",       "debug, a1" },
      { "log.logger.l1",         "err, a1" },
      { "log.appender.a1.class", "MemoryAppender" }
    };
    CPPUNIT_ASSERT_NO_THROW(l_loader.configure(l_props, l_r1));
    CPPUNIT_ASSERT_EQUAL(1ul,  l_loader.m_appenders.size());
    CPPUNIT_ASSERT_EQUAL(level::debug,  l_r1.getLevel());
    CPPUNIT_ASSERT_EQUAL(level::err,    l_r1.getLevelTo("l1"));
  }


  {
    // OK, single def with formatter
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    l_loader.registerFormatter<ColoredFormatter>("ColoredFormatter");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",           "debug, a1" },
      { "log.appender.a1.class",     "MemoryAppender" },
      { "log.appender.a1.formatter", "f1" },
      { "log.formatter.f1.class",    "ColoredFormatter" },
    };
    CPPUNIT_ASSERT_NO_THROW(l_loader.configure(l_props, l_r1));
    CPPUNIT_ASSERT_EQUAL(1ul,  l_loader.m_appenders.size());
    CPPUNIT_ASSERT_EQUAL(1ul,  l_loader.m_formatters.size());
  }

  {
    // OK, multiple def with single formatter
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    l_loader.registerFormatter<Formatter>("Formatter");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",           "debug, a1" },
      { "log.logger.l1",             "debug, a2" },
      { "log.appender.a1.class",     "MemoryAppender" },
      { "log.appender.a1.formatter", "f1" },
      { "log.appender.a2.class",     "MemoryAppender" },
      { "log.appender.a2.formatter", "f1" },
      { "log.formatter.f1.class",    "Formatter" },
    };
    CPPUNIT_ASSERT_NO_THROW(l_loader.configure(l_props, l_r1));
    CPPUNIT_ASSERT_EQUAL(2ul,  l_loader.m_appenders.size());
    CPPUNIT_ASSERT_EQUAL(1ul,  l_loader.m_formatters.size());
  }

  {
    // KO, invalid logger def
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root", "" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }

  {
    // KO, invalid logger level
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root", "unknown, a1" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }


  {
    // KO, unknown appender
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root", "err, a1" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }

  {
    // KO, unknown appender class
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",       "err, a1" },
      { "log.appender.a1.class", "unknown-class" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }


  {
    // KO, invalid sub-object
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();

    l_loader.registerAppender<MemoryAppender>("MemoryAppender");

    RootLogger  l_r1;
    l_props = {
      { "log.logger.root", "err, a1" },
      { "log.logger.appender.a1.class", "MemoryAppender" },
      { "log.logger.appender.a1.size",  "not-a-int" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }

  {
    // KO, missing formatter class
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",           "debug, a1" },
      { "log.appender.a1.class",     "MemoryAppender" },
      { "log.appender.a1.formatter", "f1" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }


  {
    // KO, unknown formatter class
    ConfLoader::ms_instance.reset();
    ConfLoader& l_loader = ConfLoader::get();
    l_loader.registerAppender<MemoryAppender>("MemoryAppender");
    RootLogger  l_r1;
    l_props = {
      { "log.logger.root",           "debug, a1" },
      { "log.appender.a1.class",     "MemoryAppender" },
      { "log.appender.a1.formatter", "f1" },
      { "log.formatter.f1.class",    "unknown-class" }
    };
    CPPUNIT_ASSERT_THROW(l_loader.configure(l_props, l_r1), log_error);
  }
}


XTD_TEST_MAIN();

// Local Variables:
// ispell-local-dictionary: "american"
// End:
