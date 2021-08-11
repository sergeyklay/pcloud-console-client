# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD "systemd")

if(SYSTEMD_FOUND AND PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR STREQUAL "")
  execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE}
      --variable=systemduserunitdir systemd
      OUTPUT_VARIABLE PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR)
  string(REGEX REPLACE "[ \t\n]+" "" PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR
      "${PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR}")
elseif(NOT SYSTEMD_FOUND AND PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR)
  message(FATAL_ERROR
      "Variable PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR is defined, "
      "but we can't find systemd using pkg-config")
endif()

if(SYSTEMD_FOUND)
  set(with_systemd ON)
  message(STATUS "systemd services install dir: ${PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR}")
else()
  set(with_systemd OFF)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pcloudcc@.service.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/pcloudcc@.service")

if(with_systemd)
  install(FILES pcloudcc@.service
      DESTINATION ${PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR})
endif()
unset(with_systemd)
