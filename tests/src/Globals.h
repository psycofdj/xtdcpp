#pragma once
#include "types.hh" // libcore
#include "mixins/singleton.hh"

namespace xtd {
namespace tests {

class Globals : public mixins::Singleton<Globals>
{
public:
  string& sourceDir(void)    { return m_srcDir; }
  string& topSourceDir(void) { return m_topSrcDir; }
  string& topBuildDir(void)  { return m_topBuildDir; }
  string& testDir(void)      { return m_testDir; }

  const string& sourceDir(void)  const   { return m_srcDir; }
  const string& topSourceDir(void) const { return m_topSrcDir; }
  const string& topBuildDir(void) const  { return m_topBuildDir; }
  const string& testDir(void) const      { return m_testDir; }

private:
  string m_srcDir;
  string m_topSrcDir;
  string m_topBuildDir;
  string m_testDir;
};

}}
