#ifndef CORE_MIXINS_SHARED_HH_
# define CORE_MIXINS_SHARED_HH_

namespace xtd {
namespace mixins {

template<class TObj>
class Shared
{
public:
  typedef std::shared_ptr<TObj> type;

public:
  template<typename... Args>
  static std::shared_ptr<TObj> create(Args&&... p_args)
  {
    TObj* l_ptr = new TObj(p_args...);
    return std::shared_ptr<TObj>(l_ptr);
  }
};

}}

#endif // !CORE_MIXINS_SHARED_HH_
