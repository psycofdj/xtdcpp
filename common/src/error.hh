#ifndef COMMON_ERROR_HH_
# define COMMON_ERROR_HH_
# include "types.hh"
# include "logger.hh"

# define xtd_error_class(p_name, p_base)                                \
  class p_name : public p_base                                          \
  {                                                                     \
  private:                                                              \
    p_name(const std::string& p_msg) :                                  \
      p_base(p_msg)                                                     \
    { }                                                                 \
                                                                        \
  public:                                                               \
    template<typename... Arguments>                                     \
      static void raise(const string& p_module, const string& p_format, Arguments&&... p_args) \
    {                                                                   \
      logger::crit(p_module, p_format, p_args...);                      \
      throw p_name(format::vargs_noexcept(p_format + " in %s:%s:%d ", p_args...)); \
    }                                                                   \
  };

namespace xtd {

class error : public std::exception
{

public:
  error(const string& p_message) :
    m_message(p_message)
  { }
  virtual ~error(void)
  { }

public:
  virtual const char* what() const throw()
  {
    return m_message.c_str();
  }

private:
  string m_message;
};

}

#endif // !COMMON_ERROR_HH_
