# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

set(PCLOUD_VERSION_MAJOR 3)
set(PCLOUD_VERSION_MINOR 0)
set(PCLOUD_VERSION_PATCH 0)

if(NOT DEFINED PCLOUD_VERSION_SUFFIX)
  set(PCLOUD_VERSION_SUFFIX a2)
endif()

set(PROJECT_VERSION_WITHOUT_TWEAK
    ${PCLOUD_VERSION_MAJOR}.${PCLOUD_VERSION_MINOR}.${PCLOUD_VERSION_PATCH})

if(DEFINED PCLOUD_VERSION_TWEAK AND PCLOUD_VERSION_TWEAK)
  set(PCLOUD_VERSION ${PROJECT_VERSION_WITHOUT_TWEAK}.${PCLOUD_VERSION_TWEAK})
else()
  set(PCLOUD_VERSION ${PROJECT_VERSION_WITHOUT_TWEAK})
endif()

set(PCLOUD_PACKAGE_STRING "${PCLOUD_VERSION}-${PCLOUD_VERSION_SUFFIX}")
set(PCLOUD_PACKAGE_URL "https://github.com/sergeyklay/pcloud-console-client")

math(
    EXPR
    PCLOUD_VERSION_ID
    ${PCLOUD_VERSION_MAJOR}*10000+${PCLOUD_VERSION_MINOR}*100+${PCLOUD_VERSION_PATCH}
)

set(PCLOUD_PACKAGE_NAME "pCloud Console Client")
set(PCLOUD_VERSION_FULL "${PCLOUD_PACKAGE_NAME} v${PCLOUD_PACKAGE_STRING}")

string(TIMESTAMP CURRENT_YEAR %Y)
string(TIMESTAMP CURRENT_DATE_TIME "%Y-%m-%d %H:%M:%S")

set(PCLOUD_TEAM "Serghei Iakovlev")
set(PCLOUD_COPYRIGHT "(c) ${CURRENT_YEAR} ${PCLOUD_TEAM}")
set(PCLOUD_PACKAGE_BUILD_DATE "${CURRENT_DATE_TIME}")

set(PROJECT_VERSION ${PCLOUD_VERSION})
set(PROJECT_DESCRIPTION "A simple console client for pCloud cloud storage.")

get_filename_component(current_dir ${CMAKE_CURRENT_LIST_FILE} PATH)
configure_file("${current_dir}/version.hpp.in"
    "${pcloudcc_SOURCE_DIR}/include/pcloudcc/version.hpp")

unset(current_dir)
unset(CURRENT_YEAR)
unset(CURRENT_DATE_TIME)
unset(PROJECT_VERSION_WITHOUT_TWEAK)
