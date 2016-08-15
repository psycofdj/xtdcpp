find_program(CPPCHECK_EXECUTABLE
  NAMES cppcheck
  DOC "cppcheck static code anylyser tool (http://cppcheck.sourceforge.net/)"
)

if (CPPCHECK_EXECUTABLE)
  SET(CPPCHECK_FOUND TRUE)
else()
  SET(CPPCHECK_FOUND FALSE)
  message(WARNING "unable to find cppcheck program, please install cppcheck (http://cppcheck.sourceforge.net/)")
endif()

if (CPPCHECK_FOUND)
  add_custom_target(cppcheck)
  add_custom_target(cppcheck-clean)

  function(add_cppcheck module)
    set(CMAKE_CPPCHECK_OUTPUT "${CMAKE_BINARY_DIR}/reports/${module}/cppcheck")
    file(GLOB_RECURSE files_cppcheck
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hh"
      "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hxx")
    add_custom_command(
      OUTPUT ${CMAKE_CPPCHECK_OUTPUT}/cppcheck.xml
      COMMAND mkdir -p ${CMAKE_CPPCHECK_OUTPUT}
      COMMAND ${CPPCHECK_EXECUTABLE} -q --xml ${CMAKE_CURRENT_SOURCE_DIR}/src 2> ${CMAKE_CPPCHECK_OUTPUT}/cppcheck.xml
      COMMAND sed -i "2i <?xml-stylesheet type='text/xsl' href='stylesheet.xsl'?>" ${CMAKE_CPPCHECK_OUTPUT}/cppcheck.xml
      COMMAND cp ${PROJECT_SOURCE_DIR}/xtdmake/cppcheck/stylesheet.xsl ${CMAKE_CPPCHECK_OUTPUT}/
      DEPENDS ${files_cppcheck}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${module} cppcheck report" VERBATIM)
    add_custom_target(cppcheck-${module}
      DEPENDS ${CMAKE_CPPCHECK_OUTPUT}/cppcheck.xml)
    add_custom_target(cppcheck-${module}-clean
      COMMAND rm -rf ${CMAKE_CPPCHECK_OUTPUT})
    add_dependencies(cppcheck       cppcheck-${module})
    add_dependencies(cppcheck-clean cppcheck-${module}-clean)
  endfunction()
endif()

mark_as_advanced(
  CPPCHECK_EXECUTABLE
  )
