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

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)

find_package(Boost COMPONENTS system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_subdirectory(lib/pclsync)

set (OVERLAY_CLIENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/poverlay_linux")
include_directories(${OVERLAY_CLIENT_PATH})

add_library(pclcli
    STATIC pclcli.cpp
           control_tools.cpp
           "${OVERLAY_CLIENT_PATH}/overlay_client.c"
           "${OVERLAY_CLIENT_PATH}/debug.c")

target_include_directories(pclcli
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync")

target_link_libraries(pclcli
    pcloud::psync
    fuse pthread sqlite3)

if(PCLSYNC_SSL_IMPL STREQUAL "mbedtls") # TODO: Add to docs
  target_link_libraries(pclcli CONAN_PKG::mbedtls)
endif()

add_executable(pcloudcc main.cpp)

target_link_libraries(pcloudcc
    pclcli
    ${Boost_LIBRARIES})

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync"
    "${OVERLAY_CLIENT_PATH}")

set_target_properties(pcloudcc PROPERTIES
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

install(TARGETS pcloudcc
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
