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
    pdevice_monitor.c
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
    plocks.c)

if(PCLOUD_TLS_IMPL STREQUAL "openssl")
  list(APPEND psync_sources pssl-openssl.c)
elseif(PCLOUD_TLS_IMPL STREQUAL "securetransport")
  list(APPEND psync_sources pssl-securetransport.c)
else()
  list(APPEND psync_sources pssl-mbedtls.c)
endif()

# Add definition for psync library
add_library(psync STATIC ${psync_sources})
add_library(pcloud::psync ALIAS psync)
unset(psync_sources)

if (WIN32)
  target_compile_definitions(psync PRIVATE P_OS_WINDOWS)
elseif(APPLE)
  target_compile_definitions(psync PRIVATE P_OS_MACOSX)
  find_package(Threads REQUIRED)
elseif(UNIX)
  target_compile_definitions(psync PRIVATE P_OS_LINUX)
  find_package(Threads REQUIRED)
endif()

find_package(FUSE 2.6 REQUIRED)
target_compile_definitions(psync
    PRIVATE FUSE_USE_VERSION=${FUSE_USE_VERSION}
            "${FUSE_DEFINITIONS}")

target_include_directories(psync
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
          ${FUSE_INCLUDE_DIR}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

if(PCLOUD_TLS_IMPL STREQUAL "openssl")
  target_link_libraries(psync CONAN_PKG::openssl)
  target_compile_definitions(psync PRIVATE P_SSL_OPENSSL)
elseif(PCLOUD_TLS_IMPL STREQUAL "securetransport")
  target_compile_definitions(psync PRIVATE P_SSL_SECURETRANSPORT)
else()
  target_link_libraries(psync CONAN_PKG::mbedtls)
  target_compile_definitions(psync PRIVATE P_SSL_MBEDTLS)
endif()

target_link_libraries(psync
    pcloud::logger
    CONAN_PKG::zlib
    CONAN_PKG::sqlite3
    ${FUSE_LIBRARY})

if(APPLE)
  target_link_libraries(psync "-framework Cocoa")
endif()

if (NOT WIN32)
  target_link_libraries(psync ${CMAKE_THREAD_LIBS_INIT})
endif()
