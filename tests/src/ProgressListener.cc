#include "ProgressListener.hh"
#include <cppunit/Test.h>
#include <string>
namespace CppUnit { class TestFailure; }


namespace CppUnit { class TestFailure; }
namespace xtd {
namespace tests {

ProgressListener::ProgressListener(std::ostream& p_out) :
  m_out(p_out),
  m_failed(false)
{ }

void
ProgressListener::startTest (CppUnit::Test* p_test)
{
  m_failed = false;
  m_out << p_test->getName() << " : start" << std::endl;
}
void
ProgressListener::endTest (CppUnit::Test* p_test)
{
  m_out << p_test->getName() << " : end " << (m_failed ? "Ko" : "Ok") << std::endl;
}

void
ProgressListener::addFailure (const CppUnit::TestFailure&)
{
  m_failed = true;
}

}}
