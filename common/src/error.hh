#ifndef COMMON_ERROR_HH_
# define COMMON_ERROR_HH_
# include "types.hh"
# include "logger.hh"


namespace xtd {

template<class TType, typename... Arguments>
void do_throw(const string& p_module, const string& p_format, Arguments&&... p_args)
{
  logger::crit(p_module, p_format, p_args...);
  throw TType(logger::format_vargs(p_format + " in %s:%s:%d ", p_args...));
}

class error : std::exception
{
public:
  template<typename... Arguments>
  static void do_throw(const string& p_module, const string& p_format, Arguments&&... p_args)
  {
    xtd::do_throw<error>(p_module, p_format, p_args...);
  }

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
