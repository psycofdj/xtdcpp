#ifndef SERVERS_PARAM_POD_HH_
# define SERVERS_PARAM_POD_HH_
# include "types.hh" // libcore
# include "param/Base.hh"

namespace xtd { namespace servers { namespace param { class Visitor; } } }
namespace xtd {
namespace servers {
namespace param {

/**
 * \class POD
 * \brief Templated param class
 *
 * handle type specific operation of a parameter (Base inheritence)
 */
template<typename T>
class POD : public Base
{
public:
  POD(const T& p_value);
  POD(const T& p_value, const string& p_name);

public:
  /**
   * @brief Initialize the POD from a string
   * @param p_src The string to be casted
   * @return true if the string casting and the value setting succeeds
   * @details N/A
   */
  bool fromStr(const string& p_src);

  /**
   * @brief Function to verify if given string value can be setted on parameter
   * @param p_src the new value
   * @return true if value can be setted
   * @details N/A   */
  bool verify(const string& p_src);

  /**
   * @brief Get a string from the POD representing the current value
   * @param p_dst The string containing the current value
   * @return true if the value getting and string building succeeds
   * @details N/A
   */
  bool toStr(string& p_dst)  const;

  /**
   * @brief Register this parameter to a visitor
   * @param p_visitor the right visitor
   * @details N/A
   */
  void accept(Visitor& p_visitor)  const;

private:

  bool as(const bool& p_src, string& p_dst) const;

  bool to(const string& p_src, bool& p_dst) const;

  template<typename V>
  bool as(const V& p_src, string& p_dst) const;

  template<typename V>
  bool to(const string& p_src, V& p_dst) const;
};

}}}

#endif // !SERVERS_PARAM_POD_HH_
