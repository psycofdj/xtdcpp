#ifndef XTD_CORE_CONFIG_PARSER_HH_
# define XTD_CORE_CONFIG_PARSER_HH_
# include "types.hh" // libcore
# include "error.hh" // libcore

namespace xtd {
namespace config { namespace impl { class section;   }}
namespace config { namespace impl { class property;  }}



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
 **
 ** @section format_spec Format specification
 **
 ** - A @b configuration has:
 **   - zero--or-more @b section
 ** - A @b section has:
 **   - a name
 **   - zero-or-more @b subsection
 **   - zero-or-more @b property
 ** - A @b subsection has:
 **   - a name
 **   - zero-or-more @b property
 ** - A @b property has:
 **   - a key
 **   - a value
 **
 ** Example:
 **
 ** @code
 ** [myapp]
 ** {
 **   [logging] {
 **     level : info
 **     file  : /dev/log
 **   }
 **
 **   [webserver] {
 **     port : 8080
 **     interface : 0.0.0.0
 **   }
 ** }
 ** @endcode
 **
 ** @section parser_run Running the parser
 **
 ** Two ways of using this object :
 ** - use default constructor then call one of the following overloads :
 **   - parse<Iterator>(Iterator start, Iterator end) : parses configuration data from
 **     start to end
 **   - parse(istream&) : parses configuration from given istream
 **   - parse(const string&) : parses configuration from given file path
 **  You'll get return status through the result of the parse methods.
 **
 ** - use one of the following constructor overloads :
 **   - Parser(istream&, const t_data&)
 **   - Parser(const string&, const t_data&)
 **   Theses are equivalent to the first method but you'll have to catch
 **   @ref parse_error exception
 **
 ** @section read_keys Reading keys
 **
 ** Each property of the configuration file is accessible by its key formed by :
 ** - @< section_name @> . @< subsection_name @> . @< key_name @> : for subsection
 **   properties
 ** - @< section_name @> . @< key_name @> : for section properties
 **
 ** You can retrieve the value for a given key by calling one of the following methods :
 ** * get(const string&, string&) const : get key as string value
 ** * get(const string&, const char*&) const : get key as char* value
 ** * get(const string&, bool&) const : get key as boolean value
 ** * get<T>(const string&, T&) const : get key lexically casted to T value
 ** * get_default<T>(const string&, T&, const T&) const, get key value, use default if
 **   error
 **
 ** Alternatively, you can retreive a set of key prefixed by a certain parttern using
 ** the function search(const string&) const;
 **
 ** @section property_variables Variables in property values
 **
 ** Property value starts at first non-blank character after colon and ends at the end of
 ** the line.
 **
 ** It may contain three types of variable reference.
 **
 ** - @code $ENV{name} @endcode
 **
 **   Will be replaced by environment valiable @b name
 **
 ** - @code $PARAM{name} @endcode
 **
 **   Will be replaced by the value of @b name in the parameter map given at constructor.
 **   See @ref Parser(const t_data&).
 **
 ** - @code ${key} @endcode
 **
 **   Will be replaced by the value of @b key in the configuration.
 **
 **   @b Note that ${key} may contain references to other variables.
 **   Variable resolution will go until no more references are found or a cycle reference
 **   is detected.
 **
 ** Example : Given an environment variable @b password equals to @b secret, the two
 ** following configuration file are equivalent :
 **
 ** - @code
 ** [myapp]
 ** {
 **   [mysql] {
 **     host : localhost
 **     port : 3306
 **     user : demo
 **     pass : prefix_$ENV{password}
 **   }
 **
 **   [service1] {
 **     odbc_string : mysql://${myapp.mysql.user}:${myapp.mysql.pass}@${myapp.mysql.host}:${myapp.mysql.port}/service1
 **   }
 **
 **   [service2] {
 **     odbc_string : mysql://${myapp.mysql.user}:${myapp.mysql.pass}@${myapp.mysql.host}:${myapp.mysql.port}/service2
 **   }
 ** }
 ** @endcode
 **
 ** - @code
 ** [myapp]
 ** {
 **   [mysql] {
 **     host : localhost
 **     port : 3306
 **     user : demo
 **     pass : secret
 **   }
 **
 **   [service1] {
 **     odbc_string : mysql://demo:prefix_secret@localhost:3306/service1
 **   }
 **
 **   [service2] {
 **     odbc_string : mysql://demo:prefix_secret@localhost:3306/service2
 **   }
 ** }
 ** @endcode
 **
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

# include "config/Parser.hxx"

#endif //!XTD_CORE_CONFIG_PARSER_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
