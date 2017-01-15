#ifndef NETWORK_BASE_CLIENTTHREADMANAGER_HH_
# define NETWORK_BASE_CLIENTTHREADMANAGER_HH_

# include <boost/asio.hpp>
# include <mutex>
# include <thread>
# include <memory>
# include <types.hh> //libcore

namespace xtd {
namespace network {
namespace base {

class ThreadManager
{
public:
  typedef map<size_t, std::shared_ptr<std::thread> > threadMap_t;

private:
  ThreadManager(void);
  ThreadManager(const ThreadManager&) = delete;
  ~ThreadManager(void);

public:
  static ThreadManager&    getInstance(void);
  void                     createThread(const size_t p_threadId);
  boost::asio::io_service& getIoService(void);

private:
  // Attention à l'initialisation d'un pointeur static lors des tests unitaires qui cachent un fork
  // pour garantir l'ordre d'initialisation, création du pointeur au constructeur
  static boost::asio::io_service*                m_ioService;
  static std::mutex                              m_mutex;
  static ThreadManager*                          m_threadManager;
  std::shared_ptr<boost::asio::io_service::work> m_workPtr;
  threadMap_t                                    m_threadMap;
  std::shared_ptr<std::thread>                   m_threadPtr;
};

}}} // end namespaces

#endif // !NETWORK_BASE_CLIENTTHREADMANAGER_HH_
