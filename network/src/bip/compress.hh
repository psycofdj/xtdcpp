#ifndef NETWORK_BIP_TOOLS_HH_
# define NETWORK_BIP_TOOLS_HH_

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include "base/Config.hh"

namespace xtd {
namespace network {
namespace bip {


template<typename Mode, typename T>
status load(compress_codec      p_conf,
            const vector<char>& p_data,
            T&                  p_obj,
            bool&               p_debug);

template<typename Mode, typename T>
status save(compress_codec p_conf,
            const T        p_obj,
            const bool     p_debug,
            vector<char>&  p_data);


}}}

# include "bip/compress.hxx"

#endif // !NETWORK_BIP_TOOLS_HH_
