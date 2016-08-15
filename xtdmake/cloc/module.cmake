find_program(CLOC_EXECUTABLE
  NAMES cloc
  DOC "cloc code line counting tool (http://cloc.sourceforge.net/)"
)

if (CLOC_EXECUTABLE)
  SET(CLOC_FOUND TRUE)
else()
  SET(CLOC_FOUND FALSE)
  message(WARNING "unable to find cloc program, please install cloc (http://cloc.sourceforge.net/)")
endif()

if (CLOC_FOUND)
  add_custom_target(cloc)
  add_custom_target(cloc-clean)
  function(add_cloc module)
    set(CMAKE_CLOC_OUTPUT "${CMAKE_BINARY_DIR}/reports/${module}/cloc")
    file(GLOB_RECURSE files_cloc
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hh"
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hxx")
    add_custom_command(
      OUTPUT ${CMAKE_CLOC_OUTPUT}/cloc.xml
      COMMAND mkdir -p ${CMAKE_CLOC_OUTPUT}
      COMMAND ${CLOC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src --xml --xsl=stylesheet.xsl --out ${CMAKE_CLOC_OUTPUT}/cloc.xml --by-file-by-lang
      COMMAND cp ${PROJECT_SOURCE_DIR}/xtdmake/cloc/stylesheet.xsl ${CMAKE_CLOC_OUTPUT}/
      DEPENDS ${files_cloc}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${module} cloc report" VERBATIM)
    add_custom_target(cloc-${module}
      DEPENDS ${CMAKE_CLOC_OUTPUT}/cloc.xml)
    add_custom_target(cloc-${module}-clean
      COMMAND rm -rf ${CMAKE_CLOC_OUTPUT})
    add_dependencies(cloc       cloc-${module})
    add_dependencies(cloc-clean cloc-${module}-clean)
  endfunction()
endif()

mark_as_advanced(
  CLOC_EXECUTABLE
  )
