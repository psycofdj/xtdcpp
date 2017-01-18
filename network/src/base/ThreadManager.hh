#ifndef NETWORK_BASE_CLIENTTHREADMANAGER_HH_
# define NETWORK_BASE_CLIENTTHREADMANAGER_HH_

# include <boost/asio.hpp>
# include <mutex>
# include <thread>
# include <memory>
# include <types.hh> //libcore
# include <mixins/singleton.hh> // libcore

namespace xtd {
namespace network {
namespace base {

class ThreadManager : public mixins::Singleton<ThreadManager>
{
  friend class mixins::Singleton<ThreadManager>;

public:
  typedef map<size_t, sptr<std::thread> > t_map;

protected:
  ThreadManager(void);
  ThreadManager(const ThreadManager&) = delete;

public:
  virtual ~ThreadManager(void);

public:
  void                     createThread(const size_t p_threadId);
  boost::asio::io_service& getIoService(void);

private:
  // Attention à l'initialisation d'un pointeur static lors des tests unitaires qui cachent un fork
  // pour garantir l'ordre d'initialisation, création du pointeur au constructeur
  sptr<boost::asio::io_service>       m_ioService;
  sptr<boost::asio::io_service::work> m_work;
  sptr<std::thread>                   m_thread;
  t_map                               m_threadMap;
};

}}} // end namespaces

#endif // !NETWORK_BASE_CLIENTTHREADMANAGER_HH_
