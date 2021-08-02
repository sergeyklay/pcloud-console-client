# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Add definition for logger library
add_library(logger STATIC logger.c)
add_library(pcloud::logger ALIAS logger)

target_include_directories(
  logger
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/src)

if (PTHREADS_FOUND AND NOT "${PTHREADS_INCLUDE_DIR}" STREQUAL "")
  target_include_directories(logger PRIVATE ${PTHREADS_INCLUDE_DIR})
endif()

# See: https://stackoverflow.com/q/33557506/1661465
if(WIN32 AND HAVE_STRUCT_TIMESPEC)
  target_compile_definitions(logger PRIVATE HAVE_STRUCT_TIMESPEC)
endif()

if (PTHREADS_FOUND AND NOT "${PTHREADS_LIBRARY}" STREQUAL "")
  target_link_libraries(logger PRIVATE ${PTHREADS_LIBRARY})
endif()

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
  PUBLIC CONAN_PKG::log.c)
