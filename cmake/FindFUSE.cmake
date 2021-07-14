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

if(FUSE_INCLUDE_DIR)
  set(FUSE_FIND_QUIETLY TRUE)
endif()

if(APPLE)
  set(FUSE_NAMES libosxfuse.dylib fuse)
  set (FUSE_SUFFIXES osxfuse macfuse fuse)
elseif(WINDOWS)
  set (FUSE_NAMES libdokanfuse1)
  set (FUSE_SUFFIXES dokanfuse1)
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
          /usr/lib/x86_64-linux-gnu
)

set(_fuse_message "Check for fuse")
if(FUSE_LIBRARY)
  message(STATUS "${_fuse_message}: ${FUSE_LIBRARY}")
else()
  message(SEND_ERROR "${_fuse_message}: not found")
  set(FUSE_LIBRARY "" CACHE STRING "" FORCE) # delete it
endif()

find_path(
    FUSE_INCLUDE_DIR
    NAMES fuse.h
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES ${FUSE_SUFFIXES})

# check found version
if(FUSE_INCLUDE_DIR)
  # usually on Linux FUSE_INCLUDE_DIR is /usr/include
  if (FUSE_INCLUDE_DIR MATCHES "/include$" AND NOT WINDOWS)
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
  if(CMAKE_SYSTEM_PROCESSOR MATCHES ia64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES amd64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES x86_64)
    set(FUSE_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
  endif()
endif()

mark_as_advanced(FUSE_FIND_QUIETLY FUSE_NAMES FUSE_LIBRARY)
mark_as_advanced(FUSE_DEFINITIONS FUSE_INCLUDE_DIR FUSE_COMMON_INCLUDE_DIR)
