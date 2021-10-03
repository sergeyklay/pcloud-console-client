# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

cmake_minimum_required(VERSION 3.0)

project(log_c
  VERSION 0.1.0
  LANGUAGES C)

add_library(log_c SHARED src/log.c)
add_library(rxi::log_c ALIAS log_c)

target_include_directories(log_c
  PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

file(RELATIVE_PATH relative
  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  ${CMAKE_INSTALL_PREFIX})

if(APPLE)
  set(rpath "@loader_path/${relative}")
else()
  set(rpath "\$ORIGIN/${relative}")
endif()

file(TO_NATIVE_PATH
  "${rpath}/${CMAKE_INSTALL_LIBDIR}"
  message_RPATH)

set_target_properties(log_c PROPERTIES
  VERSION ${PROJECT_VERSION}
  MACOSX_RPATH ON
  # Don't skip the full RPATH for the build tree.
  SKIP_BUILD_RPATH OFF
  # When building, don't use the install RPATH already.
  BUILD_WITH_INSTALL_RPATH OFF
  # The RPATH to be used when installing.
  INSTALL_RPATH "${message_RPATH}"
  # Tells CMake to append linker search paths to
  # the RPATH executable.
  INSTALL_RPATH_USE_LINK_PATH ON)

install(TARGETS log_c
  LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
