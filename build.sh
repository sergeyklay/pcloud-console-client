#!/usr/bin/env bash
#
# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
#
# This source file is subject to the New BSD License that is bundled with this
# project in the file LICENSE.
#
# If you did not receive a copy of the license and are unable to obtain it
# through the world-wide-web, please send an email to egrep@protonmail.ch so
# we can send you a copy immediately.

# trace ERR through pipes
set -o pipefail

# trace ERR through 'time command' and other functions
set -o errtrace

# set -u : exit the script if you try to use an uninitialised variable
set -o nounset

# set -e : exit the script if any statement returns a non-true return value
set -o errexit

# Clear any cache
rm -rf "$(pwd)/.build"

conan install . -if=.build --build=missing

echo "Configure client"
cmake -S . -B .build \
  -DPCLOUD_MAINTAINER_LOGS=ON \
  -DPCLOUD_MAINTAINER_LOG_FILE="" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local"

echo "Build client"
cmake --build .build --config Debug

echo "Install client"
cmake --build .build --target install
