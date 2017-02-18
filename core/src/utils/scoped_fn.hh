#ifndef CORE_UTILS_SCOPED_FN_HH_
# define CORE_UTILS_SCOPED_FN_HH_
# include <functional>

namespace xtd {
namespace utils {

class scoped_fn
{
public:
  explicit scoped_fn(std::function<void(void)> p_handler) :
    m_handler(p_handler)
  {
  }

  scoped_fn(const scoped_fn&) = delete;

  ~scoped_fn(void) {
    m_handler();
  }

private:
  std::function<void(void)> m_handler;
};

}}

#endif // !CORE_UTILS_SCOPED_FN_HH_
