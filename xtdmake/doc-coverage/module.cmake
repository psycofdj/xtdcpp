find_package(Doxygen)

find_program(LCOV_EXECUTABLE
  NAMES genhtml
  DOC "Lcov html report generation tool (http://ltp.sourceforge.net/coverage/lcov.php)"
)

find_program(COVERXYGEN_EXECUTABLE
  NAMES coverxygen.py
  DOC "Tool to generate coverage report from Doxygen documentation (https://github.com/psycofdj/coverxygen)"
  )

SET(DOC_COVERAGE_FOUND FALSE)
if (LCOV_EXECUTABLE AND COVERXYGEN_EXECUTABLE AND DOXYGEN_FOUND)
  SET(DOC_COVERAGE_FOUND TRUE)
else()
  if (NOT LCOV_EXECUTABLE)
    message(WARNING "unable to find genhtml program, please install lcov (http://ltp.sourceforge.net/coverage/lcov.php)")
  endif()
  if (NOT COVERXYGEN_EXECUTABLE)
    message(WARNING "unable to find coverxygen.py program, please install goverxygen (https://github.com/psycofdj/coverxygen)")
  endif()
endif()




if (DOC_COVERAGE_FOUND)
  set(CMAKE_DOXYGEN_XML "YES")
  add_custom_target(doc-coverage)
  add_custom_target(doc-coverage-clean)

  function(add_doc_coverage module)
    if (NOT TARGET doc-${module})
      message(FATAL_ERROR "unable to find target doc-${module}, please call add_doc command for your module")
    endif()

    get_target_property(CMAKE_DOXYGEN_OUTPUT doc-${module} OUTPUT_DIR)
    set(CMAKE_DOCCOVERAGE_OUTPUT "${CMAKE_BINARY_DIR}/reports/${module}/doc-coverage")

    add_custom_command(
      OUTPUT ${CMAKE_DOCCOVERAGE_OUTPUT}/doc-coverage.info
      COMMAND mkdir -p ${CMAKE_DOCCOVERAGE_OUTPUT}
      COMMAND ${COVERXYGEN_EXECUTABLE} --xml-dir ${CMAKE_DOXYGEN_OUTPUT}/xml/ --output ${CMAKE_DOCCOVERAGE_OUTPUT}/doc-coverage.info --prefix ${CMAKE_CURRENT_SOURCE_DIR}/src
      DEPENDS ${CMAKE_DOXYGEN_OUTPUT}/xml/index.xml
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${module} documentation coverage informations" VERBATIM)

    add_custom_command(
      OUTPUT ${CMAKE_DOCCOVERAGE_OUTPUT}/index.html
      COMMAND mkdir -p ${CMAKE_DOCCOVERAGE_OUTPUT}
      COMMAND ${LCOV_EXECUTABLE} --no-function-coverage --no-branch-coverage ${CMAKE_DOCCOVERAGE_OUTPUT}/doc-coverage.info -o ${CMAKE_DOCCOVERAGE_OUTPUT}/
      DEPENDS ${CMAKE_DOCCOVERAGE_OUTPUT}/doc-coverage.info
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${module} documentation coverage result interface" VERBATIM)

    add_custom_target(doc-coverage-${module}
      DEPENDS ${CMAKE_DOCCOVERAGE_OUTPUT}/index.html)
    set_target_properties(doc-coverage-${module}
      PROPERTIES OUTPUT_DIR "${CMAKE_DOCCOVERAGE_OUTPUT}")
    add_custom_target(doc-coverage-${module}-clean
      COMMAND rm -rf ${CMAKE_DOCCOVERAGE_OUTPUT})

    add_dependencies(doc-coverage       doc-coverage-${module})
    add_dependencies(doc-coverage-clean doc-coverage-${module}-clean)
  endfunction()
endif()



mark_as_advanced(
  COVERXYGEN_EXECUTABLE
  LCOV_EXECUTABLE
  )
