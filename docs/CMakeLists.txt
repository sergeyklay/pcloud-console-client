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

  # copy the files needed by the documentation
  configure_file(${DOXYGEN_INPUT_DIR}/docs/doxygen.css
    ${DOXYGEN_OUTPUT_DIR}/html/doxygen.css COPYONLY)

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
    COMMAND ;
    COMMENT "Open ${doc_path} in your browser to view the API Documentation.")
  unset(doc_path)

  # setup install rules
  install(
    FILES "${DOXYGEN_OUTPUT_DIR}/pcloudcc.tag"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT doc)

  install(
    DIRECTORY "${DOXYGEN_OUTPUT_DIR}/html"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT doc)
endif()
