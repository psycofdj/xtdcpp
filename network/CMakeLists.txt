include_directories(
  ${Boost_INCLUDE_DIRS}
  ${core_INCLUDE_DIRS}
  ${serializer_INCLUDE_DIRS}
  ${network_INCLUDE_DIRS}
)

add_shared_static_library(network
  src/utils/Utils.cc
  src/utils/Resolver.cc
  src/base/Config.cc
  src/base/Connection.cc
  src/base/Server.cc
  src/base/Client.cc
  src/base/ThreadManager.cc
  src/bip/ClientPool.cc
  src/bip/Server.cc
  src/bip/Client.cc
  src/bip/Connection.cc
  src/http/Server.cc
  src/http/Connection.cc
  src/http/Message.cc
  src/http/Request.cc
  src/http/Response.cc
  src/http/Template.cc
  src/http/Server.cc
  src/http/Client.cc
  src/http/cpptempl.cc
  src/http/http_types.cc
  src/http/codes.cc
  src/http/ClientPool.cc
  )



add_doc(network WERROR)
add_doc_coverage(network)
add_cloc(network)
add_codedup(network)
add_cppcheck(network)
add_check(network
  ARGS     -p -o compiler
  INCLUDES ${core_INCLUDE_DIRS} ${network_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS      network_s tests_s core_s ${Boost_LIBRARIES})
add_cov(network)
add_memcheck(network)
