include_directories(
  ${Boost_INCLUDE_DIRS}
  ${common_INCLUDE_DIRS}
  ${serializer_INCLUDE_DIRS}
  ${network_INCLUDE_DIRS}
)

add_shared_static_library(network
  src/utils/Config.cc
  src/utils/Utils.cc
  src/utils/Resolver.cc
  src/utils/CacheDns.cc
  src/base/Connection.cc
  src/base/Server.cc
  src/base/Client.cc
  src/base/ClientThreadManager.cc
  src/bip/Server.cc
  src/bip/Client.cc
  src/bip/Connection.cc
  src/http/Server.cc
  src/http/Connection.cc
  src/http/Request.cc
  src/http/Response.cc
  src/http/Template.cc
  src/http/Server.cc
  src/http/cpptempl.cc
)



add_doc(network WERROR)
add_doc_coverage(network)
add_cloc(network)
add_cppcheck(network)
