// IWYU pragma: private, include "http/Request.hh"
#ifndef NETWORK_HTTP_REQUEST_HXX_
# define NETWORK_HTTP_REQUEST_HXX_
# include <boost/lexical_cast.hpp>
# include "types.hh"                // libcore
# include "log/helpers.hh"          // libcore
# include "log/logtypes.hh"         // libcore

namespace xtd {
namespace network {
namespace http {

template<typename T>
bool
Request::getCgi(const string& p_key, T& p_dst) const
{
  return getData(m_cgi, p_key, p_dst);
}

template<typename T>
void
Request::getCgi(const string& p_key, vector<T>& p_dst) const
{
  getData(m_cgi, p_key, p_dst);
}

template<typename T>
void
Request::getCgi(const string& p_key, T& p_dst, const T& p_default) const
{
  getData(m_cgi, p_key, p_dst, p_default);
}



template<typename T>
bool
Request::getPost(const string& p_key, T& p_dst) const
{
  return getData(m_post, p_key, p_dst);
}

template<typename T>
void
Request::getPost(const string& p_key, vector<T>& p_dst) const
{
  getData(m_post, p_key, p_dst);
}

template<typename T>
void
Request::getPost(const string& p_key, T& p_dst, const T& p_default) const
{
  getData(m_post, p_key, p_dst, p_default);
}

template<typename T>
bool
Request::getParam(const string& p_key, T& p_dst) const
{
  return getData(m_cgi, p_key, p_dst) || getData(m_post, p_key, p_dst);
}

template<typename T>
void
Request::getParam(const string& p_key, vector<T>& p_dst) const
{
  getData(m_cgi, p_key, p_dst);
  getData(m_post, p_key, p_dst);
}

template<typename T>
void
Request::getParam(const string& p_key, T& p_dst, const T& p_default) const
{
  if ((false == getData(m_cgi,  p_key, p_dst)) &&
      (false == getData(m_post, p_key, p_dst)))
    p_dst = p_default;
}

template<typename T>
void
Request::getParam(const string& p_key, vector<T>& p_dst, const vector<T>& p_default) const
{
  getData(m_cgi,  p_key, p_dst);
  getData(m_post, p_key, p_dst);
  if (p_dst.size() == 0)
    p_dst = p_default;
}


template<typename TDst>
bool
Request::as(const string& p_src, TDst& p_dst) const
{
  try {
    p_dst = boost::lexical_cast<TDst>(p_src);
  }
  catch (boost::bad_lexical_cast&) {
    log::err("network.http.request", "warning : invalid conversion of key %s", p_src, HERE);
    return false;
  }
  return true;
}

template<typename T>
bool
Request::getData(const t_param_map& p_src, const string& p_key, T& p_dst) const
{
  t_param_map::const_iterator c_iter = p_src.find(p_key);
  if (c_iter == p_src.end())
    return false;
  return as(c_iter->second, p_dst);
}

template<typename T>
void
Request::getData(const t_param_map& p_src, const string& p_key, vector<T>& p_dst) const
{
  t_param_map::const_iterator c_iter = p_src.find(p_key);
  while ((c_iter != p_src.end()) &&
         (c_iter->first == p_key))
  {
    T l_value;
    if (true == as(c_iter->second, l_value))
      p_dst.push_back(l_value);
    c_iter++;
  }
}

template<typename T>
void
Request::getData(const t_param_map& p_src, const string& p_key, T& p_dst, const T& p_default) const
{
  if (false == getData(p_src, p_key, p_dst))
    p_dst = p_default;
}

template<typename T>
void
Request::getData(const t_param_map& p_src, const string& p_key, vector<T>& p_dst, const vector<T>& p_default) const
{
  getData(p_src, p_key, p_dst);
  if (p_dst.size() == 0)
    p_dst = p_default;
}


}}}


#endif // !NETWORK_HTTP_REQUEST_HH_
