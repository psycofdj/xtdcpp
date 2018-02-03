#ifndef XTD_TESTS_TESTFIXTURE_HH_
# define XTD_TESTS_TESTFIXTURE_HH_
# include "asserts.hh" // IWYU pragma: export
# include "cppunit/TestFixture.h"
# include "types.hh"

# define XTD_TESTS_LOC __FILE__,__LINE__

namespace xtd {
namespace tests {

class TestFixture : public CppUnit::TestFixture
{
protected:
  virtual ~TestFixture(void);

protected:
  string getFileContent(const string& p_path, const char* p_file, int p_line) const;
  void   exec(const std::string& p_cmd, int p_status, const char* p_file, int p_line) const;
};

}}

#endif // !XTD_TESTS_TESTFIXTURE_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
