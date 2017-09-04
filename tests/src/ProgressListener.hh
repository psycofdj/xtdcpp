#pragma once
#include <cppunit/TestListener.h>
#include <ostream>
namespace CppUnit { class Test; }
namespace CppUnit { class TestFailure; }


namespace xtd {
namespace tests {

class ProgressListener : public CppUnit::TestListener
{
public:
  ProgressListener(std::ostream& p_out);

private:
  void startTest (CppUnit::Test* p_test);
  void endTest (CppUnit::Test* p_test);
  void addFailure (const CppUnit::TestFailure& p_failure);

private:
  std::ostream& m_out;
  bool          m_failed;
};

}}
