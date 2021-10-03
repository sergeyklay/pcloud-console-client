# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Find the MbedTLS includes and library
#
# Once done this will define:
#
#  MBEDTLS_FOUND       - True if MbedTLS is found.
#  MBEDTLS_INCLUDE_DIR - The MbedTLS include directory.
#  MBEDTLS_LIBRARIES   - Link these to use MbedTLS.
#  MBEDTLS_VERSION     - MbedTLS version string (e.g. ``2.8.0``).
#
# Hints:
#
# Set ``MBEDTLS_ROOT_DIR`` to the root directory of MbedTLS installation.

include(CheckSymbolExists)

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(_MBEDTLS QUIET mbedtls)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_lib_suffix 64)
else()
  set(_lib_suffix 32)
endif()

set(_MBEDTLS_ROOT_HINTS
  ${MBEDTLS_ROOT_DIR${_lib_suffix}}
  ${MBEDTLS_ROOT_DIR}
  ENV MBEDTLS_ROOT_DIR${_lib_suffix}
  ENV MBEDTLS_ROOT_DIR)

find_path(MBEDTLS_INCLUDE_DIR
  NAMES mbedtls/ssl.h
  HINTS ${_MBEDTLS_ROOT_HINTS}
  PATHS /usr
        /usr/local
        /opt/local
        /usr/local/opt
  PATH_SUFFIXES include)

find_library(MBEDTLS_LIB
  NAMES mbedtls
  HINTS ${_MBEDTLS_ROOT_HINTS}
  PATHS /usr
        /usr/local
        /usr/local/opt
        /opt/local
        /opt/homebrew
  PATH_SUFFIXES lib${_lib_suffix} lib)

if (${MBEDTLS_LIB-NOTFOUND})
  message(FATAL_ERROR "Failed to find MbedTLS library")
endif()

find_library(MBEDCRYPTO_LIB
  NAMES mbedcrypto
  HINTS ${_MBEDTLS_ROOT_HINTS}
  PATHS /usr
        /usr/local
        /usr/local/opt
        /opt/local
        /opt/homebrew
  PATH_SUFFIXES lib${_lib_suffix} lib)

find_library(MBEDX509_LIB
  NAMES mbedx509
  HINTS ${_MBEDTLS_ROOT_HINTS}
  PATHS /usr
        /usr/local
        /usr/local/opt
        /opt/local
        /opt/homebrew
  PATH_SUFFIXES lib${_lib_suffix} lib)

# Sometimes mbedtls is split between three libs, and sometimes it isn't.
# If it isn't, let's check if the symbols we need are all in MBEDTLS_LIB.
if(MBEDTLS_LIB AND NOT MBEDCRYPTO_LIB AND NOT MBEDX509_LIB)
  set(CMAKE_REQUIRED_LIBRARIES ${MBEDTLS_LIB})
  set(CMAKE_REQUIRED_INCLUDES ${MBEDTLS_INCLUDE_DIR})
  check_symbol_exists(mbedtls_x509_crt_init "mbedtls/x509_crt.h" MBEDTLS_INCLUDES_X509)
  check_symbol_exists(mbedtls_sha256_init "mbedtls/sha256.h" MBEDTLS_INCLUDES_CRYPTO)
  unset(CMAKE_REQUIRED_INCLUDES)
  unset(CMAKE_REQUIRED_LIBRARIES)
endif()

# Extract the version from the header... hopefully it matches the library.
if (EXISTS ${MBEDTLS_INCLUDE_DIR}/mbedtls/build_info.h)
  file(STRINGS ${MBEDTLS_INCLUDE_DIR}/mbedtls/build_info.h _MBEDTLS_VERSION_LINE
    REGEX "^#define[ \t]+MBEDTLS_VERSION_STRING[\t ].*")
else ()
  file(STRINGS ${MBEDTLS_INCLUDE_DIR}/mbedtls/version.h _MBEDTLS_VERSION_LINE
    REGEX "^#define[ \t]+MBEDTLS_VERSION_STRING[\t ].*")
endif ()

string(REGEX REPLACE ".*MBEDTLS_VERSION_STRING[\t ]+\"(.*)\"" "\\1"
  MBEDTLS_VERSION ${_MBEDTLS_VERSION_LINE})

# If we find all three libraries, then go ahead.
if(MBEDTLS_LIB AND MBEDCRYPTO_LIB AND MBEDX509_LIB)
  set(MBEDTLS_INCLUDE_DIR ${MBEDTLS_INCLUDE_DIR})
  set(MBEDTLS_LIBRARIES ${MBEDTLS_LIB} ${MBEDCRYPTO_LIB} ${MBEDX509_LIB})
elseif(MBEDTLS_LIB AND MBEDTLS_INCLUDES_CRYPTO AND MBEDTLS_INCLUDES_X509)
  # Otherwise, if we find MBEDTLS_LIB, and it has both CRYPTO and x509
  # within the single lib (i.e. a windows build environment), then also
  # feel free to go ahead.
  set(MBEDTLS_INCLUDE_DIR ${MBEDTLS_INCLUDE_DIR})
  set(MBEDTLS_LIBRARIES ${MBEDTLS_LIB})
endif()

# Now we've accounted for the 3-vs-1 library case:
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MbedTLS
  REQUIRED_VARS MBEDTLS_LIB MBEDTLS_INCLUDE_DIR
  VERSION_VAR MBEDTLS_VERSION)

# show the MBEDTLS_INCLUDE_DIR and MBEDTLS_LIBRARIES variables only in the advanced view
mark_as_advanced(MBEDTLS_LIB MBEDCRYPTO_LIB MBEDX509_LIB)
