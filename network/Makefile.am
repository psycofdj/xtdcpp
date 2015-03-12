include $(XTDMAKE_DIR)/make/include.mk

noinst_LIBRARIES  = libnetwork.a

AM_CXXFLAGS =                      \
  -std=c++11                       \
  -I$(XTD_INCLUDE)/                \
  -I$(XTD_INCLUDE)/common/src      \
  -I$(XTD_INCLUDE)/network/src     \
  -I$(XTD_INCLUDE)/serializer/src  \
  -I$(BOOST_INCLUDE)

libnetwork_a_SOURCES =                  \
  ./src/utils/Config.cc                 \
  ./src/utils/Utils.cc                  \
  ./src/utils/Resolver.cc               \
  ./src/utils/CacheDns.cc               \
  ./src/base/Connection.cc              \
  ./src/base/Server.cc                  \
  ./src/base/Client.cc                  \
  ./src/base/ClientThreadManager.cc     \
  ./src/bip/Server.cc                   \
  ./src/bip/Client.cc                   \
  ./src/bip/Connection.cc               \
  ./src/http/Server.cc                  \
  ./src/http/Connection.cc              \
  ./src/http/Request.cc                 \
  ./src/http/Response.cc                \
  ./src/http/Template.cc                \
  ./src/http/Server.cc                  \
  ./src/http/cpptempl.cc
