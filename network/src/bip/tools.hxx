#ifndef NETWORK_BIP_TOOLS_HXX_
# define NETWORK_BIP_TOOLS_HXX_

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
# include <boost/iostreams/filtering_stream.hpp>
# include <boost/iostreams/filter/zlib.hpp>
# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filter/bzip2.hpp>
# include "utils/Config.hh"
# include <serializer.hh> // libserializer

namespace xtd {
namespace network {
namespace bip {


template<typename Mode, typename T>
status loadCompress(const utils::Config&        p_conf,
                    const utils::vectorBytes_t& p_data,
                    T&                          p_obj,
                    bool&                       p_debug)
{
  boost::iostreams::filtering_istream l_fis;

  if (p_conf.getCompress())
  {
    switch (p_conf.getCompressionCodec())
    {
    case utils::codec::zlib:
      l_fis.push(boost::iostreams::zlib_decompressor());
      break;
    case utils::codec::gzip:
      l_fis.push(boost::iostreams::gzip_decompressor());
      break;
    case utils::codec::bzip2:
      l_fis.push(boost::iostreams::bzip2_decompressor());
      break;
    }
  }

  l_fis.push(boost::make_iterator_range(p_data));

  try {
    serializer::serializer<Mode>::load(l_fis, p_obj, p_debug);
  }
  catch (boost::archive::archive_exception& l_error)
  {
    logger::crit("network.bip.tools", "Error while unserializing request, %s", l_error.what());
    return status::error;
  }
  catch (std::exception l_e)
  {
    logger::crit("network.bip.tools", "Error while unserializing request, %s", l_e.what());
    return status::error;
  }

  return status::ok;
}




template<typename Mode, typename T>
status saveCompress(const utils::Config&  p_conf,
                    const T               p_obj,
                    const bool            p_debug,
                    utils::vectorBytes_t& p_data)
{
  boost::iostreams::filtering_ostream l_fos;

  if (p_conf.getCompress())
  {
    switch (p_conf.getCompressionCodec())
    {
    case utils::codec::zlib:
      l_fos.push(boost::iostreams::zlib_compressor());
      break;
    case utils::codec::gzip:
      l_fos.push(boost::iostreams::gzip_compressor());
      break;
    case utils::codec::bzip2:
      l_fos.push(boost::iostreams::bzip2_compressor());
      break;
    }
  }

  l_fos.push(boost::iostreams::back_inserter(p_data));

  try {
    serializer::serializer<Mode>::save(l_fos, p_obj, p_debug);
  }
  catch (boost::archive::archive_exception& l_error)
  {
    logger::crit("network.bip.tools", "Error while serializing request : %s", l_error.what());
    return status::error;
  }


  return status::ok;
}

}}}

#endif // !NETWORK_BIP_TOOLS_HXX_
