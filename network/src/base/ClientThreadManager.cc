#include "base/ClientThreadManager.hh"
#include <functional>

namespace xtd {
namespace network {
namespace base {

std::mutex                ThreadManager::m_mutex;
boost::asio::io_service*  ThreadManager::m_ioService     = 0;
ThreadManager*            ThreadManager::m_threadManager = 0;


ThreadManager::ThreadManager(void)
{
  m_ioService = new boost::asio::io_service;
  m_workPtr.reset(new boost::asio::io_service::work(*m_ioService));
  m_threadPtr.reset(new std::thread([this](void) { m_ioService->run(); }));
}


ThreadManager::~ThreadManager(void)
{
  m_workPtr.reset();
  threadMap_t::iterator l_threadMapIt = m_threadMap.begin();
  while (l_threadMapIt != m_threadMap.end())
  {
    (l_threadMapIt->second)->join();
  }
}


void
ThreadManager::createThread(const size_t p_threadId)
{
  threadMap_t::iterator l_threadMapIt = m_threadMap.find(p_threadId);
  if (l_threadMapIt == m_threadMap.end())
  {
    m_threadMap[p_threadId] = std::make_shared<std::thread>([this](void) {
        m_ioService->run();
      });
  }
}


boost::asio::io_service&
ThreadManager::getIoService(void)
{
  return *m_ioService;
}


ThreadManager&
ThreadManager::getInstance(void)
{
  std::lock_guard<std::mutex> l_lock(m_mutex);
  if (!m_threadManager)
  {
    m_threadManager = new ThreadManager();
  }
  return *m_threadManager;
}

}}} // end namespace
