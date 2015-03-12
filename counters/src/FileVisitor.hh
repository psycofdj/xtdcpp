#ifndef COUNTERS_FILEVISITOR_HH_
# define COUNTERS_FILEVISITOR_HH_


# include <string>
# include <fstream>
# include <boost/filesystem.hpp>
# include "Visitor.hh"


namespace xtd {
namespace counters {

class FileVisitor : public Visitor
{
private:
  typedef boost::filesystem::path t_path;

public:
  FileVisitor(const t_path& p_path) :
    m_path(p_path)
  {
  }

public:
  void operator()(const string& p_name, const string& p_data) { write(p_name, p_data); }
  void operator()(const string& p_name, uint8_t p_data)        { write(p_name, p_data); }
  void operator()(const string& p_name, uint16_t p_data)       { write(p_name, p_data); }
  void operator()(const string& p_name, uint32_t p_data)       { write(p_name, p_data); }
  void operator()(const string& p_name, uint64_t p_data)       { write(p_name, p_data); }

private:
  template<typename TType>
  void write(const string& p_name, TType p_data)
  {
    t_path l_path(m_path);

    l_path /= p_name;
    std::ofstream l_file(l_path.string().c_str());
    l_file << p_data << std::endl;
    l_file.close();
  }

private:
  const t_path& m_path;
};

}}

#endif // !COUNTERS_FILEVISITOR_HH_
