# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

set(PCLSYNC_ALLOWED_SSL_IMPL mbedtls openssl securetransport)
set(PCLSYNC_SSL_IMPL "mbedtls" CACHE STRING "Used SSL implementation") # TODO: Add to docs
set_property(CACHE PCLSYNC_SSL_IMPL PROPERTY STRINGS "${PCLSYNC_ALLOWED_SSL_IMPL}")

if(NOT PCLSYNC_SSL_IMPL IN_LIST PCLSYNC_ALLOWED_SSL_IMPL)
  message(FATAL_ERROR "Invalid SSL implementation: ${PCLSYNC_SSL_IMPL}")
else()
  message(STATUS "Used SSL implementation: ${PCLSYNC_SSL_IMPL}")
endif()

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
    poverlay_lin.c
    poverlay_mac.c
    poverlay_win.c
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

if(PCLSYNC_SSL_IMPL STREQUAL "openssl")
  set(psync_ssl_def P_SSL_OPENSSL)
  list(APPEND psync_sources pssl-openssl.c)
elseif(PCLSYNC_SSL_IMPL STREQUAL "securetransport")
  set(psync_ssl_def P_SSL_SECURETRANSPORT)
  list(APPEND psync_sources pssl-securetransport.c)
else()
  set(psync_ssl_def P_SSL_MBEDTLS)
  list(APPEND psync_sources pssl-mbedtls.c)
endif()

mark_as_advanced(PCLSYNC_ALLOWED_SSL_IMPL PCLSYNC_SSL_IMPL)

# Add definition for psync library
add_library(psync STATIC ${psync_sources})
add_library(pcloud::psync ALIAS psync)
unset(psync_sources)

target_compile_definitions(psync PRIVATE ${psync_ssl_def})
unset(psync_ssl_def)

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
if(FUSE_FOUND)
  target_compile_definitions(psync
      PRIVATE FUSE_USE_VERSION=${FUSE_USE_VERSION}
              "${FUSE_DEFINITIONS}")
endif()

target_include_directories(psync
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${FUSE_INCLUDE_DIR})

if(PCLSYNC_SSL_IMPL STREQUAL "mbedtls")
  target_link_libraries(psync CONAN_PKG::mbedtls)
endif()

# TODO: Do we need openssl here in case of mbedtls?
target_link_libraries(psync
    CONAN_PKG::openssl
    CONAN_PKG::zlib
    CONAN_PKG::sqlite3
    ${FUSE_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

if(APPLE)
  target_link_libraries(psync "-framework Cocoa")
endif()
