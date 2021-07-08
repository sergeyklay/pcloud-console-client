#!/usr/bin/env bash
#
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

# trace ERR through pipes
set -o pipefail

# trace ERR through 'time command' and other functions
set -o errtrace

# set -u : exit the script if you try to use an uninitialised variable
set -o nounset

# set -e : exit the script if any statement returns a non-true return value
set -o errexit

echo "Build pclsync"
make -C src/lib/pclsync clean fs

pushd src/lib/mbedtls || exit 1
  echo "Configure Mbed TLS"
  cmake -S . -B . -DCMAKE_C_STANDARD=99 -DCMAKE_BUILD_TYPE=Release

  echo "Build Mbed TLS"
  cmake --build . --config Release
popd || exit 1

echo "Configure client"
cmake -S . -B .build -DCMAKE_INSTALL_PREFIX="$(pwd)/install"

echo "Build client"
cmake --build .build

echo "Install client"
cmake --build .build --target install
