#ifndef SERIALIZER_DEBUGARCHIVE_HH_
# define SERIALIZER_DEBUGARCHIVE_HH_

# include <boost/serialization/serialization.hpp>
# include <boost/serialization/version.hpp>
# include <boost/serialization/base_object.hpp>
# include <boost/serialization/nvp.hpp>
# include <boost/serialization/list.hpp>
# include <boost/serialization/string.hpp>
# include <boost/serialization/vector.hpp>
# include <boost/serialization/deque.hpp>
# include <boost/serialization/utility.hpp>
# include <boost/serialization/map.hpp>
# include <boost/archive/xml_oarchive.hpp>
# include <boost/archive/text_oarchive.hpp>
# include <boost/archive/binary_oarchive.hpp>
# include <boost/archive/xml_iarchive.hpp>
# include <boost/archive/text_iarchive.hpp>
# include <boost/archive/binary_iarchive.hpp>
// # include "objects/unorderedmap.hh"

namespace xtd {
namespace serializer {

class DebugXmlOArchive : public boost::archive::xml_oarchive_impl<DebugXmlOArchive>
{
  typedef      boost::archive::xml_oarchive_impl<DebugXmlOArchive> base;
  friend class boost::archive::detail::interface_oarchive<DebugXmlOArchive>;
  friend class boost::archive::basic_xml_oarchive<DebugXmlOArchive>;
  friend class boost::archive::save_access;

public:
  DebugXmlOArchive(std::ostream& p_os, bool p_debugState = false, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(p_debugState),
    m_statusSaved(false)
  {}


  template<class T>
  DebugXmlOArchive& operator*(T& p_value)
  {
    if (false == m_statusSaved)
    {
      //std::cout << "saving debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusSaved = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugXmlOArchive& operator/(T& p_value)
  {
    //std::cout << "should save debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "saving debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

private:
  bool m_debugMode;
  bool m_statusSaved;
};


class DebugXmlIArchive : public boost::archive::xml_iarchive_impl<DebugXmlIArchive>
{
  typedef      boost::archive::xml_iarchive_impl<DebugXmlIArchive> base;
  friend class boost::archive::detail::interface_iarchive<DebugXmlIArchive>;
  friend class boost::archive::basic_xml_iarchive<DebugXmlIArchive>;
  friend class boost::archive::save_access;

public:
  DebugXmlIArchive(std::istream& p_os, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(false),
    m_statusLoaded(false)
  {}

  template<class T>
  DebugXmlIArchive& operator*(T& p_value)
  {
    if (false == m_statusLoaded)
    {
      //std::cout << "loading debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusLoaded = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugXmlIArchive& operator/(T& p_value)
  {
    //std::cout << "should load debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "loading debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

  bool getDebugState(void) const
  {
    return m_debugMode;
  }

private:
  bool m_debugMode;
  bool m_statusLoaded;
};



class DebugTextOArchive : public boost::archive::text_oarchive_impl<DebugTextOArchive>
{
  typedef      boost::archive::text_oarchive_impl<DebugTextOArchive> base;
  friend class boost::archive::detail::interface_oarchive<DebugTextOArchive>;
  friend class boost::archive::basic_text_oarchive<DebugTextOArchive>;
  friend class boost::archive::save_access;

public:
  DebugTextOArchive(std::ostream& p_os, bool p_debugState = false, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(p_debugState),
    m_statusSaved(false)
  {}


  template<class T>
  DebugTextOArchive& operator*(T& p_value)
  {
    if (false == m_statusSaved)
    {
      //std::cout << "saving debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusSaved = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugTextOArchive& operator/(T& p_value)
  {
    //std::cout << "should save debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "saving debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

private:
  bool m_debugMode;
  bool m_statusSaved;
};


class DebugTextIArchive : public boost::archive::text_iarchive_impl<DebugTextIArchive>
{
  typedef      boost::archive::text_iarchive_impl<DebugTextIArchive> base;
  friend class boost::archive::detail::interface_iarchive<DebugTextIArchive>;
  friend class boost::archive::basic_text_iarchive<DebugTextIArchive>;
  friend class boost::archive::save_access;

public:
  DebugTextIArchive(std::istream& p_os, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(false),
    m_statusLoaded(false)
  {}

  template<class T>
  DebugTextIArchive& operator*(T& p_value)
  {
    if (false == m_statusLoaded)
    {
      //std::cout << "loading debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusLoaded = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugTextIArchive& operator/(T& p_value)
  {
    //std::cout << "should load debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "loading debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

  bool getDebugState(void) const
  {
    return m_debugMode;
  }

private:
  bool m_debugMode;
  bool m_statusLoaded;
};



class DebugBinOArchive : public boost::archive::binary_oarchive_impl<DebugBinOArchive, std::ostream::char_type, std::ostream::traits_type>
{
  typedef boost::archive::binary_oarchive_impl<DebugBinOArchive, std::ostream::char_type, std::ostream::traits_type> base;

  friend class boost::archive::detail::interface_oarchive<DebugBinOArchive>;
  friend class boost::archive::basic_binary_oarchive<DebugBinOArchive>;
  friend class boost::archive::save_access;

public:
  DebugBinOArchive(std::ostream& p_os, bool p_debugState = false, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(p_debugState),
    m_statusSaved(false)
  {}


  template<class T>
  DebugBinOArchive& operator*(T& p_value)
  {
    if (false == m_statusSaved)
    {
      //std::cout << "saving debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusSaved = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugBinOArchive& operator/(T& p_value)
  {
    //std::cout << "should save debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "saving debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

private:
  bool m_debugMode;
  bool m_statusSaved;
};



class DebugBinIArchive : public boost::archive::binary_iarchive_impl<DebugBinIArchive, std::istream::char_type, std::istream::traits_type>
{
  typedef boost::archive::binary_iarchive_impl<DebugBinIArchive, std::istream::char_type, std::istream::traits_type> base;
  friend class boost::archive::detail::interface_iarchive<DebugBinIArchive>;
  friend class boost::archive::basic_binary_iarchive<DebugBinIArchive>;
  friend class boost::archive::save_access;

public:
  DebugBinIArchive(std::istream& p_os, int p_flags = 0) :
    base(p_os, p_flags),
    m_debugMode(false),
    m_statusLoaded(false)
  {}

  template<class T>
  DebugBinIArchive& operator*(T& p_value)
  {
    if (false == m_statusLoaded)
    {
      //std::cout << "loading debug status..." << std::endl;
      base::operator&(boost::serialization::make_nvp("debug", m_debugMode));
      m_statusLoaded = true;
    }
    return base::operator&(p_value);
  }

  template<class T>
  DebugBinIArchive& operator/(T& p_value)
  {
    //std::cout << "should load debug field ?" << std::endl;
    if (m_debugMode)
    {
      //std::cout << "loading debug field..." << std::endl;
      *this * p_value;
    }
    return *this;
  }

  bool getDebugState(void) const
  {
    return m_debugMode;
  }

private:
  bool m_debugMode;
  bool m_statusLoaded;
};

}}

BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugXmlOArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugXmlIArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugTextOArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugTextIArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugBinOArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(xtd::serializer::DebugBinIArchive)

#endif // !SERIALIZER_DEBUGARCHIVE_HH_
