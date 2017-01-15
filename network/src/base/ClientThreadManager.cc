#include "base/ClientThreadManager.hh"
#include <functional>
#include <iostream>
namespace xtd {
namespace network {
namespace base {


ThreadManager::ThreadManager(void) :
  m_ioService(),
  m_work(),
  m_thread(),
  m_threadMap()
{
  m_ioService.reset(new boost::asio::io_service());
  m_work.reset(new boost::asio::io_service::work(*m_ioService));
  m_thread.reset(new std::thread([this](void) { m_ioService->run(); }));
}


ThreadManager::~ThreadManager(void)
{
  m_work.reset();
  for (auto& c_thread : m_threadMap) {
    c_thread.second->join();
  }
  m_threadMap.clear();
  m_thread->join();
  m_thread.reset();
  m_ioService.reset();
}


void
ThreadManager::createThread(const size_t p_threadId)
{
  t_map::iterator l_threadMapIt = m_threadMap.find(p_threadId);
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


}}} // end namespace
