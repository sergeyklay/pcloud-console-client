# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Find the MbedTLS includes and library
#
# Once done this will define
#
#  MBEDTLS_FOUND        - True if MbedTLS is found.
#  MBEDTLS_INCLUDE_DIRS - The MbedTLS include directory.
#  MBEDTLS_LIBRARIES    - Link these to use MbedTLS.

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(_MBEDTLS QUIET mbedtls)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_lib_suffix 64)
else()
  set(_lib_suffix 32)
endif()

find_path(
  MBEDTLS_INCLUDE_DIR
  NAMES mbedtls/ssl.h
  HINTS ENV MBEDTLS_PATH${_lib_suffix}
        ENV MBEDTLS_PATH
        ${MBEDTLS_PATH${_lib_suffix}}
        ${MBEDTLS_PATH}
  PATHS /usr/include
        /usr/local/include
        /opt/local/include
        /usr/local/opt/include
        /opt/homebrew/include
  PATH_SUFFIXES include)

if(MBEDTLS_INCLUDE_DIR)
  message(STATUS "Check for MbedTLS includes: ${MBEDTLS_INCLUDE_DIR}")
else()
  message(SEND_ERROR "Check for MbedTLS includes: not found")
  set(MBEDTLS_INCLUDE_DIR "" CACHE STRING "" FORCE) # delete it
endif()


if(APPLE)
  set(MBEDTLS_LIB_NAMES libmbedtls.dylib mbedtls)
else()
  set(MBEDTLS_LIB_NAMES libmbedtls mbedtls)
endif()

find_library(
  MBEDTLS_LIB
  NAMES ${MBEDTLS_LIB_NAMES}
  HINTS ENV MBEDTLS_PATH${_lib_suffix}
        ENV MBEDTLS_PATH
        ${MBEDTLS_PATH${_lib_suffix}}
        ${MBEDTLS_PATH}
  PATHS /lib64
        /lib
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /usr/local/opt/lib64
        /usr/local/opt/lib
        /opt/local/lib
        /opt/homebrew/lib)

if(MBEDTLS_LIB)
  message(STATUS "Check for mbedtls: ${MBEDTLS_LIB}")
else()
  message(SEND_ERROR "Check for mbedtls: not found")
  set(MBEDTLS_LIB "" CACHE STRING "" FORCE) # delete it
endif()

if(APPLE)
  set(MBEDCRYPTO_LIB_NAMES libmbedcrypto.dylib mbedcrypto)
else()
  set(MBEDCRYPTO_LIB_NAMES libmbedcrypto mbedcrypto)
endif()

find_library(
  MBEDCRYPTO_LIB
  NAMES ${MBEDCRYPTO_LIB_NAMES}
  HINTS ENV MBEDTLS_PATH${_lib_suffix}
        ENV MBEDTLS_PATH
        ${MBEDTLS_PATH${_lib_suffix}}
        ${MBEDTLS_PATH}
  PATHS /lib64
        /lib
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /usr/local/opt/lib64
        /usr/local/opt/lib
        /opt/local/lib
        /opt/homebrew/lib)

if(MBEDCRYPTO_LIB)
  message(STATUS "Check for mbedcrypto: ${MBEDCRYPTO_LIB}")
else()
  message(STATUS "Check for mbedcrypto: not found")
  set(MBEDCRYPTO_LIB "" CACHE STRING "" FORCE) # delete it
endif()

if(APPLE)
  set(MBEDX509_LIB_NAMES libmbedx509.dylib mbedx509)
else()
  set(MBEDX509_LIB_NAMES libmbedx509 mbedx509)
endif()

find_library(
  MBEDX509_LIB
  NAMES ${MBEDX509_LIB_NAMES}
  HINTS ENV MBEDTLS_PATH${_lib_suffix}
        ENV MBEDTLS_PATH
        ${MBEDTLS_PATH${_lib_suffix}}
        ${MBEDTLS_PATH}
  PATHS /lib64
        /lib
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /usr/local/opt/lib64
        /usr/local/opt/lib
        /opt/local/lib
        /opt/homebrew/lib)

if(MBEDX509_LIB)
  message(STATUS "Check for mbedx509: ${MBEDX509_LIB}")
else()
  message(STATUS "Check for mbedx509: not found")
  set(MBEDX509_LIB "" CACHE STRING "" FORCE) # delete it
endif()

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

# If we find all three libraries, then go ahead.
if(MBEDTLS_LIB AND MBEDCRYPTO_LIB AND MBEDX509_LIB)
  set(MBEDTLS_INCLUDE_DIRS ${MBEDTLS_INCLUDE_DIR})
  set(MBEDTLS_LIBRARIES ${MBEDTLS_LIB} ${MBEDCRYPTO_LIB} ${MBEDX509_LIB})
elseif(MBEDTLS_LIB AND MBEDTLS_INCLUDES_CRYPTO AND MBEDTLS_INCLUDES_X509)
  # Otherwise, if we find MBEDTLS_LIB, and it has both CRYPTO and x509
  # within the single lib (i.e. a windows build environment), then also
  # feel free to go ahead.
  set(MBEDTLS_INCLUDE_DIRS ${MBEDTLS_INCLUDE_DIR})
  set(MBEDTLS_LIBRARIES ${MBEDTLS_LIB})
endif()

# Now we've accounted for the 3-vs-1 library case:
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MbedTLS
  "Could NOT find mbedTLS, try to set the path to MbedTLS root folder in the system variable MBEDTLS_ROOT_DIR"
  MBEDTLS_INCLUDE_DIRS
  MBEDTLS_LIBRARIES)

# show the MBEDTLS_INCLUDE_DIRS and MBEDTLS_LIBRARIES variables only in the advanced view
mark_as_advanced(MBEDTLS_INCLUDE_DIR MBEDTLS_LIB MBEDCRYPTO_LIB MBEDX509_LIB)
