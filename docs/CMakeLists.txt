# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

find_package(Doxygen)

option(PCLOUD_BUILD_DOC
  "ON to generate the API documentation, OFF to ignore it"
  ${DOXYGEN_FOUND})

if(PCLOUD_BUILD_DOC)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  # set the input and output documentation paths
  set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
  set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs)

  # configure the source Doxyfile by copying it and replacing all @variables@
  set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}/Doxyfile)
  configure_file(${DOXYGEN_INPUT_DIR}/docs/Doxyfile.in
    ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

  # target setup
  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
    WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR}
    COMMENT "Building API Documentation...")

  set(doc_path "file://${DOXYGEN_OUTPUT_DIR}/html/index.html")
  add_custom_command(
    TARGET doc
    POST_BUILD
    # copy the files needed by the documentation only if "doc" target was called
    COMMAND "${CMAKE_COMMAND}" -E copy
      "${DOXYGEN_INPUT_DIR}/docs/doxygen.css"
      "${DOXYGEN_OUTPUT_DIR}/html/doxygen.css"
    COMMENT "Open ${doc_path} in your browser to view the API Documentation.")
  unset(doc_path)

  # setup install rules
  if(${CMAKE_VERSION} VERSION_LESS "3.14.0")
    install(
      FILES ${DOXYGEN_OUTPUT_DIR}/pcloudcc.tag
      DESTINATION ${CMAKE_INSTALL_DOCDIR}
      COMPONENT doc
      OPTIONAL)

    install(
      DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html
      DESTINATION ${CMAKE_INSTALL_DOCDIR}
      OPTIONAL
      COMPONENT doc)
  else()
    install(
      FILES ${DOXYGEN_OUTPUT_DIR}/pcloudcc.tag
      TYPE DOC
      COMPONENT doc
      OPTIONAL)

    install(
      DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html
      TYPE DOC
      OPTIONAL
      COMPONENT doc)
  endif()
endif()
