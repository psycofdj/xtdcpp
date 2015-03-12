#include "Template.hh"

#include <fstream>
#include <iterator>
#include <boost/assign/std/vector.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/format.hpp>
#include <json_parser.hpp> // libcommon
#include <text.hh>         // libcommon

using namespace boost::assign;
using namespace boost;


namespace xtd {
namespace network {
namespace http {


const string Generator::mcs_defaultContentType = "text/plain";

const string HtmlTemplate::mcs_htmlTemplate =
  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
  "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
  "  <head>\n"
  "    <meta http-equiv='Content-Type' content='{$__mimeType}; charset={$__charset}'/>\n"
  "    <meta name='description'        content='{$__metaDescription}'/>\n"
  "    <meta name='keywords'           content='{$__metaKeywords}'/>\n"
  "    <link rel='shortcut icon'       type='image/x-icon' href='{$__favico}'/>"
  "    <title>{$__title}</title>\n"
  "{% for jsUrl in __jsList %}"
  "    <script type='text/javascript' src='{$jsUrl}'></script>\n"
  "{% endfor %}"
  "{% for cssUrl in __cssList %}"
  "    <link rel='stylesheet' type='text/css' href='{$cssUrl}'/>\n"
  "{% endfor %}"
  "  </head>\n"
  "  <body id='main_body'>\n"
  "   {$__content}\n"
  "  </body>\n"
  "</html>\n";

const string XmlTemplate::mcs_xmlTemplate =
  "<?xml version='1.0' encoding='{$__charset}'?>\n"
  "{% if __xsltUrl %}"
  "<?xml-stylesheet href='{$__xsltUrl}' type='text/xsl'?>\n"
  "{% endif %}"
  "{$__xmlData}\n";


/* ------------------------------------------------------------------- */

Generator::Generator(const string& p_contentType) :
  m_contentType(p_contentType),
  m_error()
{
}

Generator::~Generator(void)
{
}

/* ------------------------------------------------------------------- */

Json::Json(void) :
  Generator("application/json"),
  m_root()
{
}

Json::~Json(void)
{
}

status
Json::resolve(string& p_result)
{
  std::stringstream l_out;

  try {
    boost::property_tree::json_parser::write_json(l_out, m_root);
    p_result = l_out.str();
  } catch (const boost::property_tree::ptree_error& l_error)
  {
    addError(str(format("json resolve error : %s") % l_error.what()));
    return status::error;
  }
  return status::ok;
}


/* ------------------------------------------------------------------- */

Template::Template(const string& p_contentType) :
  Generator(p_contentType),
  m_template(),
  m_data(),
  m_content()
{
}


Template::~Template(void)
{
}


status
Template::resolve(string& p_result)
{
  if ((0 == m_template.size()) && (m_content.size() == 0))
  {
    addError("you must set either template content or raw content");
    return status::error;
  }

  if (0 != m_template.size())
  {
    try {
      p_result = cpptempl::parse(m_template, m_data);
    }
    catch (cpptempl::TemplateException& l_error)
    {
      addError(str(format("template parsing error : %s") % l_error.what()));
      p_result.clear();
      return status::error;
    }
  }
  else
  {
    p_result = m_content;
  }

  return status::ok;
}


status
Template::loadTemplate(const string& p_templatePath)
{
  string        l_templateText;
  std::ifstream l_templateFile(p_templatePath.c_str());

  if (!l_templateFile)
  {
    logger::err("network.http.template", "unable to open template file '%s'", p_templatePath, HERE);
    return status::error;
  }

  copy(std::istreambuf_iterator<char>(l_templateFile.rdbuf()),
       std::istreambuf_iterator<char>(),
       std::insert_iterator<string>(l_templateText, l_templateText.begin()));

  setTemplate(l_templateText);
  return status::ok;
}


Template::t_val
Template::val(const string& p_val, bool p_encode)
{
  string l_value = p_val;
  if (true == p_encode)
    xtd::text::xml::encode(l_value);
  return cpptempl::make_data(l_value);
}


Template&
Template::add(const string& p_key,
              const string& p_val,
              bool          p_encode)
{
  m_data[p_key] = val(p_val, p_encode);
  return *this;
}

/* ------------------------------------------------------------------- */


HtmlTemplate::HtmlTemplate(bool p_defaultAssests) :
  Template("text/html"),
  m_title("unspecified page title"),
  m_cssList(),
  m_jsList(),
  m_metaDescription(),
  m_metaKeywords(),
  m_mimeType("text/html"),
  m_charset("UTF-8"),
  m_favico()
{
  if (true == p_defaultAssests)
  {
    setFavico("/favico.ico");
    addJs("/js/jquery.js");
    addJs("/js/jquery.cookie.js");
    addJs("/js/jquery.mousewheel.js");
    addJs("/js/jquery.dataTables.js");
    addJs("/js/jquery.sprintf.js");
    addJs("/js/jquery-ui.js");
    addJs("/js/jquery-ui.timepicker.js");
    addJs("/js/jquery-ui.multiselect.js");
    addJs("/js/jquery.validate.js");
    addJs("/js/jquery.validate.methods.js");
    addJs("/js/jquery.validate.locale_fr-utf8.js");
    addCss("/css/jquery-ui.css");
    addCss("/css/jquery-ui.dataTables.css");
    addCss("/css/jquery-ui.multiselect.css");
    addCss("/css/jquery-ui.timepicker.css");
    addCss("/css/codemirror.css");
  }
}

status
HtmlTemplate::resolve(string& p_result)
{
  string                    l_content;
  cpptempl::data_map             l_data;
  cpptempl::data_list            l_cssList;
  cpptempl::data_list            l_jsList;
  vector<string>::const_iterator cc_item;
  Template                       l_template;

  if (status::ok != Template::resolve(l_content))
  {
    // log
    return status::error;
  }

  for (cc_item = m_cssList.begin(); cc_item != m_cssList.end(); cc_item++)
    l_cssList += val(*cc_item, true);

  for (cc_item = m_jsList.begin(); cc_item != m_jsList.end(); cc_item++)
    l_jsList += val(*cc_item, true);

  l_data["__title"]           = val(m_title, true);
  l_data["__favico"]          = val(m_favico);
  l_data["__content"]         = val(l_content);
  l_data["__cssList"]         = val(l_cssList);
  l_data["__jsList"]          = val(l_jsList);
  l_data["__mimeType"]        = val(m_mimeType, true);
  l_data["__charset"]         = val(m_charset, true);
  l_data["__metaDescription"] = val(m_metaDescription, true);
  l_data["__metaKeywords"]    = val(m_metaKeywords, true);

  l_template.setTemplate(mcs_htmlTemplate);
  l_template.setData(l_data);
  return l_template.resolve(p_result);
}

HtmlTemplate&
HtmlTemplate::setFavico(const string& p_ico)
{
  m_favico = p_ico;
  return *this;
}

/* ------------------------------------------------------------------- */


XmlTemplate::XmlTemplate() :
  Template("text/xml"),
  m_charset("UTF-8"),
  m_xsltUrl()
{
}


status
XmlTemplate::resolve(string& p_result)
{
  string        l_content;
  cpptempl::data_map l_data;
  Template           l_template;

  if (status::ok != Template::resolve(l_content))
  {
    // log
    return status::error;
  }

  l_data["__charset"] = val(m_charset, true);
  l_data["__xsltUrl"] = val(m_xsltUrl, true);
  l_data["__xmlData"] = val(l_content);


  l_template.setTemplate(mcs_xmlTemplate);
  l_template.setData(l_data);
  return l_template.resolve(p_result);
}


/* ------------------------------------------------------------------- */


}}} //end namespaces
