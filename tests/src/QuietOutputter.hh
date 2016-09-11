#pragma once
#include <cppunit/TextOutputter.h>       // libcppunit

namespace xtd {
namespace tests {

class QuietOutputter : public CppUnit::TextOutputter
{
public:
  QuietOutputter(CppUnit::TestResultCollector* p_result, std::ostream& p_stream) :
    CppUnit::TextOutputter(p_result, p_stream)
  { }

  void write ()                                                                {}
  virtual void printFailures ()                                                {}
  virtual void printHeader ()                                                  {}
  virtual void printFailure (CppUnit::TestFailure *failure, int failureNumber) {}
  virtual void printFailureListMark (int failureNumber)                        {}
  virtual void printFailureTestName (CppUnit::TestFailure *failure)            {}
  virtual void printFailureType (CppUnit::TestFailure *failure)                {}
  virtual void printFailureLocation (CppUnit::SourceLine sourceLine)           {}
  virtual void printFailureDetail (CppUnit::Exception *thrownException)        {}
  virtual void printFailureWarning ()                                          {}
  virtual void printStatistics ()                                              {}
};

}}
