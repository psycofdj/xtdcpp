#pragma once
#include <cppunit/TextOutputter.h>       // libcppunit
#include <cppunit/SourceLine.h>

namespace xtd {
namespace tests {

class QuietOutputter : public CppUnit::TextOutputter
{
public:
  QuietOutputter(CppUnit::TestResultCollector* p_result, std::ostream& p_stream) :
    CppUnit::TextOutputter(p_result, p_stream)
  { }

  void write (void)                                         {}
  virtual void printFailures (void)                         {}
  virtual void printHeader (void)                           {}
  virtual void printFailure (CppUnit::TestFailure*, int )   {}
  virtual void printFailureListMark (int)                   {}
  virtual void printFailureTestName (CppUnit::TestFailure*) {}
  virtual void printFailureType (CppUnit::TestFailure*)     {}
  virtual void printFailureLocation (CppUnit::SourceLine )  {}
  virtual void printFailureDetail (CppUnit::Exception*)     {}
  virtual void printFailureWarning (void)                   {}
  virtual void printStatistics (void)                       {}
};

}}
