#ifndef NETWORK_BASE_CLIENT_HH_
# define NETWORK_BASE_CLIENT_HH_

# include <string>
# include <boost/asio.hpp>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# include <boost/noncopyable.hpp>
# include <boost/enable_shared_from_this.hpp>

# include <types.hh> // libcommon
# include <logger.hh>   // libcommon

# include "utils/Config.hh"
# include "utils/CommTypeDefs.hh"
# include "utils/Utils.hh"
# include "utils/Resolver.hh"

namespace xtd {
namespace network {
namespace base {



// fwd decl
class ThreadManager;
template <typename Domain> class Connection;

/**
 * TODO explain private + shared_ptr
 * */
template <typename Domain>
class Client : private boost::noncopyable
{
protected:
  typedef std::shared_ptr<Connection<Domain> > cnx_sptr_t;

protected:
  Client(const utils::Config& p_conf);

public:
  virtual ~Client(void);

public:
  status connect(const string& p_hostname,
                      const uint32_t p_port);
  void async_connect(const string& p_hostname,
                     const uint32_t p_port);
  status wait_async_connect();
  status reconnect(void);
  void        close(void);

public:
  inline const uint32_t& getCnxTotal(void) const        { return m_cnxTotal; }
  inline const uint32_t& getCnxSuccess(void) const      { return m_cnxSuccess; }
  inline const uint32_t& getCnxTimeout(void) const      { return m_cnxTimeout; }
  inline const uint32_t& getCnxError(void) const        { return m_cnxError; }

protected:
  virtual cnx_sptr_t createCnx(string p_hostname, uint32_t p_port) = 0;
  virtual void       onConnected(const boost::system::error_code p_error);

private:
  ThreadManager&                              m_threadManager;
  std::shared_ptr<utils::Resolver<Domain> > m_resolver;
  string                                 m_hostname;
  uint32_t                               m_port;
  boost::interprocess::interprocess_semaphore m_semaphoreConnect;

protected:
  utils::Config m_conf;
  boost::asio::io_service&         m_ioService;
  cnx_sptr_t                       m_connection;
  status                 m_connectStatus;

private:
  uint32_t m_cnxTotal;
  uint32_t m_cnxSuccess;
  uint32_t m_cnxTimeout;
  uint32_t m_cnxError;
};

}}} //end namepaces


#endif // !NETWORK_BASE_CLIENT_HH_
