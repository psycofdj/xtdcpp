#ifndef COMMON_APPLICATION_HXX_
# define COMMON_APPLICATION_HXX_

# include <boost/lexical_cast.hpp>

namespace xtd {


template<typename... Arguments>
void
Application::error_nohelp(int p_code, const string& p_format, Arguments&&... p_args) const
{
  string l_message = logger::format_vargs(p_format, p_args...);
  std::cerr << l_message << endl;
  logger::crit("common.application", "%s", l_message);
  exit(p_code);
}

template<typename... Arguments>
void
Application::error(int p_code, const string& p_format, Arguments&&... p_args) const
{
  string l_message = logger::format_vargs(p_format, p_args...);
  std::cerr << "error : " << l_message << endl;
  logger::crit("common.application", "error : %s", l_message);
  usage();
  exit(p_code);
}

template<typename... Arguments>
void
Application::warn(const string& p_format, Arguments&&... p_args) const
{
  string l_message = logger::format_vargs(p_format, p_args...);

  std::cerr << "warning : " << l_message << endl;
  logger::warning("common.application", "warning : %s", l_message);
}

template<typename TCollection>
Application::t_callback
Application::bindAccumulator(TCollection& p_target)
{
  return boost::bind(&Application::h_accumulator<TCollection>, this, _1, _2, boost::ref(p_target));
}

template<typename T>
Application::t_callback
Application::bindNumber(T& p_target, T p_min, T p_max)
{
  return boost::bind(&Application::h_number<T>, this, _1, _2, boost::ref(p_target), p_min, p_max);
}


template<typename T, typename Iterable>
Application::t_callback
Application::bindValues(T& p_target, const Iterable& p_values)
{
  return boost::bind(&Application::h_values<T,Iterable>, this, _1, _2, boost::ref(p_target), p_values);
}

template<typename T>
Application::t_callback
Application::bindValueIfGiven(T& p_target, const T& p_value)
{
  return boost::bind(&Application::h_value_if_given<T>, this, _1, _2, boost::ref(p_target), boost::ref(p_value));
}

template<typename T>
Application::t_callback
Application::bindAction(T p_method)
{
  return boost::bind(&Application::h_action, this, _1, _2, t_action(p_method));
}


template<typename T>
Application::t_callback
Application::bindMethod(T p_method)
{
  return boost::bind(&Application::h_action, this, _1, _2, t_action(boost::bind(this, p_method)));
}



template<typename TCollection>
void
Application::h_accumulator(const string&      p_value,
                           const t_option& /* p_opt */,
                           TCollection&       p_dest)
{
  typename TCollection::value_type l_value;
  try {
    l_value = boost::lexical_cast<typename TCollection::value_type, string>(p_value);
  }
  catch (boost::bad_lexical_cast& l_error) {
    error_nohelp(1, "unable to cast value '%s'", p_value.c_str());
  }
  p_dest.insert(p_dest.end(), l_value);
}


template<typename T>
void
Application::h_value_if_given(const string&    /* p_path */,
                              const t_option&  /* p_opt  */,
                              T&                  p_target,
                              const T&            p_value)
{
  p_target = p_value;
}


template<typename T>
void
Application::h_number(const string&   p_value,
                      const t_option& p_opt,
                      T&              p_target,
                      T               p_min,
                      T               p_max)
{
  try {
    p_target = boost::lexical_cast<T>(p_value);
  }
  catch (boost::bad_lexical_cast) {
    error(1, "invalid option -%c=%s, must be a number", p_opt.m_shortOpt, p_value.c_str());
  }

  if (p_target < p_min)
    error(1, "invalid option -%c=%s, must be superior to %d",
          p_opt.m_shortOpt, p_value.c_str(), p_min);

  if (p_target > p_max)
    error(1, "invalid option -%c=%s, must be inferior to %d",
          p_opt.m_shortOpt, p_value.c_str(), p_max);
}

template<typename T, typename Iterable>
void
Application::h_values(const string&   p_value,
                      const t_option& p_opt,
                      T&              p_target,
                      const Iterable& p_values)
{
  try {
    p_target = boost::lexical_cast<T>(p_value);
  }
  catch (boost::bad_lexical_cast) {
    error(1, "invalid option -%c=%s, has invalid type", p_opt.m_shortOpt, p_value.c_str());
  }

  if (p_values.end() == std::find(p_values.begin(), p_values.end(), p_target))
    error(1, "invalid option -%c=%s, value not in range", p_opt.m_shortOpt, p_value.c_str());
}

}

#endif // !COMMON_APPLICATION_HXX_
