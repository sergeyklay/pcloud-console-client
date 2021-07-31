# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Find the FUSE includes and library
#
#  FUSE_INCLUDE_DIR    - Where to find fuse.h, etc.
#  FUSE_LIBRARY        - List of libraries when using FUSE.
#  FUSE_FOUND          - True if FUSE lib is found.
#  FUSE_USE_VERSION    - FUSE version API.
#  FUSE_VERSION_STRING - FUSE version string.
#  FUSE_DEFINITIONS    - The compiler definitions, required for building with FUSE

if (MSVC)
  option(DOKAN_PATH "Location of the Dokan library, e.g. C:\\Program Files\\Dokan\\DokanLibrary-1.5.0" "")
endif()

set(fuse_message "Check for fuse")

if (WIN32)
  if("${DOKAN_PATH}" STREQUAL "")
    message(FATAL_ERROR
      "DOKAN_PATH is not set. Please set it to actual location of the "
      "Dokan library, e.g. C:\\Program Files\\Dokan\\DokanLibrary-1.5.0")
  endif()

  if ("${PCLOUD_TARGET_ARCH}" STREQUAL "x86_64")
    set(DOKAN_LIB_PATH "${DOKAN_PATH}")
  elseif("${PCLOUD_TARGET_ARCH}" STREQUAL "i386")
    set(DOKAN_LIB_PATH "${DOKAN_PATH}/x86")
  else ()
    message(FATAL_ERROR "Unsupported architecture: ${PCLOUD_TARGET_ARCH}")
  endif()

  set(FUSE_FOUND True)
  set(FUSE_INCLUDE_DIR "${DOKAN_PATH}/include")
  set(dokan_libs "${DOKAN_LIB_PATH}/lib/dokan1.lib"
                  "${DOKAN_LIB_PATH}/lib/dokanfuse1.lib")

  foreach(dokan_lib ${dokan_libs})
    if(NOT EXISTS ${dokan_lib})
      set(FUSE_FOUND False)
    endif()
  endforeach()

  if(FUSE_FOUND)
    set(FUSE_LIBRARY ${dokan_libs})
    message(STATUS "${fuse_message}: ${DOKAN_LIB_PATH}/lib/dokanfuse1.lib")
  else()
    message(SEND_ERROR "${fuse_message}: not found")
    set(FUSE_LIBRARY "" CACHE STRING "" FORCE) # delete it
  endif()

  install(
    FILES ${dokan_libs}
    DESTINATION ${CMAKE_INSTALL_BINDIR})
  unset(dokan_libs)
else()
  if(FUSE_INCLUDE_DIR)
    set(FUSE_FIND_QUIETLY TRUE)
  endif()

  if(APPLE)
    set(FUSE_NAMES libosxfuse.dylib fuse)
    set (FUSE_SUFFIXES osxfuse macfuse fuse)
  else()
    set(FUSE_NAMES fuse)
    set(FUSE_SUFFIXES fuse)
  endif()

  find_library(
    FUSE_LIBRARY
    NAMES ${FUSE_NAMES}
    PATHS /lib64
          /lib
          /usr/lib64
          /usr/lib
          /usr/local/lib64
          /usr/local/lib
          /usr/lib/x86_64-linux-gnu)


  if(FUSE_LIBRARY)
    message(STATUS "${fuse_message}: ${FUSE_LIBRARY}")
  else()
    message(SEND_ERROR "${fuse_message}: not found")
    set(FUSE_LIBRARY "" CACHE STRING "" FORCE) # delete it
  endif()

  find_path(
    FUSE_INCLUDE_DIR
    NAMES fuse.h
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES ${FUSE_SUFFIXES})
endif()

# check found version
if(FUSE_INCLUDE_DIR)
  # usually on Linux FUSE_INCLUDE_DIR is /usr/include
  if(FUSE_INCLUDE_DIR MATCHES "/include$" )
    set(FUSE_INCLUDE_DIR "${FUSE_INCLUDE_DIR}/fuse")
  endif()

  # retrieve version information from the header
  file(STRINGS "${FUSE_INCLUDE_DIR}/fuse_common.h" fuse_version_str
      REGEX "^#define[\t ]+FUSE.+VERSION[\t ]+[0-9]+")

  string(REGEX REPLACE ".*#define[\t ]+FUSE_MAJOR_VERSION[\t ]+([0-9]+).*" "\\1"
      fuse_version_major "${fuse_version_str}")
  string(REGEX REPLACE ".*#define[\t ]+FUSE_MINOR_VERSION[\t ]+([0-9]+).*" "\\1"
      fuse_version_minor "${fuse_version_str}")

  set(FUSE_USE_VERSION "${fuse_version_major}${fuse_version_minor}")
  set(FUSE_VERSION_STRING "${fuse_version_major}.${fuse_version_minor}")

  unset(fuse_version_str)
  unset(fuse_version_minor)
  unset(fuse_version_major)

  mark_as_advanced(FUSE_USE_VERSION FUSE_VERSION_STRING)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("FUSE" DEFAULT_MSG
    FUSE_INCLUDE_DIR FUSE_LIBRARY)

# add definitions
if(FUSE_FOUND)
  string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" sys_cpu)
  if(sys_cpu MATCHES ia64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  elseif(sys_cpu MATCHES amd64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  elseif(sys_cpu MATCHES x86_64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  else()
    message(WARNING "Unknown system processor: ${sys_cpu}")
  endif()
  unset(sys_cpu)
endif()

mark_as_advanced(FUSE_FIND_QUIETLY FUSE_NAMES FUSE_LIBRARY)
mark_as_advanced(FUSE_DEFINITIONS FUSE_INCLUDE_DIR)
unset(fuse_message)
