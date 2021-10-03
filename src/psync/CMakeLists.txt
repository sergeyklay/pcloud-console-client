# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Setup TLS implementation
include(TLSImplementation)

set(psync_sources
    pcompat.c
    pstringcompat.c
    psynclib.c
    plocks.c
    plibs.c
    pcallbacks.c
    pdiff.c
    pstatus.c
    papi.c
    ptimer.c
    pupload.c
    pdownload.c
    pfolder.c
    psyncer.c
    ptasks.c
    psettings.c
    pnetlibs.c
    pcache.c
    pscanner.c
    plist.c
    plocalscan.c
    plocalnotify.c
    pp2p.c
    pcrypto.c
    pssl.c
    pfileops.c
    ptree.c
    ppassword.c
    prunratelimit.c
    pmemlock.c
    pnotifications.c
    pexternalstatus.c
    publiclinks.c
    pbusinessaccount.c
    pcontacts.c
    poverlay.c
    pcompression.c
    pasyncnet.c
    ppathstatus.c
    pdeviceid.c
    # It seems these ones are experimental features
    # so disabled for now:
    #
    # pdevicemap.c
    # pdevice_monitor.c
    #
    # Not:e After enable do not forget install 'libudev-dev' on Linux.
    # Also amend documentation and CI workflow file.
    pfs.c
    ppagecache.c
    pfsfolder.c
    pfstasks.c
    pfsupload.c
    pintervaltree.c
    pfsxattr.c
    pcloudcrypto.c
    pfscrypto.c
    pcrc32c.c
    pfsstatic.c
    plocks.c
    psockets.c)

if(PCLOUD_TLS_IMPL STREQUAL "openssl")
  list(APPEND psync_sources pssl-openssl.c)
elseif(PCLOUD_TLS_IMPL STREQUAL "securetransport")
  list(APPEND psync_sources pssl-securetransport.c)
elseif(PCLOUD_TLS_IMPL STREQUAL "mbedtls")
  list(APPEND psync_sources pssl-mbedtls.c)
else()
  if(PCLOUD_TLS_IMPL STREQUAL "")
    list(APPEND psync_sources pssl-mbedtls.c)
  else()
    message(SEND_ERROR "Unknown TLS implementation: ${PCLOUD_TLS_IMPL}")
  endif()
endif()

# Add definition for psync library
add_library(psync SHARED ${psync_sources})
add_library(pcloud::psync ALIAS psync)
unset(psync_sources)

if(APPLE)
  target_compile_definitions(psync PRIVATE P_OS_MACOSX)
elseif(UNIX)
  target_compile_definitions(psync PRIVATE P_OS_LINUX)
endif()

find_package(FUSE 2.6 REQUIRED)
target_compile_definitions(psync
    PRIVATE FUSE_USE_VERSION=${FUSE_USE_VERSION}
            "${FUSE_DEFINITIONS}")

target_include_directories(psync
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
          ${FUSE_INCLUDE_DIR}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/src)

if(PCLOUD_TLS_IMPL STREQUAL "openssl")
  target_link_libraries(psync PUBLIC CONAN_PKG::openssl)
  target_compile_definitions(psync PUBLIC P_SSL_OPENSSL)
elseif(PCLOUD_TLS_IMPL STREQUAL "securetransport")
  target_compile_definitions(psync PUBLIC P_SSL_SECURETRANSPORT)
else()
  find_package(MbedTLS 2.8.0 REQUIRED)
  if (NOT MBEDTLS_FOUND)
    message(FATAL_ERROR "Could not find MbedTLS library")
  else()
    target_include_directories(psync PUBLIC ${MBEDTLS_INCLUDE_DIR})
  endif()
  target_link_libraries(psync PUBLIC ${MBEDTLS_LIBRARIES})
  target_compile_definitions(psync PUBLIC P_SSL_MBEDTLS)
endif()

find_package(ZLIB REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(psync
    PRIVATE pcloud::logger
            ZLIB::ZLIB
            SQLite::SQLite3
            Threads::Threads
            ${FUSE_LIBRARY})

if(APPLE)
  target_link_libraries(psync PRIVATE "-framework Cocoa")
endif()

# It seems device monitor is in experimental phase
# so disable it for now:
#
#  find_package(PkgConfig REQUIRED)
#  pkg_check_modules(UDEV REQUIRED libudev)
#  target_link_libraries(psync ${UDEV_LIBRARIES})
#
# Not:e After enable do not forget install 'libudev-dev' on Linux.
# Also amend documentation and CI workflow file.

# Library version info
set(PSYNC_VERSION_MAJOR 2)
set(PSYNC_VERSION_MINOR 0)
set(PSYNC_VERSION_PATCH 0)

set(PSYNC_VERSION_STRING
    ${PSYNC_VERSION_MAJOR}.${PSYNC_VERSION_MINOR}.${PSYNC_VERSION_PATCH})
set(PSYNC_VERSION_FULL "libpsync v${PSYNC_VERSION_STRING}")

configure_file("${PROJECT_SOURCE_DIR}/cmake/psync-version.h.in"
    "${PROJECT_SOURCE_DIR}/include/pcloudcc/psync/version.h")

# The SOVERSION is not necessarily the same as the major version. The rule is
# that any breakage of the ABI must be indicated by incrementing the SOVERSION.
# So, adding e.g. functions is no problem, modifying argument lists or removing
# functions would required the SOVERSION to be incremented. Similar rules hold
# of course for non-opaque data-structures.
set_target_properties(psync PROPERTIES
    VERSION   ${PSYNC_VERSION_STRING}
    SOVERSION ${PSYNC_VERSION_MAJOR})

install(TARGETS psync
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
