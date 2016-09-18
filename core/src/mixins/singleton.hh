#pragma once
#include <memory>

namespace xtd {
namespace mixins {

/**
 ** @brief Singleton utility class
 ** @tparam TClass Base class
 ** @details
 ** This objects uses the CRTP pattern. To make a class T a singleton, it must inherit of
 ** Singleton<T>.
 **
 ** @code
 ** class MyHeavyObject : public xtd::mixin::Singleton<MyHeavyObject>
 ** {
 **   // your stuff here
 ** };
 **
 ** int main(int,char**)
 ** {
 **   MyHeavyObject& l_instance = MyHeavyObject::get();
 ** }
 ** @endcode
 */
template<class TClass>
class Singleton
{
public:
  /**
   ** @brief Default constructor
   */
  Singleton(void) { };

  /**
   ** @brief Deleted copy constructor
   */
  Singleton(const Singleton&) = delete;

public:
  /**
   ** @brief Construct TClass instance if needed and return them
   ** @tparam Args Variable number of template argumnet
   ** @param p_args TClass constructor arguments
   ** @return Singleton instance
   */
  template<typename ... Args>
  static TClass& get(Args... p_args)
  {
    if (nullptr == ms_instance.get())
      ms_instance.reset(new TClass(p_args...));
    return *ms_instance;
  }

private:
  static std::shared_ptr<TClass> ms_instance;
};

/**
 ** @brief Singleton static instance
 */
template<class TClass>
std::shared_ptr<TClass> Singleton<TClass>::ms_instance;

}}
