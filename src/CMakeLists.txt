# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

add_compile_definitions(P_CONSOLE_CLIENT)

include(CheckSymbolExists)
message(STATUS "Checking symbols")
# macOS, *BSD, Cygwin, musl
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)
message(STATUS "Checking symbols - done")

configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
  "${CMAKE_CURRENT_LIST_DIR}/config.h")

add_subdirectory(logger)
add_subdirectory(psync)
add_subdirectory(overlay_client)
add_subdirectory(cli)

add_executable(pcloudcc main.cpp)

target_link_libraries(pcloudcc
    PRIVATE pcloud::cli
            pcloud::compilerflags)

file(RELATIVE_PATH relative
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
    ${CMAKE_INSTALL_PREFIX})

if(APPLE)
  set(rpath "@loader_path/${relative}")
else()
  set(rpath "\$ORIGIN/${relative}")
endif()

file(TO_NATIVE_PATH
    "${rpath}/${CMAKE_INSTALL_LIBDIR}"
    message_RPATH)

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
