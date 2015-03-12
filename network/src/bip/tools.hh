#ifndef NETWORK_BIP_TOOLS_HH_
# define NETWORK_BIP_TOOLS_HH_

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include "utils/Config.hh"

namespace xtd {
namespace network {

namespace bip {


template<typename Mode, typename T>
status loadCompress(const utils::Config&        p_conf,
                    const utils::vectorBytes_t& p_data,
                    T&                          p_obj,
                    bool&                       p_debug);


template<typename Mode, typename T>
status saveCompress(const utils::Config&  p_conf,
                    const T               p_obj,
                    const bool            p_debug,
                    utils::vectorBytes_t& p_data);


}}}

# include "bip/tools.hxx"

#endif // !NETWORK_BIP_TOOLS_HH_
