# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Setup TLS implementation
include(TLSImplementation)

include(CheckSymbolExists)
message(STATUS "Checking symbols")
# macOS, *BSD, Cygwin, musl
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)
message(STATUS "Checking symbols - done")

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
    plocks.c)

if(PCLOUD_TLS_IMPL STREQUAL "openssl")
  list(APPEND psync_sources pssl-openssl.c)
elseif(PCLOUD_TLS_IMPL STREQUAL "securetransport")
  list(APPEND psync_sources pssl-securetransport.c)
else()
  list(APPEND psync_sources pssl-mbedtls.c)
endif()

# Add definition for psync library
add_library(psync SHARED ${psync_sources})
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


configure_file("${PROJECT_SOURCE_DIR}/cmake/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/pcloudcc/config.h")

target_compile_definitions(psync
    PUBLIC HAVE_CONFIG_H)

target_link_libraries(psync
    pcloud::logger
    CONAN_PKG::zlib
    CONAN_PKG::sqlite3
    ${FUSE_LIBRARY})

if(APPLE)
  target_link_libraries(psync "-framework Cocoa")
elseif(NOT WIN32)
  # It seems device monitor is in experimental phase
  # so disable it for now:
  #
  #  find_package(PkgConfig REQUIRED)
  #  pkg_check_modules(UDEV REQUIRED libudev)
  #  target_link_libraries(psync ${UDEV_LIBRARIES})
  #
  # Not:e After enable do not forget install 'libudev-dev' on Linux.
  # Also amend documentation and CI workflow file.
endif()

if (NOT WIN32)
  target_link_libraries(psync ${CMAKE_THREAD_LIBS_INIT})
endif()

# Library version info
set(PSYNC_VERSION_MAJOR 1)
set(PSYNC_VERSION_MINOR 6)
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

# CMake treats shared libraries as runtime objects on "DLL platforms"
# like Windows. Thus RUNTIME DESTINATION is required here.
install(TARGETS psync
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
