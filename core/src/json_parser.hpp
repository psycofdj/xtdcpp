// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------

/**
 ** @details
 ** On a copier le header de boost pour palier a une limitation du
 ** writer json qui ne gere que des strings
 **   ptree.put<int>("key", 5)
 **   ptree.put<string>("key1", "salut")
 **   write_json(cout, ptree)
 **   -> { "key" : "5", "key1" : "salut" }
 **   on coudrait avoir :
 **   -> { "key" : 5, "key1" : "salut" }
 */


#ifndef BOOST_PROPERTY_TREE_JSON_PARSER_HPP_INCLUDED
# define BOOST_PROPERTY_TREE_JSON_PARSER_HPP_INCLUDED
// on empeche l'inclusion du header de boost
# define BOOST_PROPERTY_TREE_DETAIL_JSON_PARSER_WRITE_HPP_INCLUDED

# include <boost/property_tree/ptree.hpp>
# include <boost/property_tree/detail/json_parser_read.hpp>

/**
 ** warning: comparison is always true due to limited range of data
 ** type [-Wtype-limits]
 */
# pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wtype-limits"
#   include "json_parser_write.hpp" // libcore
# pragma GCC diagnostic pop
# include <boost/property_tree/detail/json_parser_error.hpp>

# include <fstream>
# include <string>
# include <locale>
# include <boost/lexical_cast.hpp>

namespace boost { namespace property_tree { namespace json_parser
{


template <typename T>
struct translator
{
  typedef std::string internal_type;
  typedef T           external_type;

  boost::optional<T>           get_value(const std::string&v) { return boost::lexical_cast<T>(v); }
  boost::optional<std::string> put_value(const T&v)           { return boost::lexical_cast<std::string>(v); }
};

template <>
struct translator<std::string>
{
  typedef std::string internal_type;
  typedef std::string external_type;

  boost::optional<std::string> get_value(const std::string& v) { return v.substr(1, v.size() - 2); }
  boost::optional<std::string> put_value(const std::string& v) { return "\"" + v + "\"";           }
};


template <>
struct translator<bool>
{
  typedef std::string internal_type;
  typedef bool        external_type;

  boost::optional<bool>        get_value(const std::string& v) { return (v == "true");        }
  boost::optional<std::string> put_value(const bool& v)        { if (v) return std::string("true"); return std::string("false"); }
};


/**
 * Read JSON from a the given stream and translate it to a property tree.
 * @note Clears existing contents of property tree.  In case of error the
 *       property tree unmodified.
 * @note Items of JSON arrays are translated into ptree keys with empty
 *       names. Members of objects are translated into named keys.
 * @note JSON data can be a string, a numeric value, or one of literals
 *       "null", "true" and "false". During parse, any of the above is
 *       copied verbatim into ptree data string.
 * @throw json_parser_error In case of error deserializing the property
 *                          tree.
 * @param stream Stream from which to read in the property tree.
 * @param[out] pt The property tree to populate.
 */
template<class Ptree>
void read_json(std::basic_istream<
               typename Ptree::key_type::value_type
               > &stream,
               Ptree &pt)
{
  read_json_internal(stream, pt, std::string());
}

/**
 * Read JSON from a the given file and translate it to a property tree.
 * @note Clears existing contents of property tree.  In case of error the
 *       property tree unmodified.
 * @note Items of JSON arrays are translated into ptree keys with empty
 *       names. Members of objects are translated into named keys.
 * @note JSON data can be a string, a numeric value, or one of literals
 *       "null", "true" and "false". During parse, any of the above is
 *       copied verbatim into ptree data string.
 * @throw json_parser_error In case of error deserializing the property
 *                          tree.
 * @param filename Name of file from which to read in the property tree.
 * @param[out] pt The property tree to populate.
 * @param loc The locale to use when reading in the file contents.
 */
template<class Ptree>
void read_json(const std::string &filename,
               Ptree &pt,
               const std::locale &loc = std::locale())
{
  std::basic_ifstream<typename Ptree::key_type::value_type>
    stream(filename.c_str());
  if (!stream)
    BOOST_PROPERTY_TREE_THROW(json_parser_error(
                                                "cannot open file", filename, 0));
  stream.imbue(loc);
  read_json_internal(stream, pt, filename);
}

/**
 * Translates the property tree to JSON and writes it the given output
 * stream.
 * @note Any property tree key containing only unnamed subkeys will be
 *       rendered as JSON arrays.
 * @pre @e pt cannot contain keys that have both subkeys and non-empty data.
 * @throw json_parser_error In case of error translating the property tree
 *                          to JSON or writing to the output stream.
 * @param stream The stream to which to write the JSON representation of the
 *               property tree.
 * @param pt The property tree to tranlsate to JSON and output.
 * @param pretty Whether to pretty-print. Defaults to true for backward
 *               compatibility.
 */
template<class Ptree>
void write_json(std::basic_ostream<
                typename Ptree::key_type::value_type
                > &stream,
                const Ptree &pt,
                bool pretty = true)
{
  write_json_internal(stream, pt, std::string(), pretty);
}

/**
 * Translates the property tree to JSON and writes it the given file.
 * @note Any property tree key containing only unnamed subkeys will be
 *       rendered as JSON arrays.
 * @pre @e pt cannot contain keys that have both subkeys and non-empty data.
 * @throw json_parser_error In case of error translating the property tree
 *                          to JSON or writing to the file.
 * @param filename The name of the file to which to write the JSON
 *                 representation of the property tree.
 * @param pt The property tree to translate to JSON and output.
 * @param loc The locale to use when writing out to the output file.
 * @param pretty Whether to pretty-print. Defaults to true and last place
 *               for backward compatibility.
 */
template<class Ptree>
void write_json(const std::string &filename,
                const Ptree &pt,
                const std::locale &loc = std::locale(),
                bool pretty = true)
{
  std::basic_ofstream<typename Ptree::key_type::value_type>
    stream(filename.c_str());
  if (!stream)
    BOOST_PROPERTY_TREE_THROW(json_parser_error(
                                                "cannot open file", filename, 0));
  stream.imbue(loc);
  write_json_internal(stream, pt, filename, pretty);
}

} } }

namespace boost { namespace property_tree
{
using json_parser::read_json;
using json_parser::write_json;
using json_parser::json_parser_error;
} }

#endif
