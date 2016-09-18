#ifndef NETWORK_HTTP_REQUEST_HH_
# define NETWORK_HTTP_REQUEST_HH_

# include <string>
# include <vector>
# include <map>
# include <boost/regex.hpp>
# include <boost/noncopyable.hpp>
# include <types.hh> // libcore

namespace xtd {
namespace network {
namespace http {

class Request
{
  friend std::ostream& operator<<(std::ostream&, const Request&);

public:
  typedef std::multimap<string, string> t_param_map;
  typedef map<string, string>      t_header_map;

  enum t_method {
    METHOD_GET  = 0,
    METHOD_POST = 1,
    METHOD_HEAD = 2
  };

  enum t_version {
    VERSION_1_0 = 0,
    VERSION_1_1 = 1
  };

public:
  Request(bool p_cgiToLower = true);

public:
  void setMethod(t_method p_method);
  void addCgi(const string& p_key, const string& p_value);
  void addPost(const string& p_key, const string& p_value);
  void addHeader(const string& p_name, const string& p_value);
  void setVersion(const string& p_version);
  void setVersion(t_version p_version);
  void removeCgi(const string& p_key);
  void removePost(const string& p_key);
  void removeData(const string& p_key);

  status   readQS(const string& p_qs);
  status   readHead(std::istream& p_request);
  status   read(std::istream& p_request);


  t_method            getMethod(void) const;
  const string&  getPath(void) const;
  string         getQS(void) const;
  string         getRawUrl(void) const;
  const t_param_map&  getCgis(void) const;
  const t_param_map&  getPosts(void) const;
  t_param_map         getParams(void) const;
  bool                existsCgi(const string& p_key) const;
  bool                existsPost(const string& p_key) const;
  bool                existsParam(const string& p_key) const;
  bool                existsHeader(const string& p_name) const;
  bool                getHeader(const string& p_name, string& p_value) const;
  const t_header_map& getHeaders(void) const;
  const string&  getVersionStr(void) const;
  t_version           getVersion(void) const;
  status    getDataSize(size_t& p_length) const;



  template<typename T>
  bool getCgi(const string& p_key, T& p_dst) const;
  template<typename T>
  void getCgi(const string& p_key, vector<T>& p_dst) const;
  template<typename T>
  void getCgi(const string& p_key, T& p_dst, const T& p_default) const;
  template<typename T>
  bool getPost(const string& p_key, T& p_dst) const;
  template<typename T>
  void getPost(const string& p_key, vector<T>& p_dst) const;
  template<typename T>
  void getPost(const string& p_key, T& p_dst, const T& p_default) const;
  template<typename T>
  bool getParam(const string& p_key, T& p_dst) const;
  template<typename T>
  void getParam(const string& p_key, vector<T>& p_dst) const;
  template<typename T>
  void getParam(const string& p_key, T& p_dst, const T& p_default) const;
  template<typename T>
  void getParam(const string& p_key, vector<T>& p_dst, const vector<T>& p_default) const;


private:
  status readData(std::istream& p_request);
  status readDataFormEncoded(const string& p_data);
  status readHeaders(std::istream& p_request);
  status readVersion(const string& p_version);
  status readUrl(const string&  p_url);
  status readMethod(const string& p_method);
  status readInitial(std::istream& p_request);
  string paramValueDecode(const string& p_value);
  void        normalizePath(const string& p_path);

  bool as(const string& p_src, string& p_dst) const;
  bool as(const string& p_src, bool&        p_dst) const;
  bool as(const string& p_src, uint8_t& p_dst) const;
  template<typename TDst>
  bool as(const string& p_src, TDst& p_dst) const;

  bool existsData(const t_param_map& p_src, const string& p_key) const;
  template<typename T>
  bool getData(const t_param_map& p_src, const string& p_key, T& p_dst) const;
  template<typename T>
  void getData(const t_param_map& p_src, const string& p_key, vector<T>& p_dst) const;
  template<typename T>
  void getData(const t_param_map& p_src, const string& p_key, T& p_dst, const T& p_default) const;
  template<typename T>
  void getData(const t_param_map& p_src, const string& p_key, vector<T>& p_dst, const vector<T>& p_default) const;

private:
  string  m_rawUrl;
  t_method     m_method;
  string  m_path;
  t_param_map  m_cgi;
  t_param_map  m_post;
  t_header_map m_headers;
  string  m_versionStr;
  t_version    m_version;
  bool         m_cgiToLower;
  boost::regex m_regex_tooManyStartingSlash;
  boost::regex m_regex_tooManyEndingSlash;
};

std::ostream& operator<<(std::ostream& p_buf, const Request& p_obj);

}}} //end namespaces

# include "Request.hxx"


#endif // !NETWORK_HTTP_REQUEST_HH_
