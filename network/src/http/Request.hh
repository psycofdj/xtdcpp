#ifndef NETWORK_HTTP_REQUEST_HH_
# define NETWORK_HTTP_REQUEST_HH_

# include <string>
# include <vector>
# include <map>
# include <regex>
# include <boost/noncopyable.hpp>
# include <types.hh> // libcore
# include "http/Message.hh"

namespace xtd {
namespace network {
namespace http {

class Request : public Message
{
  friend std::ostream& operator<<(std::ostream&, const Request&);

public:
  typedef std::multimap<string, string> t_param_map;

public:
  Request(void);

public:
  void setPath(const string& p_path);
  void setMethod(method p_method);
  void addCgi(const string& p_key, const string& p_value);
  void addPost(const string& p_key, const string& p_value);
  void removeCgi(const string& p_key);
  void removePost(const string& p_key);
  void removeData(const string& p_key);

  method             getMethod(void) const;
  const string&      getPath(void) const;
  string             getQS(void) const;
  string             getRawUrl(void) const;
  const t_param_map& getCgis(void) const;
  const t_param_map& getPosts(void) const;
  t_param_map        getParams(void) const;
  bool               existsCgi(const string& p_key) const;
  bool               existsPost(const string& p_key) const;
  bool               existsParam(const string& p_key) const;

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
  status readInitial(std::istream& p_request) override;
  status readUrl(const string&  p_url);
  status readMethod(const string& p_method);
  status readData(std::istream& p_request) override;
  status readDataFormEncoded(const string& p_data);
  void   writeInitial(std::ostream& p_stream) const override;
  void   normalizePath(const string& p_path);

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
  string      m_rawUrl;
  method      m_method;
  string      m_path;
  t_param_map m_cgi;
  t_param_map m_post;
  std::regex  m_regex_tooManyStartingSlash;
  std::regex  m_regex_tooManyEndingSlash;
};

std::ostream& operator<<(std::ostream& p_buf, const Request& p_obj);

}}} //end namespaces

# include "Request.hxx"


#endif // !NETWORK_HTTP_REQUEST_HH_
