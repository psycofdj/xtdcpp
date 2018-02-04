#ifndef SERVERS_PARAM_POD_HXX_
# define SERVERS_PARAM_POD_HXX_
# include "types.hh" // libcore
# include <boost/algorithm/string/case_conv.hpp>
# include <boost/lexical_cast.hpp>
# include "param/Base.hh"
# include "param/Pod.hh"
# include "param/Visitor.hh" // IWYU pragma: keep

namespace xtd {
namespace servers {
namespace param {

using namespace boost;

template<typename T>
POD<T>::POD(const T& p_value) :
  Base(p_value)
{
}


template<typename T>
POD<T>::POD(const T& p_value, const string& p_name) :
  Base(p_value, p_name)
{
}


template<typename T>
bool
POD<T>::toStr(string& p_dst) const
{
  T l_value = T();
  return get(l_value) && as(l_value, p_dst);
}


template<typename T>
bool
POD<T>::fromStr(const string& p_src)
{
  T l_value = T();
  return to(p_src, l_value) && set(l_value);
}

template<typename T>
bool
POD<T>::verify(const string& p_src)
{
  T l_value = T();
  return to(p_src, l_value) && isAccepted(l_value);
}


template<typename T>
void
POD<T>::accept(Visitor& p_visitor) const
{
  p_visitor(*this);
}


template<typename T>
bool
POD<T>::as(const bool& p_src, string& p_dst) const
{
  if (p_src)
    p_dst = "1";
  else
    p_dst = "0";
  return true;
}


template<typename T>
template<typename V>
bool
POD<T>::as(const V& p_src, string& p_dst) const
{
  p_dst = lexical_cast<string>(p_src);
  return true;
}


template<typename T>
bool
POD<T>::to(const string& p_src, bool& p_dst) const
{
  string l_src = to_lower_copy(p_src);

  if ((l_src == "0") || (l_src == "off") || (l_src == "no") || (l_src == "false"))
  {
    p_dst = false;
    return true;
  }
  if ((l_src == "1") || (l_src == "on") || (l_src == "yes") || (l_src == "true"))
  {
    p_dst = true;
    return true;
  }
  return false;
}


template<typename T>
template<typename V>
bool
POD<T>::to(const string& p_src, V& p_dst) const
{
  try {
    p_dst = lexical_cast<V>(p_src);
    return true;
  } catch (bad_lexical_cast) {
    return false;
  }
}


}}}

#endif // !SERVERS_PARAM_POD_HXX_
