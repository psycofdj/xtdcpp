#ifndef SERVERS_APP_HTTPSERVER_HXX_
# define SERVERS_APP_HTTPSERVER_HXX_


namespace xtd {
namespace servers {
namespace app {

template<typename TType>
status
HttpServer::addProbe(const TType&  p_value,
                     const string& p_path,
                     const string& p_name)
{
  t_counter l_counter(new counters::ExtValue<TType>(p_name, p_value));
  return addProbe(l_counter, p_path);
}


template<typename T>
status
HttpServer::readConf(const string& p_key,
                     T&            p_dst,
                     const T&      p_default)
{
  string l_default;

  if (status::ok != m_config->get_default(p_key, p_dst, p_default))
  {
    l_default = boost::lexical_cast<string>(p_default);
    warn("unable to ready key '%s' from configuration, using default '%s'", p_key, l_default);
    return status::notfound;
  }
  return status::ok;
}


template<typename T>
status
HttpServer::readConf(const std::string& p_key, T& p_dst)
{
  if (status::ok != m_config->get(p_key, p_dst))
    error_nohelp(1, "unable to read mandatory key '%s' from configuration", p_key);
  return status::ok;
}

}}} //end namespaces


#endif // !SERVERS_APP_HTTPSERVER_HXX_
