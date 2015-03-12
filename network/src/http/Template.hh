#ifndef NETWORK_HTTP_TEMPLATE_HH_
# define NETWORK_HTTP_TEMPLATE_HH_

# include <string>
# include <boost/property_tree/ptree.hpp>
# include <types.hh> // libcommon
# include "cpptempl.hh"

namespace xtd {
namespace network {
namespace http {

class Generator
{
protected:
  static const string mcs_defaultContentType;

public:
  Generator(const string& p_contentType = mcs_defaultContentType);
  virtual ~Generator(void);

public:
  inline const string& getContentType(void) const;
  inline const string& getError(void) const;

protected:
  inline void addError(const string& p_error);

public:
  virtual status resolve(string& p_result) = 0;

private:
  string m_contentType;
  string m_error;
};


class Json : public Generator
{
private:

public:
  Json(void);
  ~Json(void);

public:
  status resolve(string& p_result);

  template<typename T>
  void add(const string& p_path,
           const string& p_key,
           const T&           p_val);

private:
  boost::property_tree::ptree m_root;
};

class Template : public Generator
{
public:
  typedef cpptempl::data_map  t_map;
  typedef cpptempl::data_list t_list;
  typedef cpptempl::data_ptr  t_val;

public:
  template<class T>
  static t_val val(T p_val);
  static t_val val(const string& p_val, bool p_encode = false);

public:
  Template(const string& p_contentType = mcs_defaultContentType);
  virtual ~Template(void);

public:
  inline Template& setTemplate(const string& p_text);
  inline Template& setData(cpptempl::data_map& p_data);
  inline Template& setContent(const string& p_text);

public:
  virtual status resolve(string& p_result);

public:
  status loadTemplate(const string& p_path);

  template<typename T>
  Template& add(const string& p_key,
                T                  p_val);

  Template& add(const string& p_key,
                const string& p_val,
                bool               p_encode);

private:
  string        m_template;
  cpptempl::data_map m_data;
  string        m_content;
};



class HtmlTemplate : public Template
{
private:
  static const string mcs_htmlTemplate;

public:
  HtmlTemplate(bool p_defaultAssests = false);

public:
  inline HtmlTemplate& setFavico(const string& p_ico);
  inline HtmlTemplate& setTitle(const string& p_title);
  inline HtmlTemplate& addCss(const string& p_cssUrl);
  inline HtmlTemplate& addJs(const string& p_jsUrl);
  inline HtmlTemplate& setMetaKeywords(const string&  p_keywords);
  inline HtmlTemplate& setCharset(const string& p_charset);
  inline HtmlTemplate& setMetaDescription(const string& p_description);
  inline HtmlTemplate& setContentType(const string& p_mimeType,
                                      const string& p_charset);

public:
  status resolve(string& p_result);

private:
  string              m_title;
  vector<string> m_cssList;
  vector<string> m_jsList;
  string              m_metaDescription;
  string              m_metaKeywords;
  string              m_mimeType;
  string              m_charset;
  string              m_favico;
};




class XmlTemplate : public Template
{
private:
  static const string mcs_xmlTemplate;

public:
  XmlTemplate(void);

public:
  inline XmlTemplate& setXslt(const string& p_xsltUrl);
  inline XmlTemplate& setCharset(const string& p_charset);

public:
  status resolve(string& p_result);

private:
  string m_charset;
  string m_xsltUrl;
};

}}} //end namespaces

# include "Template.hxx"

#endif // !NETWORK_HTTP_TEMPLATE_HH_
