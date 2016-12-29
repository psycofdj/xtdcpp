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
  ${core_INCLUDE_DIRS}
  ${serializer_INCLUDE_DIRS}
  ${network_INCLUDE_DIRS}
  ${counters_INCLUDE_DIRS}
  ${servers_INCLUDE_DIRS}
  )

add_shared_static_library(servers
  src/app/HtmlOArchive.cc
  src/app/HttpServer.cc
  src/app/Server.cc
  src/param/Handler.cc
  src/param/Base.cc
  src/param/Visitor.cc
)


add_doc(servers WERROR)
add_doc_coverage(servers)
add_cloc(servers)
add_cppcheck(servers)
add_check(servers
  ARGS     -p -o compiler
  INCLUDES ${core_INCLUDE_DIRS} ${servers_INCLUDE_DIRS} ${counters_INCLUDE_DIRS} ${network_INCLUDE_DIRS} ${tests_INCLUDE_DIRS}
  LINKS    core_s tests_s counters_s servers_s network_s ${Boost_LIBRARIES})
add_cov(servers)
add_memcheck(servers)
