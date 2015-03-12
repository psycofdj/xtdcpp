include $(XTDMAKE_DIR)/make/include.mk

noinst_LIBRARIES = libserializer.a

libserializer_a_CXXFLAGS =     \
  -I$(srcdir)/src              \
  -I$(XTD_INCLUDE)/common/src  \
  -I$(BOOST_INCLUDE)

libserializer_a_SOURCES =   \
  src/objects/Doc.cc        \
  src/serializer.cc
