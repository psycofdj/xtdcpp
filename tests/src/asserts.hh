#ifndef XTD_TESTS_ASSERTS_HH_
# define XTD_TESTS_ASSERTS_HH_
# include <cppunit/extensions/HelperMacros.h>

#define CPPUNIT_ASSERT_AT( condition, file, line)                       \
  (CppUnit::Asserter::failIf(!(condition),                              \
                             CppUnit::Message("assertion failed", "Expression: " #condition), \
                             CppUnit::SourceLine(file, line)))


#define CPPUNIT_ASSERT_ASSERTION_FAIL_AT(assertion, file, line)       \
  CPPUNIT_ASSERT_THROW_AT(assertion, CppUnit::Exception, file, line)




#define CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE_AT(message, expected, actual, delta, file, line) \
  (CppUnit::assertDoubleEquals((expected),                              \
                               (actual),                                \
                               (delta),                                 \
                               CppUnit::SourceLine(file, line),         \
                               (message)))

#define CPPUNIT_ASSERT_DOUBLES_EQUAL_AT(expected,  actual, delta, file, line) \
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE_AT("", expected,  actual, delta)


#define CPPUNIT_ASSERT_EQUAL_AT(expected, actual, file, line) \
  (CppUnit::assertEquals((expected),                          \
                         (actual),                            \
                         CppUnit::SourceLine(file, line),     \
                         ""))


#define CPPUNIT_ASSERT_EQUAL_MESSAGE_AT(message, expected, actual, file, line) \
  (CppUnit::assertEquals((expected),                                    \
                         (actual),                                      \
                         CppUnit::SourceLine(file, line),               \
                         (message)))


#define CPPUNIT_ASSERT_MESSAGE_AT(message, condition, file, line)       \
  (CppUnit::Asserter::failIf(!(condition),                              \
                             CppUnit::Message( "assertion failed",      \
                                               "Expression: " #condition, \
                                               message ),               \
                             CppUnit::SourceLine(file, line)))


#define CPPUNIT_ASSERT_NO_THROW_MESSAGE_AT(message, expression, file, line) \
  do {                                                                  \
    CppUnit::Message cpputMsg_("unexpected exception caught");          \
    cpputMsg_.addDetail(message);                                       \
                                                                        \
    try {                                                               \
      expression;                                                       \
    } catch ( const std::exception &e ) {                               \
      cpputMsg_.addDetail("Caught: " +                                  \
                          CPPUNIT_EXTRACT_EXCEPTION_TYPE_(e, "std::exception or derived")); \
      cpputMsg_.addDetail(std::string("What(): ") + e.what());          \
      CppUnit::Asserter::fail( cpputMsg_, CppUnit::SourceLine(file, line)); \
    } catch ( ... ) {                                                   \
      cpputMsg_.addDetail( "Caught: unknown." );                        \
      CppUnit::Asserter::fail(cpputMsg_, CppUnit::SourceLine(file, line)); \
    }                                                                   \
  } while ( false )


#define CPPUNIT_ASSERT_NO_THROW_AT(expression, file, line)          \
  CPPUNIT_ASSERT_NO_THROW_MESSAGE_AT(CppUnit::AdditionalMessage(),  \
                                     expression, file, line )



#define CPPUNIT_ASSERT_THROW_MESSAGE_AT(message, expression, ExceptionType, file, line) \
  do {                                                                  \
    bool cpputCorrectExceptionThrown_ = false;                          \
    CppUnit::Message cpputMsg_("expected exception not thrown");        \
    cpputMsg_.addDetail(message);                                       \
    cpputMsg_.addDetail("Expected: "                                    \
                        CPPUNIT_GET_PARAMETER_STRING(ExceptionType));   \
                                                                        \
    try {                                                               \
      expression;                                                       \
    } catch ( const ExceptionType & ) {                                 \
      cpputCorrectExceptionThrown_ = true;                              \
    } catch ( const std::exception &e) {                                \
      cpputMsg_.addDetail("Actual  : " +                                \
                          CPPUNIT_EXTRACT_EXCEPTION_TYPE_(e, "std::exception or derived") ); \
      cpputMsg_.addDetail(std::string("What()  : ") + e.what());        \
    } catch ( ... ) {                                                   \
      cpputMsg_.addDetail("Actual  : unknown.");                        \
    }                                                                   \
                                                                        \
    if (cpputCorrectExceptionThrown_)                                   \
      break;                                                            \
                                                                        \
    CppUnit::Asserter::fail(cpputMsg_, CppUnit::SourceLine(file, line)); \
  } while (false)


#define CPPUNIT_ASSERT_THROW_AT(expression, ExceptionType)      \
  CPPUNIT_ASSERT_THROW_MESSAGE_AT(CppUnit::AdditionalMessage(), \
                                  expression,                   \
                                  ExceptionType, file, line)


#define CPPUNIT_FAIL_AT(message, file, line)                  \
  (CppUnit::Asserter::fail(CppUnit::Message("forced failure", \
                                            message ),        \
                           CppUnit::SourceLine(file, line)))

#endif // !XTD_TESTS_ASSERTS_HH_
