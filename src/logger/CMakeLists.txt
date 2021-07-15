# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Add definition for logger library
add_library(logger STATIC logger.c)
add_library(pcloud::logger ALIAS logger)

target_include_directories(logger PUBLIC ${CMAKE_CURRENT_LIST_DIR})

configure_file("${CMAKE_CURRENT_LIST_DIR}/logger.h.in"
  "${CMAKE_CURRENT_LIST_DIR}/logger.h")

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

target_link_libraries(logger
  PUBLIC CONAN_PKG::log.c)
