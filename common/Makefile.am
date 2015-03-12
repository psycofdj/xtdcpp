include $(XTDMAKE_DIR)/make/include.mk

noinst_PROGRAMS  = test
noinst_LIBRARIES = \
  libcommon.a

AM_CPPFLAGS =           \
  -std=c++11            \
  -I$(XTD_INCLUDE)   \
  -I$(BOOST_INCLUDE)

test_SOURCES = src/main.cc
test_LDADD   = libcommon.a

libcommon_a_SOURCES =   \
  src/logger.cc         \
  src/text.cc           \
  src/Application.cc    \
  src/ConfParser.cc

libcommon_a_CFLAGS   = $(THREAD_CFLAGS)
libcommon_a_CXXFLAGS = $(THREAD_CXXFLAGS)
