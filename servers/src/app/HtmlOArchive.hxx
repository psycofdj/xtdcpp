// IWYU pragma: private, include "app/HtmlOArchive.hh"
#ifndef SERVERS_APP_HTMLOARCHIVE_HXX_
# define SERVERS_APP_HTMLOARCHIVE_HXX_
# include <boost/serialization/nvp.hpp>
// # include <boost/serialization/pfto.hpp>
# include <deque>
# include <list>
# include "types.hh"

namespace xtd {
namespace servers {
namespace app {

template<class T>
void
HtmlOArchive::save_override(T & t SAVE_OVERRIVE_PROTO)
{
  base::save_override(t SAVE_OVERRIVE_CALL);
}


template<class T>
void
HtmlOArchive::save_override(const boost::serialization::nvp<T>& t SAVE_OVERRIVE_PROTO)
{
  if ((0 != t.name()) && (string(t.name()) == "count"))
    return;

  if ((0 != t.name()) && (string(t.name()) == "item_version"))
    m_pass = true;
  else
    m_pass = false;

  base::save_override(t SAVE_OVERRIVE_CALL);
}

template<class T>
void
HtmlOArchive::save_override(const boost::serialization::nvp<vector<T> >& t SAVE_OVERRIVE_PROTO)
{
  vector<T> l_val;
  l_val.resize(1);
  m_isVector = true;
  base::save_override(boost::serialization::make_nvp(t.name(), l_val) SAVE_OVERRIVE_CALL);
}


template<class T>
void
HtmlOArchive::save_override(const boost::serialization::nvp<std::deque<T> >& t SAVE_OVERRIVE_PROTO)
{
  std::deque<T> l_val;
  l_val.resize(1);
  m_isVector = true;
  base::save_override(boost::serialization::make_nvp(t.name(), l_val) SAVE_OVERRIVE_CALL);
}


template<class T>
void
HtmlOArchive::save_override(const boost::serialization::nvp<std::list<T> >& t SAVE_OVERRIVE_PROTO)
{
  std::list<T> l_val;
  l_val.resize(1);
  m_isVector = true;
  base::save_override(boost::serialization::make_nvp(t.name(), l_val) SAVE_OVERRIVE_CALL);
}


template<typename T>
std::ostream& to_javascript(std::ostream& p_stream)
{
  stringstream l_dummy;
  HtmlOArchive      l_ar(l_dummy);
  T                 l_req;

  l_ar << boost::serialization::make_nvp("object", l_req);
  l_ar.m_objects.top()->write(p_stream);
  return p_stream;
}

}}} //end namespaces

#endif // !SERVERS_APP_HTMLOARCHIVE_HXX_
