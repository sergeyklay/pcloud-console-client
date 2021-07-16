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

# Clear any cache
rm -rf "$(pwd)/build"

conan install . -if=build --build=missing

echo "Configure client"
cmake -S . -B build \
  -DPCLOUD_MAINTAINER_LOGS_LEVEL=0 \
  -DPCLOUD_MAINTAINER_LOGS=ON \
  -DPCLOUD_MAINTAINER_LOG_FILE="" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local"

echo "Build client"
cmake --build build --config Debug

echo "Install client"
cmake --build build --target install
