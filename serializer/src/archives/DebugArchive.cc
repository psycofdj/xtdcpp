#include "archives/DebugArchive.hh"

#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_xml_oarchive.ipp>
#include <boost/archive/impl/xml_oarchive_impl.ipp>
#include <boost/archive/impl/basic_xml_iarchive.ipp>
#include <boost/archive/impl/xml_iarchive_impl.ipp>
#include <boost/archive/impl/text_oarchive_impl.ipp>
#include <boost/archive/impl/text_iarchive_impl.ipp>
#include <boost/archive/impl/basic_binary_oprimitive.ipp>
#include <boost/archive/impl/basic_text_oarchive.ipp>
#include <boost/archive/impl/basic_binary_oarchive.ipp>
#include <boost/archive/impl/basic_binary_iprimitive.ipp>
#include <boost/archive/impl/basic_text_iarchive.ipp>
#include <boost/archive/impl/basic_binary_iarchive.ipp>

namespace boost {
namespace archive {

template class detail::archive_serializer_map<serializer::DebugXmlOArchive>;
template class             basic_xml_oarchive<serializer::DebugXmlOArchive>;
template class              xml_oarchive_impl<serializer::DebugXmlOArchive>;
template class detail::archive_serializer_map<serializer::DebugXmlIArchive>;
template class             basic_xml_iarchive<serializer::DebugXmlIArchive>;
template class              xml_iarchive_impl<serializer::DebugXmlIArchive>;

template class detail::archive_serializer_map<serializer::DebugTextOArchive>;
template class            basic_text_oarchive<serializer::DebugTextOArchive>;
template class             text_oarchive_impl<serializer::DebugTextOArchive>;
template class detail::archive_serializer_map<serializer::DebugTextIArchive>;
template class            basic_text_iarchive<serializer::DebugTextIArchive>;
template class             text_iarchive_impl<serializer::DebugTextIArchive>;

template class detail::archive_serializer_map<serializer::DebugBinOArchive>;
template class          basic_binary_oarchive<serializer::DebugBinOArchive>;
template class           binary_oarchive_impl<serializer::DebugBinOArchive, std::ostream::char_type, std::ostream::traits_type>;
template class        basic_binary_oprimitive<serializer::DebugBinOArchive, std::ostream::char_type, std::ostream::traits_type>;
template class detail::archive_serializer_map<serializer::DebugBinIArchive>;
template class          basic_binary_iarchive<serializer::DebugBinIArchive>;
template class           binary_iarchive_impl<serializer::DebugBinIArchive, std::istream::char_type, std::istream::traits_type>;
template class        basic_binary_iprimitive<serializer::DebugBinIArchive, std::istream::char_type, std::istream::traits_type>;

}}
