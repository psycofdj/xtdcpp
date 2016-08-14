find_package(Doxygen)

if (DOXYGEN_FOUND)
  function(add_doxygen module)
    set(multiValueArgs  EXCLUDE FILE_PATTERNS CALL_GRAPHS)
    set(oneValueArgs    EXAMPLE)
    set(options         WERROR)
    cmake_parse_arguments(CMAKE_DOXYGEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(CMAKE_DOXYGEN_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
    set(CMAKE_DOXYGEN_INPUT    ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(CMAKE_DOXYGEN_MODULE   "${module}")
    set(CMAKE_DOXYGEN_OUTPUT   ${CMAKE_BINARY_DIR}/reports/${module}/doc)

    string(REPLACE  ";"    " "                                CMAKE_DOXYGEN_EXCLUDE "${CMAKE_DOXYGEN_EXCLUDE}")
    string(REPLACE  "src/" "${CMAKE_CURRENT_SOURCE_DIR}/src/" CMAKE_DOXYGEN_EXCLUDE "${CMAKE_DOXYGEN_EXCLUDE}")

    if ("${CMAKE_DOXYGEN_FILE_PATTERNS}" STREQUAL "")
      set(CMAKE_DOXYGEN_FILE_PATTERNS "*.cc *.hh *.hpp")
    endif()

    if ("${CMAKE_DOXYGEN_CALL_GRAPHS}" STREQUAL "")
      set(CMAKE_DOXYGEN_CALL_GRAPHS "YES")
    endif()

    if ("${CMAKE_DOXYGEN_WERROR}" STREQUAL "TRUE")
      set(CMAKE_DOXYGEN_WERROR "YES")
    else()
      set(CMAKE_DOXYGEN_WERROR "NO")
    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/doc)
      set(CMAKE_DOXYGEN_INPUT "${CMAKE_DOXYGEN_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/doc")
      file(GLOB_RECURSE files_doc
        "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.hxx")

    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/doc/example)
      set(CMAKE_DOXYGEN_EXAMPLE ${CMAKE_CURRENT_SOURCE_DIR}/doc/example)
    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/doc/image)
      set(CMAKE_DOXYGEN_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/doc/image)
    endif()

    file(GLOB_RECURSE files
      "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh"
      "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx")


    configure_file(${PROJECT_SOURCE_DIR}/xtdmake/doc/doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg @ONLY)
    add_custom_command(
      OUTPUT ${CMAKE_DOXYGEN_OUTPUT}/html/index.html
      COMMAND mkdir -p ${CMAKE_DOXYGEN_OUTPUT}
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
      DEPENDS ${files} ${files_doc}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${module} API documentation with doxygen" VERBATIM)
    add_custom_target(doc
      DEPENDS ${CMAKE_DOXYGEN_OUTPUT}/html/index.html
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endfunction(add_doxygen)
endif(DOXYGEN_FOUND)
