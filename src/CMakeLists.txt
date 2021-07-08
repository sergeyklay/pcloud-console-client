# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
# Copyright (c) 2013-2016 Anton Titov.
# Copyright (c) 2013-2016 pCloud Ltd.
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
include_directories("${Boost_INCLUDE_DIR}")
link_directories("${Boost_LIBRARY_DIR}")

set (CMAKE_C_FLAGS "-fPIC")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync")

set (OVERLAY_CLIENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/poverlay_linux")
include_directories("${OVERLAY_CLIENT_PATH}")

add_library(sqlite3 STATIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite/sqlite3.c")
target_link_libraries(sqlite3 z dl)

add_library(pcloudcc_lib
    SHARED pclsync_lib_c.cpp
           pclsync_lib.cpp
           control_tools.cpp
           "${OVERLAY_CLIENT_PATH}/overlay_client.c"
           "${OVERLAY_CLIENT_PATH}/debug.c")

target_include_directories(pcloudcc_lib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync")

target_link_libraries(pcloudcc_lib
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync/libpsynclib.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/mbedtls/library/libmbedtls.a"
    fuse
    pthread
    sqlite3)

add_executable(pcloudcc main.cpp)

target_link_libraries(pcloudcc pcloudcc_lib)

target_link_libraries(pcloudcc "${Boost_LIBRARIES}")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/pclsync"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/mbedtls/library"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite"
    "${OVERLAY_CLIENT_PATH}")

install(TARGETS pcloudcc RUNTIME DESTINATION bin)
install(TARGETS pcloudcc_lib LIBRARY DESTINATION lib)
