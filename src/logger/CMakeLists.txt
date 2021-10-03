# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# TODO: Consider migrate to https://github.com/google/glog

# Add definition for logger library
add_library(logger STATIC logger.c)
add_library(pcloud::logger ALIAS logger)

target_include_directories(
  logger
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/src)

target_link_libraries(logger PRIVATE Threads::Threads)

# The additional / is important to remove the last character from the path.
# Note that it does not matter if the OS uses / or \, because we are only
# saving the path size.
string(LENGTH "${PROJECT_SOURCE_DIR}/" PCLOUD_SOURCE_PATH_SIZE)
target_compile_definitions(logger
  PRIVATE "-DPCLOUD_SOURCE_PATH_SIZE=${PCLOUD_SOURCE_PATH_SIZE}"
          "-DPCLOUD_SOURCE_PATH=\"${PROJECT_SOURCE_DIR}\"")

set(log_levels 0 1 2 3 4 5)
set(PCLOUD_MAINTAINER_LOGS_LEVEL "1" CACHE STRING "Maintainer logs level")
set_property(CACHE PCLOUD_MAINTAINER_LOGS_LEVEL PROPERTY STRINGS "${log_levels}")

if(PCLOUD_MAINTAINER_LOGS_LEVEL IN_LIST log_levels)
  target_compile_definitions(logger
    PRIVATE "-DPCLOUD_LOG_LEVEL=${PCLOUD_MAINTAINER_LOGS_LEVEL}")
endif()

unset(log_levels)

option(PCLOUD_MAINTAINER_LOGS "Enable maintainer logs" OFF)
if (PCLOUD_MAINTAINER_LOGS)
  target_compile_definitions(logger
    PRIVATE PCLOUD_MAINTAINER_LOGS)
endif()

set(PCLOUD_MAINTAINER_LOG_FILE "/tmp/pcloudcc.log" CACHE STRING "Maintainer log file")
if (PCLOUD_MAINTAINER_LOG_FILE)
  target_compile_definitions(logger
    PRIVATE "-DPCLOUD_LOG_FILE=\"${PCLOUD_MAINTAINER_LOG_FILE}\"")
endif()

unset(log_levels)

# Create position independent library (add -fPIC)
set_property(TARGET logger
  PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(logger
  PUBLIC log_c::log_c)
