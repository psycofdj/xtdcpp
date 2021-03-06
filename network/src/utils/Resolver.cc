#include "utils/Resolver.hh"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include "log/helpers.hh"    // libcore
#include "log/logtypes.hh"   // libcore
#include "types.hh"          // libcore
#include "utils/CacheLRU.hh" // libcore



namespace xtd {
namespace network {
namespace utils {


Resolver<af_inet>::Resolver(boost::asio::io_service& p_ioService, uint32_t p_ttlSec) :
  m_ioService(p_ioService)
{
  m_cacheDns = std::make_shared<xtd::utils::CacheLRU<string,string>>(200, p_ttlSec);
}

Resolver<af_inet>::~Resolver(void)
{
}


af_inet::endpoint
Resolver<af_inet>::resolve(const string&  p_host,
                           const string&  p_port)
{
  string l_ipAddress;
  bool   l_isCached = m_cacheDns->pop(p_host, l_ipAddress);

  if (l_isCached)
  {
    // the element was retrieved from the cache
    // build af_inet::endpoint
    uint32_t l_port = boost::lexical_cast<uint32_t>(p_port);
    string  l_addr = l_ipAddress;
    l_addr += "_";
    l_addr += p_port;
    af_inet::endpoint l_endpoint(boost::asio::ip::address::from_string(l_ipAddress), l_port);
    return l_endpoint;
  }
  else
  {
    // perform resolution of the host
    af_inet::resolver        l_resolver(m_ioService);
    af_inet::resolver::query l_query(p_host, p_port);
    af_inet::endpoint        l_endpoint;
    try
    {
      l_endpoint = *l_resolver.resolve(l_query);
      // insertion in the cache;
      m_cacheDns->push(p_host, l_endpoint.address().to_string());
    }
    catch(const boost::system::system_error& ex)
    {
      log::crit("network.utils", "Can't resolve hostname %s:%s", p_host, p_port, HERE);
    }
    return l_endpoint;
  }
}


uint32_t
Resolver<af_inet>::getPort(const af_inet::endpoint& p_endpoint)
{
  return p_endpoint.port();
}


string
Resolver<af_inet>::getAddr(const af_inet::endpoint& p_endpoint)
{
  return p_endpoint.address().to_string();
}


// remove warning p_dnsCacheTTL unused
Resolver<af_unix>::Resolver(boost::asio::io_service&, uint32_t)
{
}


Resolver<af_unix>::~Resolver(void)
{
};

// utilisation d'une Linux Abstract Socket Namespace
// pour éviter d'avoir à gérer (côté server) la création d'une socket
// physique avec un path ... son effacement ...
af_unix::endpoint
Resolver<af_unix>::resolve(const string& p_host,
                           const string& p_port)
{
  m_port = boost::lexical_cast<uint32_t>(p_port);
  // '\0' Permet d'eviter la creation d'un fichier sur le filesystem
  string l_addr = '\0' + p_port + "_" + p_host;
  af_unix::endpoint l_endpoint(l_addr);
  return l_endpoint;
}

uint32_t
Resolver<af_unix>::getPort(const af_unix::endpoint&)
{
  return m_port;
}


string
Resolver<af_unix>::getAddr(const af_unix::endpoint& p_endpoint)
{
  return p_endpoint.path();
}

}}} // end namespaces
