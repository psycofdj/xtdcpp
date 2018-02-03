#ifndef XTD_CORE_CONFIG_PARSER_HH_
# define XTD_CORE_CONFIG_PARSER_HH_
# include "types.hh"
# include "config/types.hh"
# include "error.hh"

namespace xtd {

/**
 ** @brief Parse and read configuration files
 */
namespace config {

/**
 ** @class xtd::config::parse_error
 ** @brief thrown by Parser non default constructors
 */

/**
 ** @brief Defines class parse_error
 */
xtd_error_class(parse_error, error);


/**
 ** @brief Configuration parser
 ** @details
 ** See @ref core_config for usage details
 */
class Parser
{
private:
  typedef map<string,impl::property>                           t_refs;

public:
  /// key/value parameter map type
  typedef map<string,string>                                   t_data;
  /// iterator start/end pair returned by search operations
  typedef pair<t_data::const_iterator, t_data::const_iterator> t_result;

public:

  /**
   ** @brief Construct empty parser
   ** @param p_params Parameter map for $PARAM{} values
   */
  Parser(const t_data& p_params = t_data());

  /**
   ** @brief Construct and parse given file name
   ** @param p_file filename to parse
   ** @param p_params Parameter map for $PARAM{} values
   ** @throw parse_error when parsing is not successful
   */
  Parser(const string& p_file,
         const t_data& p_params = t_data());

  /**
   ** @brief Construct and parse given stream
   ** @param p_stream valid opened stream
   ** @param p_params Parameter map for $PARAM{} values
   ** @throw parse_error when parsing is not successful
   */
  Parser(istream&      p_stream,
         const t_data& p_params = t_data());

public:
  /**
   ** @brief Read key as string value
   ** @param p_name key name
   ** @param p_dval string value
   ** @return status::ok if key is found
   */
  status get(const string& p_name, string& p_dval) const;

  /**
   ** @brief Read key as char* value
   ** @param  p_name key name
   ** @param p_dval char* value
   ** @return status::ok if key is found
   ** @details
   ** Returned pointer points to parser's internal structures. Pointer will be invalid
   ** if parser is destroyed or used to parse another configuration.
   */
  status get(const string& p_name, const char*& p_dval) const;

  /**
   ** @brief Read key as bool value
   ** @param p_name key name
   ** @param p_dval bool value
   ** @return status::ok if key found and compatible with bool specification
   ** @details
   ** - are valid true boolean values : true, yes, 1, on
   ** - are valid false boolean values : false, no, 0, off
   ** Any other values will return an error.
   **
   ** Detailed return codes :
   ** - status::ok : key found and castable to bool
   ** - status::error : key found but not castable to boolean
   ** - status::notfound : key not found in configuration
   */
  status get(const string& p_name, bool& p_dval) const;

  /**
   ** @brief Read key as generic T value
   ** @tparam T output value type
   ** @param p_name key name
   ** @param p_val T value
   ** @return status::ok if key found and lexically castable to T
   ** @details
   ** See @ref get(const string&,bool&)const for return code specification
   */
  template<typename T>
  status get(const string& p_name, T& p_val) const;

  /**
   ** @brief Read key as generic T value with fallback default
   ** @tparam T output value type
   ** @param p_key key name
   ** @param p_dest T value
   ** @param p_default T default value to use when key not found or not compatible with T
   ** @return status::ok if key found and lexically castable to T
   ** @details
   ** Return codes
   ** - status::ok       : key found and compatible with T, p_dest receive key's value
   ** - status::notfound : key not found or not castable to T, p_dest initialized with
   **   p_default
   */
  template<typename T>
  status get_default(const string& p_key, T& p_dest, const T& p_default) const;

public:
  /**
   ** @brief Parse configuration given by p_begin and p_end iterators
   ** @tparam Iterator Generic forward iterator type
   ** @param p_begin begin iterator
   ** @param p_end end iterator
   ** @return status::ok if parsing succeeded and variables were successfully resolved
   */
  template<typename Iterator>
  status parse(Iterator p_begin, Iterator p_end);

  /**
   ** @brief Parse configuration from given input stream
   ** @param p_stream input stream
   ** @return status::ok if parsing succeeded and variables were successfully resolved
   */
  status parse(istream& p_stream);

  /**
   ** @brief Parse configuration from given file path
   ** @param p_file readable file path
   ** @return status::ok if parsing succeeded and variables were successfully resolved
   */
  status parse(const string& p_file);


  /**
   ** @brief Returns values of keys prefixed by p_pattern
   ** @param p_pattern Key prefix, empty means all keys
   ** @return Pair of (begin,end) iterators
   ** @details
   **
   ** Returned value is a pair of iterator pointing to the first and last element matching
   ** given pattern. Element is itself a pair, first points to key and second points to
   ** its string value
   **
   ** Returned iterators points directly on object's internal structures, they will be
   ** invalid when object is destroyed or used to parse another configuration.
   **
   ** Basic usage :
   **
   ** @code
   ** auto l_result = l_parser.search("section.subsection.obj_");
   **
   ** while (l_result.first != l_result.second)
   ** {
   **   std::cout << "key : "   << l_result.first->first << std::endl;
   **   std::cout << "value : " << l_result.first->second << std::endl;
   **   l_result.first++;
   ** }
   ** @endcode
   **
   */
  t_result search(const string& p_pattern) const;

private:
  status translate(vector<impl::section>& p_sections,
                   t_data&                p_data) const;

  status resolveExt(impl::property& p_sections) const;
  status resolveRefs(t_refs& p_refs, t_data& p_data) const;
  std::string extractKey(const string& p_item) const;

private:
  t_data m_data;
  t_data m_params;
};

}}

# include "config/Parser.hxx"// IWYU pragma: export

#endif //!XTD_CORE_CONFIG_PARSER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
