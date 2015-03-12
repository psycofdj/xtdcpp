include $(XTDMAKE_DIR)/make/include.mk

htmlimgdir			= $(prefix)/share/xtd/www/img
htmlcssdir			= $(prefix)/share/xtd/www/css
htmljsdir				= $(prefix)/share/xtd/www/js
htmltpldir			= $(prefix)/share/xtd/www/tpl

htmlimg_DATA    = $(shell find $(srcdir)/html/img -type f )
htmlcss_DATA    = $(shell find $(srcdir)/html/css -type f )
htmljs_DATA     = $(shell find $(srcdir)/html/js  -type f )
htmltpl_DATA    = $(shell find $(srcdir)/html/tpl -type f )

noinst_LIBRARIES      = libservers.a

AM_CXXFLAGS =                  \
  -std=c++11                   \
  -I$(top_srcdir)/             \
  -I$(top_srcdir)/common/src   \
  -I$(top_srcdir)/counters/src \
  -I$(top_srcdir)/network/src  \
  -I$(top_srcdir)/serializer/src  \
  -I$(srcdir)/src              \
  -I$(BOOST_INCLUDE)

libservers_a_SOURCES =       \
  src/app/HtmlOArchive.cc    \
  src/app/HttpServer.cc      \
  src/app/Server.cc          \
  src/param/Handler.cc       \
  src/param/Base.cc          \
  src/param/Visitor.cc

