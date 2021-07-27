#!/usr/bin/env bash
#
# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# trace ERR through pipes
set -o pipefail

# trace ERR through 'time command' and other functions
set -o errtrace

# set -u : exit the script if you try to use an uninitialised variable
set -o nounset

# set -e : exit the script if any statement returns a non-true return value
set -o errexit

BUILD_TYPE=${BUILD_TYPE:-Debug}

INSTALL_PREFIX=${INSTALL_PREFIX:-"$HOME/.local"}
SYSTEMD_UNIT_PATH=${SYSTEMD_UNIT_PATH:-"$HOME/.config/systemd/user"}
LAUNCHD_PLIST_PATH=${SYSTEMD_UNIT_PATH:-"$HOME/Library/LaunchAgents"}
CONAN_PROFILE=${CONAN_PROFILE:-default}
LOG_FILE=${LOG_FILE:-""}

WITH_LOGS=${WITH_LOGS:-ON}
WITH_LEVEL=${WITH_LEVEL:-1}
WITH_DOCS=${WITH_DOCS:-ON}
WITH_SYSTEMD=${WITH_SYSTEMD:-ON}
WITH_LAUNCHD=${WITH_LAUNCHD:-OFF}
WITH_TESTS=${WITH_TESTS:-ON}

if [[ "$WITH_LOGS" == "OFF" ]]; then
  LOG_FILE=/dev/null
  WITH_LEVEL=5
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
  WITH_SYSTEMD=OFF
  SYSTEMD_UNIT_PATH=""
  if [[ "$WITH_LOGS" == "ON" ]]; then
    LOG_FILE=/var/log/pcloudcc.log
  fi
else
  WITH_LAUNCHD=OFF
  LAUNCHD_PLIST_PATH=""
fi

conan install . -if=build -pr="${CONAN_PROFILE}" --build=missing

echo "Configure client"
cmake -S . -B build \
  -DPCLOUD_MAINTAINER_LOGS_LEVEL="${WITH_LEVEL}" \
  -DPCLOUD_MAINTAINER_LOGS="${WITH_LOGS}" \
  -DPCLOUD_MAINTAINER_LOG_FILE="${LOG_FILE}" \
  -DPCLOUD_BUILD_DOC="${WITH_DOCS}" \
  -DPCLOUD_WITH_SYSTEMD="${WITH_SYSTEMD}" \
  -DPCLOUD_SYSTEMD_SERVICES_INSTALL_DIR="${SYSTEMD_UNIT_PATH}" \
  -DPCLOUD_WITH_LAUNCHD="${WITH_LAUNCHD}" \
  -DPCLOUD_LAUNCHD_PLISTS_INSTALL_DIR="${LAUNCHD_PLIST_PATH}" \
  -DPCLOUD_WITH_TESTS="${WITH_TESTS}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"

echo "Build client"
cmake --build build --config "${BUILD_TYPE}" -j "$(getconf _NPROCESSORS_ONLN)"

if [ "${WITH_DOCS}" = "ON" ]; then
  echo "Generate the API documentation"
  cmake --build build --target doc -j "$(getconf _NPROCESSORS_ONLN)"
fi

echo "Install client"
cmake --build build --target install
