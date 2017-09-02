#ifndef NETWORK_UTILS_RESOLVER_HH_
# define NETWORK_UTILS_RESOLVER_HH_

# include <memory>
# include <types.hh> // libcore
# include <utils/CacheLRU.hh> // libcore
# include "network_types.hh"

namespace xtd {
namespace network {
namespace utils {

/**
 ** @brief Resolution des noms de domaines
 ** @tparam Domain type de connexion @ref af_unix ou @ref af_inet
 ** @details
 ** Cet objet a pour objectif de fournir une interface homogene pour
 ** la création des endpoints pour les différents types de connexions.
 ** Les mécanismes étant radicalement différents en @ref af_unix (socket unix)
 ** et @ref af_inet (socket tcp ipv4), il est intégralement spécialisé.
 ** Dans le cas de @ref af_inet, on vent de résoudre un nom
 ** de domaine en IP en interrogeant les serveurs DNS. Pour ne pas les surcharger
 ** on embarque un cache qui garde en local les résolutions
 ** effectuée.
 */
template <typename Domain>
class Resolver;


template <>
class Resolver<af_inet>
{
public:
  /**
   ** @brief Constructeur
   ** @param p_ioService référence sur le io_server
   ** @param p_ttl duree de vie de la mise en cache des resolutions dns
   */
  Resolver(boost::asio::io_service& p_ioService, uint32_t p_ttl);
  ~Resolver(void);

public:
  /**
   ** @brief Resoudre un dns
   ** @param  p_host  nom de domaine ou ip à résoudre
   ** @param  p_port  port de destination
   ** @return représentation boost asio d'une résolution, un endpoint.
   */
  af_inet::endpoint resolve(const string&  p_host,
                            const string&  p_port);

  /**
   ** @brief Extraction du port à partir d'un endpoint
   */
  uint32_t getPort(const af_inet::endpoint& p_endpoint);

  /**
   ** @brief Extraction de l'IP à partir d'un endpoind.
   **/
  string  getAddr(const af_inet::endpoint& p_endpoint);

private:
  boost::asio::io_service&                  m_ioService;
  sptr<xtd::utils::CacheLRU<string,string>> m_cacheDns;
};


template <>
class Resolver<af_unix>
{
public:
  Resolver(boost::asio::io_service&, uint32_t);
  ~Resolver(void);

public:
  af_unix::endpoint resolve(const string& p_host,
                            const string& p_port);

  uint32_t getPort(const af_unix::endpoint& p_endpoint);
  string  getAddr(const af_unix::endpoint& p_endpoint);

private:
  uint32_t m_port;
};

}}} // end namespaces


#endif // !NETWORK_UTILS_RESOLVER_HH_
