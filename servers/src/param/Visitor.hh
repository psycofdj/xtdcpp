#ifndef SERVERS_PARAM_VISITOR_HH_
# define SERVERS_PARAM_VISITOR_HH_

# include <string>
# include <json_parser.hpp>  // libcore
# include <http/Template.hh> // libnetwork
# include "param/Base.hh"


namespace xtd {
namespace servers {
namespace param {

/**
 * @brief Visitor base class
 */
class Visitor
{
public:
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<bool>&)          { }
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<int>&)           { }
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<uint8_t>&)   { }
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<uint32_t>&)  { }
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<uint64_t>&)  { }
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  virtual void operator()(const POD<string>&)   { }
};


/**
 * \brief Json specific visitor
 */
class JsonVisitor : public Visitor
{
public:

  /**
   * @brief Constructor
   * @details N/A
   *
   * @param p_tmpl the Json generator for the serialization
   */
  JsonVisitor(network::http::Json& p_tmpl);

public:

  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<bool>&         p_obj);
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<int>&          p_obj);
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<uint8_t>&  p_obj);
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<uint32_t>& p_obj);
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<uint64_t>& p_obj);
  /**
   * @brief Type specific serialization operator (will call write on the right type cf. Visitor.cc) operator
   */
  void operator()(const POD<string>&  p_obj);

  /**
   * @brief Add Extra information to the Json
   * @details Little hack to be able to add some exotic information to the Json
   *
   * @param p_key the node name
   * @param p_value the node value
   */
  void writeExtraInfo(const string& p_key, const string& p_value);

private:
  template<typename T>
  void write(const POD<T>& p_val);
private:
  network::http::Json& m_tmpl;
};

template<typename T>
void
JsonVisitor::write(const POD<T>& p_obj)
{
  T l_value;
  p_obj.get(l_value);
  m_tmpl.add(p_obj.getName(), "value", l_value);
  m_tmpl.add(p_obj.getName(), "timestamp", p_obj.date());
  m_tmpl.add(p_obj.getName(), "log", p_obj.log());
}

}}}

#endif // !SERVERS_PARAM_VISITOR_HH_
