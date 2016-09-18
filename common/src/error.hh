#ifndef COMMON_ERROR_HH_
# define COMMON_ERROR_HH_
# include "types.hh"
# include "logger.hh"

/**
 ** @brief Declare a new exception class
 ** @param p_name name of the exception class
 ** @param p_base base class, usually xtd::error
 */
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

/**
 ** @brief XTD exception base class
 */
class error : public std::exception
{

public:
  /**
   ** @brief Constructor
   ** @param p_message Carried error message
   */
  error(const string& p_message) :
    m_message(p_message)
  { }

  /**
   ** @brief Destructor
   */
  virtual ~error(void)
  { }

public:
  /**
   ** @brief Returns exception's message
   */
  virtual const char* what() const throw()
  {
    return m_message.c_str();
  }

private:
  string m_message;
};

}

#endif // !COMMON_ERROR_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
