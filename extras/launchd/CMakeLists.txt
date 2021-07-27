# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

if("${PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR}" STREQUAL "")
  message(FATAL_ERROR
    "Variable PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR is not defined or empty")
endif()

message(STATUS "launchd plist install dir: ${PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR}")
