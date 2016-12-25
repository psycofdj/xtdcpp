namespace xtd {
namespace config {

/**
 ** @page core_config Configuration parsing
 **
 ** [TOC]
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Introduction {#config_intro}
 **
 ** @todo doc
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Example {#config_example}
 **
 ** @todo doc
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Reference {#config_reference}
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Format specification {#config_format}
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
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Running the parser {#config_run}
 **
 ** Two ways of using the Parser class :
 ** - use default constructor then call one of the following overloads :
 **   - @ref Parser::parse<Iterator>(Iterator,Iterator) "parse<Iterator>(Iterator start, Iterator end)" :
 **     parses configuration data from start to end
 **   - @ref Parser::parse(istream&) "parse(istream&)" : parses configuration from given
 **     istream
 **   - @ref Parser::parse(const string&) "parse(const string&)" : parses configuration
 **     from given file path
 **
 **  You'll get return status through the result of the parse methods.
 **
 ** - use one of the following constructor overloads :
 **   - @ref Parser::Parser(istream&, const t_data&) "Parser(istream&, const t_data&)"
 **   - @ref Parser::Parser(const string&, const t_data&) "Parser(const string&, const t_data&)"
 **
 **   Theses are equivalent to the first method but you'll have to catch
 **   @ref parse_error exception.
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Reading keys {#config_keys}
 **
 ** Each property of the configuration file is accessible by its key formed by :
 ** - @< section_name @> . @< subsection_name @> . @< key_name @> : for subsection
 **   properties
 ** - @< section_name @> . @< key_name @> : for section properties
 **
 ** You can retrieve the value for a given key by calling one of the following methods :
 ** - @ref Parser::get(const string&, string&) const : get key as string value
 ** - @ref Parser::get(const string&, const char*&) const : get key as char* value
 ** - @ref Parser::get(const string&, bool&) const : get key as boolean value
 ** - @ref Parser::get<T>(const string&, T&) const : get key lexically casted to T value
 ** - @ref Parser::get_default<T>(const string&, T&, const T&) const, get key value,
 **   use default if error
 **
 ** Alternatively, you can retreive a set of key prefixed by a certain parttern using
 ** the function search(const string&) const;
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Variables in property values {#config_vars}
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
 **   See @ref Parser::Parser(const t_data&) "Parser(const t_data&)".
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
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
