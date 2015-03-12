#ifndef COUNTERS_JSONVISITOR_HH_
# define COUNTERS_JSONVISITOR_HH_

# include <string>
# include <boost/property_tree/ptree.hpp>
# include <json_parser.hpp> // libcommon
# include "Visitor.hh"


namespace xtd {
namespace counters {

class JsonVisitor : public Visitor
{
private:
  typedef boost::property_tree::ptree::path_type t_path;
  typedef boost::property_tree::ptree            t_elem;

public:
  JsonVisitor(const t_path& p_path, t_elem& p_elem) :
    m_path(p_path),
    m_elem(p_elem)
  {
  }

public:
  inline void operator()(const string& p_name, const string& p_data)  { write(p_name, p_data); }
  inline void operator()(const string& p_name, uint8_t p_data)         { write(p_name, p_data); }
  inline void operator()(const string& p_name, uint16_t p_data)        { write(p_name, p_data); }
  inline void operator()(const string& p_name, uint32_t p_data)        { write(p_name, p_data); }
  inline void operator()(const string& p_name, uint64_t p_data)        { write(p_name, p_data); }

private:
  template<typename TType>
  void write(const string& p_name, TType p_data);

private:
  const t_path& m_path;
  t_elem&       m_elem;
};


template<typename TType>
void
JsonVisitor::write(const string& p_name, TType p_data)
{
  using namespace boost::property_tree;

  m_elem.put<TType>(m_path / t_path(p_name, '!'), p_data, json_parser::translator<TType>());
}

}}

#endif // !COUNTERS_JSONVISITOR_HH_
