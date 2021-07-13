# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
#
# This source file is subject to the New BSD License that is bundled with this
# project in the file LICENSE.
#
# If you did not receive a copy of the license and are unable to obtain it
# through the world-wide-web, please send an email to egrep@protonmail.ch so
# we can send you a copy immediately.

# Find the FUSE includes and library
#
#  FUSE_INCLUDE_DIR    - Where to find fuse.h, etc.
#  FUSE_LIBRARY        - List of libraries when using FUSE.
#  FUSE_FOUND          - True if FUSE lib is found.
#  FUSE_USE_VERSION    - FUSE version API.
#  FUSE_VERSION_STRING - FUSE version string.

# TODO: Provide ability to specify FUSE library path as well as includes
# TODO: Provide ability to specify FUSE version eg FUSE_API=[2,3]

if(FUSE_INCLUDE_DIR)
  set(FUSE_FIND_QUIETLY TRUE)
endif()

if(APPLE)
  set(FUSE_NAMES libosxfuse.dylib fuse)
else()
  # TODO: Is this will work on Cygwin?
  set(FUSE_NAMES fuse)
endif()

find_library(
    FUSE_LIBRARY
    NAMES ${FUSE_NAMES}
    PATHS /lib64 /lib /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu
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
    PATHS /usr/local/include/osxfuse /usr/local/include/fuse /usr/include/fuse)

if(FUSE_INCLUDE_DIR)
  file(STRINGS "${FUSE_INCLUDE_DIR}/fuse_common.h" fuse_version_str
      REGEX "^#define[\t ]+FUSE.+VERSION[\t ]+[0-9]+")

  string(REGEX REPLACE ".*#define[\t ]+FUSE_MAJOR_VERSION[\t ]+([0-9]+).*" "\\1"
      fuse_version_major "${fuse_version_str}")
  string(REGEX REPLACE ".*#define[\t ]+FUSE_MINOR_VERSION[\t ]+([0-9]+).*" "\\1"
      fuse_version_minor "${fuse_version_str}")
  if (fuse_version_minor)
    string(SUBSTRING "${fuse_version_minor}" 0 1 fuse_version_minor_1)
  endif()

  set(FUSE_USE_VERSION "${fuse_version_major}${fuse_version_minor_1}")
  set(FUSE_VERSION_STRING "${fuse_version_major}.${fuse_version_minor}")

  unset(fuse_version_str)
  unset(fuse_version_minor)
  unset(fuse_version_major)

  mark_as_advanced(FUSE_USE_VERSION FUSE_VERSION_STRING)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("FUSE" DEFAULT_MSG
    FUSE_INCLUDE_DIR FUSE_LIBRARY)

mark_as_advanced(FUSE_FIND_QUIETLY FUSE_NAMES FUSE_LIBRARY FUSE_INCLUDE_DIR)
