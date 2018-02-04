#include "TestFixture.hh"
#include <stdlib.h>
#include "asserts.hh"
#include "types.hh"

namespace xtd {
namespace tests {

TestFixture::~TestFixture(void)
{
}

string
TestFixture::getFileContent(const string& p_path, const char* p_file, int p_line) const
{
  char*    l_buffer;
  string   l_result;
  int      l_end;
  ifstream l_file(p_path, std::ios_base::in | std::ios_base::binary);

  CPPUNIT_ASSERT_EQUAL_AT(true, l_file.is_open(), p_file, p_line);


  l_file.seekg(0, std::ios_base::end);
  l_end = l_file.tellg();
  l_file.seekg(0, std::ios_base::beg);
  l_buffer = new char[l_end + 1];
  l_file.read(l_buffer, l_end);
  l_buffer[l_end] = '\0';
  l_result.assign(l_buffer, l_end);
  l_file.close();
  delete[] l_buffer;
  return l_result;
}

void
TestFixture::exec(const std::string& p_cmd, int p_status, const char* p_file, int p_line) const
{
  int l_status = system(p_cmd.c_str());
  CPPUNIT_ASSERT_AT(WIFEXITED(l_status), p_file, p_line);
  CPPUNIT_ASSERT_EQUAL_AT(p_status,  WEXITSTATUS(l_status), p_file, p_line);
}


}}
