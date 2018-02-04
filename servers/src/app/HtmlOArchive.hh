#ifndef SERVERS_APP_HTMLOARCHIVE_HH_
# define SERVERS_APP_HTMLOARCHIVE_HH_
# include <boost/archive/detail/archive_serializer_map.hpp> // IWYU pragma: keep
# include <boost/archive/impl/archive_serializer_map.ipp>   // IWYU pragma: keep
# include <boost/archive/xml_oarchive.hpp>                  // IWYU pragma: keep
# include <boost/serialization/pfto.hpp>
# include <deque>
# include <list>
# include <stack>
# include <sstream>
# include "types.hh" // libcore

namespace boost { namespace archive { class class_id_optional_type; } }
namespace boost { namespace archive { class class_id_reference_type; } }
namespace boost { namespace archive { class class_id_type; } }
namespace boost { namespace archive { class object_id_type; } }
namespace boost { namespace archive { class object_reference_type; } }
namespace boost { namespace archive { class save_access; } }
namespace boost { namespace archive { class version_type; } }
namespace boost { namespace archive { namespace detail { template <class Archive> class interface_oarchive; } } }
namespace boost { namespace archive { struct class_name_type; } }
namespace boost { namespace archive { struct tracking_type; } }
namespace boost { namespace archive { template <class Archive> class basic_xml_oarchive; } }
namespace boost { namespace serialization { class collection_size_type; } }
namespace boost { namespace serialization { class item_version_type; } }
namespace boost { namespace serialization { template <class T> struct nvp; } }

namespace xtd {
namespace servers {
namespace app {

class HtmlOArchive :
    public boost::archive::xml_oarchive_impl<HtmlOArchive>
{
  typedef boost::archive::xml_oarchive_impl<HtmlOArchive> base;

  friend class boost::archive::detail::interface_oarchive<HtmlOArchive>;
  friend class boost::archive::basic_xml_oarchive<HtmlOArchive>;
  friend class boost::archive::save_access;

private:
  class Object
  {
  public:
    Object(const string& p_name, size_t p_depth);
    virtual ~Object(void);
  public:
    virtual void write(std::ostream& p_buf);
    virtual void display(std::ostream& p_buf) = 0;

    void write_head(std::ostream& p_buf);
    void write_tail(std::ostream& p_buf);
  public:
    string m_name;
    size_t m_depth;
  };

  class PodObject : public Object
  {
  public:
    PodObject(const string& p_name,
              size_t        p_depth,
              const string& p_type,
              const string& p_default);
  public:
    void display(std::ostream& p_buf);
  public:
    string m_type;
    string m_default;
  };

  class StructObject : public Object
  {
  public:
    StructObject(const string& p_name,
                 size_t        p_depth,
                 unsigned int       p_classID,
                 unsigned int       p_objectID,
                 unsigned int       p_version,
                 unsigned int       p_trackinglevel);
    virtual ~StructObject(void);

  public:
    void write(std::ostream& p_buf);
    virtual void display(std::ostream& p_buf);

  public:
    unsigned int         m_classID;
    unsigned int         m_objectID;
    unsigned int         m_version;
    unsigned int         m_tracking;
    vector<Object*> m_childs;
  };


  class VectorObject : public StructObject
  {
  public:
    VectorObject(const string& p_name,
                 size_t        p_depth,
                 unsigned int       p_classID,
                 unsigned int       p_objectID,
                 unsigned int       p_version,
                 unsigned int       p_trackinglevel,
                 unsigned int       p_itemVersion);

  public:
    void display(std::ostream& p_buf);

  private:
    unsigned m_itemVersion;
  };



private:
  class context
  {
  public:
    context(void);
  public:
    size_t  m_depth;
    string  m_fieldName;
    unsigned int m_classID;
    unsigned int m_tracking;
    unsigned int m_version;
    unsigned int m_objectID;
    string  m_type;
    string  m_value;
    unsigned int m_itemVersion;
  };

public:
  template<class T>
  HtmlOArchive& operator*(T& p_value)
  {
    return base::operator&(p_value);
  }

  template<class T>
  HtmlOArchive& operator/(T& p_value)
  {
    return *this * p_value;
  }

  void save_binary(void *address, size_t count);
  void save_start(const char* p_name);
  void save_end(const char* p_name);

  template<class T>
  void save_override(T & t, BOOST_PFTO int);

  template<class T>
  void save_override(const boost::serialization::nvp<T>& t, int);

  template<class T>
  void save_override(const boost::serialization::nvp<vector<T> >& t, int);
  template<class T>
  void save_override(const boost::serialization::nvp<std::deque<T> >& t, int);
  template<class T>
  void save_override(const boost::serialization::nvp<std::list<T> >& t, int);

  void save(const char* /* t */);
  void save(const string& /* t */);
  void save(const unsigned char& /* t */);
  void save(const unsigned short& /* t */);
  void save(const unsigned int& /* t */);
  void save(const unsigned long& /* t */);
  void save(const unsigned long long& /* t */);
  void save(const char& /* t */);
  void save(const short& /* t */);
  void save(const int& /* t */);
  void save(const long& /* t */);
  void save(const long long& /* t */);
  void save(const bool& /* t */);
  void save(const double& /* t */);
  void save(const float& /* t */);
  void save(const boost::serialization::collection_size_type&);
  void save(const boost::serialization::item_version_type&);
  void save_override(const boost::archive::object_id_type & t, int);
  void save_override(const boost::archive::object_reference_type&, int);
  void save_override(const boost::archive::version_type & t, int);
  void save_override(const boost::archive::class_id_type & t, int);
  void save_override(const boost::archive::class_id_reference_type&, int);
  void save_override(const boost::archive::class_id_optional_type & t, int);
  void save_override(const boost::archive::class_name_type&, int);
  void save_override(const boost::archive::tracking_type & t, int);
public:
  HtmlOArchive(std::ostream& p_stream);

public:
  typedef std::stack<context> t_info;
  typedef std::stack<Object*> t_objects;

  t_info    m_info;
  t_objects m_objects;

private:
  bool          m_pass;
  size_t   m_depth;
  std::ostream& m_os;
  bool          m_isVector;
};

template<typename T>
std::ostream& to_javascript(std::ostream& p_stream);

}}} //end namespaces

# include "app/HtmlOArchive.hxx" // IWYU pragma: export

BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::servers::app::HtmlOArchive)

#endif // !SERVERS_APP_HTMLOARCHIVE_HH_
