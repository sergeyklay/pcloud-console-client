# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Add definition for cli library.
add_library(cli STATIC bridge.cpp ct.cpp formatter.cpp app.cpp)
add_library(pcloud::cli ALIAS cli)

target_include_directories(
  cli
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/src/psync
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/src/overlay_client)

find_package(SQLite3 REQUIRED)

target_link_libraries(
  cli
  PUBLIC  CLI11::CLI11
  PRIVATE pcloud::logger
          pcloud::psync
          pcloud::overlay_client
          SQLite::SQLite3
          Threads::Threads)
