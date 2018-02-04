// IWYU pragma: private, include "http/Template.hh"
#ifndef NETWORK_HTTP_TEMPLATE_HXX_
# define NETWORK_HTTP_TEMPLATE_HXX_
# include <boost/algorithm/string/replace.hpp>
# include <boost/property_tree/exceptions.hpp>
# include <boost/property_tree/ptree.hpp>
# include <boost/property_tree/ptree_fwd.hpp>
# include <json_parser.hpp>
# include "types.hh"        // libcore
# include "log/helpers.hh"  // libcore
# include "log/logtypes.hh" // libcore
# include "format.hh"       // libcore
# include "http/cpptempl.hh"

namespace xtd { namespace network { namespace http { class HtmlTemplate; } } }
namespace xtd { namespace network { namespace http { class Template; } } }
namespace xtd { namespace network { namespace http { class XmlTemplate; } } }

namespace xtd {
namespace network {
namespace http {


const string&
Generator::getContentType(void) const
{
return m_contentType;
}

const string&
Generator::getError(void) const
{
return m_error;
}

void
Generator::addError(const string& p_error)
{
if (0 != m_error.size())
  m_error += ", ";
m_error += p_error;

log::err("network.http.template", "%s", p_error, HERE);
}


template<typename T>
void
Json::add(const string& p_path,
            const string& p_key,
            const T&      p_val)
{
using namespace boost::property_tree;

ptree::path_type l_path(boost::replace_all_copy(p_path, ".", "!"), '!');

l_path /= ptree::path_type(p_key, '!');

try {
m_root.put<T>(l_path, p_val, json_parser::translator<T>());
} catch (const ptree_bad_data& l_error) {
addError(format::vargs("json add value error : %s", l_error.what()));
}
}


template<class T>
Template::t_val
Template::val(T p_val)
{
return cpptempl::make_data(p_val);
}


template<typename T>
Template&
Template::add(const string& p_key,
                T                  p_val)
{
m_data[p_key] = val(p_val);
return *this;
}


Template&
Template::setTemplate(const string& p_text)
{
m_template = p_text;
return *this;
}

Template&
Template::setData(cpptempl::data_map& p_data)
{
m_data = p_data;
return *this;
}

Template&
Template::setContent(const string& p_text)
{
m_content = p_text;
return *this;
}


HtmlTemplate&
HtmlTemplate::setCharset(const string& p_charset)
{
m_charset = p_charset;
return *this;
}

HtmlTemplate&
HtmlTemplate::setTitle(const string& p_title)
{
m_title = p_title;
return *this;
}

HtmlTemplate&
HtmlTemplate::addCss(const string& p_cssUrl)
{
m_cssList.push_back(p_cssUrl);
return *this;
}

HtmlTemplate&
HtmlTemplate::addJs(const string& p_jsUrl)
{
m_jsList.push_back(p_jsUrl);
return *this;
}

HtmlTemplate&
HtmlTemplate::setMetaKeywords(const string&  p_keywords)
{
m_metaKeywords = p_keywords;
return *this;
}

HtmlTemplate&
HtmlTemplate::setMetaDescription(const string& p_description)
{
m_metaDescription = p_description;
return *this;
}

HtmlTemplate&
HtmlTemplate::setContentType(const string& p_mimeType,
                               const string& p_charset)
{
m_mimeType = p_mimeType;
m_charset  = p_charset;
return *this;
}

XmlTemplate&
XmlTemplate::setXslt(const string& p_xsltUrl)
{
m_xsltUrl = p_xsltUrl;
return *this;
}


XmlTemplate&
XmlTemplate::setCharset(const string& p_charset)
{
m_charset = p_charset;
return *this;
}

}}} //end namespaces


#endif // !NETWORK_HTTP_TEMPLATE_HXX_
