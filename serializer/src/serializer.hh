#ifndef SERIALIZER_SERIALIZER_HH_
# define SERIALIZER_SERIALIZER_HH_

# include <iostream>
# include <types.hh> // libcore
# include <boost/archive/basic_archive.hpp>
# include "serializer_fwd.hh"  // IWYU pragma: export
# include "archives/DebugArchive.hh"


namespace xtd {
namespace serializer {

#define ENUM_CASE(p_value, p_dst)               \
  case p_value :                                \
  p_dst = #p_value;                             \
  break;

struct mode
{
  struct text
  {
    typedef DebugTextOArchive saver;
    typedef DebugTextIArchive loader;
  };
  struct xml
  {
    typedef DebugXmlOArchive saver;
    typedef DebugXmlIArchive loader;
  };
  struct bin
  {
    typedef DebugBinOArchive saver;
    typedef DebugBinIArchive loader;
  };
};

struct option
{
  option(void) :
    m_noHeader(false),
    m_noCodecvt(false),
    m_noXmlTagChecking(false)
  {
  }

  option(bool p_noHeader, bool p_noCodecvt, bool p_noXmlTagCheking) :
    m_noHeader(p_noHeader),
    m_noCodecvt(p_noCodecvt),
    m_noXmlTagChecking(p_noXmlTagCheking)
  {
  }

  int value(void) const
  {
    int l_result = 0;

    if (m_noHeader)         l_result |= boost::archive::no_header;
    if (m_noCodecvt)        l_result |= boost::archive::no_codecvt;
    if (m_noXmlTagChecking) l_result |= boost::archive::no_xml_tag_checking;
    return l_result;
  }

private:
  bool m_noHeader;
  bool m_noCodecvt;
  bool m_noXmlTagChecking;
};


template<class Mode>
struct serializer
{
  template<class T>
  static void
  save(std::ostream& p_stream, const T& p_object, bool p_debug, option p_opt = option());

  template<class T>
  static void
  load(std::istream& p_stream, T& p_object, bool& p_debug, option p_opt = option());
};



template<class T>
string            as_xml(const T& p_obj, bool p_debug, option p_opt = option());
template<class T>
std::vector<char> as_bin(const T& p_obj, bool p_debug, option p_opt = option());
template<class T>
void              as_bin(const T& p_obj, std::vector<char>& p_data, bool p_debug, option p_opt = option());
template<class T>
std::vector<char> as_text(const T& p_obj, bool p_debug, option p_opt = option());

template<class T>
void from_bin(const std::vector<char>& p_data, T& p_obj, option p_opt = option());
template<class T>
void from_bin(const std::vector<char>& p_data, T& p_obj, bool& p_debug, option p_opt = option());
template<class T>
void from_text(const std::vector<char>& p_data, T& p_obj, bool& p_debug, option p_opt = option());

}} // end of namespace serializer

# include "serializer.hxx"  // IWYU pragma: export

#endif // !SERIALIZER_SERIALIZER_HH_
