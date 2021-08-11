# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

if(PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR STREQUAL "")
  message(FATAL_ERROR
    "Variable PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR is not defined or empty")
endif()

if(NOT PCLOUD_MAINTAINER_LOG_FILE OR PCLOUD_MAINTAINER_LOG_FILE STREQUAL "")
  set(PCLOUD_MAINTAINER_LOG_FILE "/dev/null")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/com.pcloud.pcloudcc@3.plist.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/com.pcloud.pcloudcc@3.plist" @ONLY)

install(FILES com.pcloud.pcloudcc@3.plist
  DESTINATION ${PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR})
