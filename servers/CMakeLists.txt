# htmlimgdir			= $(prefix)/share/xtd/www/img
# htmlcssdir			= $(prefix)/share/xtd/www/css
# htmljsdir				= $(prefix)/share/xtd/www/js
# htmltpldir			= $(prefix)/share/xtd/www/tpl

# htmlimg_DATA    = $(shell find $(srcdir)/html/img -type f )
# htmlcss_DATA    = $(shell find $(srcdir)/html/css -type f )
# htmljs_DATA     = $(shell find $(srcdir)/html/js  -type f )
# htmltpl_DATA    = $(shell find $(srcdir)/html/tpl -type f )

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/common/src
  ${PROJECT_SOURCE_DIR}/counters/src
  ${PROJECT_SOURCE_DIR}/network/src
  ${PROJECT_SOURCE_DIR}/serializer/src
)

add_library(servers
  src/app/HtmlOArchive.cc
  src/app/HttpServer.cc
  src/app/Server.cc
  src/param/Handler.cc
  src/param/Base.cc
  src/param/Visitor.cc
)
