# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

add_subdirectory(lib/pclsync)

set(OVERLAY_CLIENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/poverlay_linux")
include_directories(${OVERLAY_CLIENT_PATH})

set(pclcli_sources
    pclcli.cpp
    control_tools.cpp
    "${OVERLAY_CLIENT_PATH}/overlay_client.c"
    "${OVERLAY_CLIENT_PATH}/debug.c")

add_library(pclcli STATIC ${pclcli_sources})
add_library(pcloud::pclcli ALIAS pclcli)
unset(pclcli_sources)

target_include_directories(pclcli
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync")

target_link_libraries(pclcli
    PRIVATE pcloud::psync
            CONAN_PKG::sqlite3
            ${CMAKE_THREAD_LIBS_INIT})

add_executable(pcloudcc main.cpp)

target_link_libraries(pcloudcc
    PRIVATE pcloud::pclcli
            CONAN_PKG::boost)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync"
    ${OVERLAY_CLIENT_PATH})

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
