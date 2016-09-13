#pragma once
#include <memory>

namespace xtd {
namespace mixin {

template<class T>
class Singleton
{
public:
  Singleton(void) { }
  Singleton(const Singleton&) = delete;

public:
  template<typename ... Args>
  static T& get(Args... p_args)
  {
    if (nullptr == ms_instance.get())
      ms_instance.reset(new T(p_args...));
    return *ms_instance;
  }

protected:
  static std::shared_ptr<T> ms_instance;
};

template<class T>
std::shared_ptr<T> Singleton<T>::ms_instance;

}}
