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

if(NOT DEFINED PCLSYNC_VERSION_MAJOR)
  set(PCLSYNC_VERSION_MAJOR 3)
endif()

if(NOT DEFINED PCLSYNC_VERSION_MINOR)
  set(PCLSYNC_VERSION_MINOR 0)
endif()

if(NOT DEFINED PCLSYNC_VERSION_PATCH)
  set(PCLSYNC_VERSION_PATCH 0)
endif()

if(NOT DEFINED PCLSYNC_VERSION_SUFFIX)
  set(PCLSYNC_VERSION_SUFFIX git)
endif()

set(PROJECT_VERSION_WITHOUT_TWEAK
    ${PCLSYNC_VERSION_MAJOR}.${PCLSYNC_VERSION_MINOR}.${PCLSYNC_VERSION_PATCH})

if(DEFINED PCLSYNC_VERSION_TWEAK AND PCLSYNC_VERSION_TWEAK)
  set(PCLSYNC_VERSION ${PROJECT_VERSION_WITHOUT_TWEAK}.${PCLSYNC_VERSION_TWEAK})
else()
  set(PCLSYNC_VERSION ${PROJECT_VERSION_WITHOUT_TWEAK})
endif()

set(PCLSYNC_PACKAGE_STRING "${PCLSYNC_VERSION}-${PCLSYNC_VERSION_SUFFIX}")
set(PCLSYNC_PACKAGE_URL "https://github.com/sergeyklay/pcloud-console-client")

math(
    EXPR
    PCLSYNC_VERSION_ID
    ${PCLSYNC_VERSION_MAJOR}*10000+${PCLSYNC_VERSION_MINOR}*100+${PCLSYNC_VERSION_PATCH}
)

set(PCLSYNC_PACKAGE_NAME "pCloud Console Client")
set(PCLSYNC_VERSION_FULL "${PCLSYNC_PACKAGE_NAME} v${PCLSYNC_PACKAGE_STRING}")

string(TIMESTAMP CURRENT_YEAR %Y)
string(TIMESTAMP CURRENT_DATE_TIME "%Y-%m-%d %H:%M:%S")

set(PCLSYNC_TEAM "Serghei Iakovlev")
set(PCLSYNC_COPYRIGHT "(c) ${CURRENT_YEAR} ${PCLSYNC_TEAM}")
set(PCLSYNC_PACKAGE_BUILD_DATE "${CURRENT_DATE_TIME}")

set(PROJECT_VERSION ${PCLSYNC_VERSION})
set(PROJECT_DESCRIPTION "A simple Linux console client for pCloud cloud storage.")

get_filename_component(current_dir ${CMAKE_CURRENT_LIST_FILE} PATH)
configure_file("${current_dir}/version.hpp.in"
    "${pcloudcc_SOURCE_DIR}/src/version.hpp")

unset(current_dir)
unset(CURRENT_YEAR)
unset(CURRENT_DATE_TIME)
unset(PROJECT_VERSION_WITHOUT_TWEAK)
