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

CMAKE_PRESET=${CMAKE_PRESET:-linux-x64-gcc-mbedtls-debug}

conan install . -if=out/build/"${CMAKE_PRESET}" --build=missing
cmake --preset="${CMAKE_PRESET}"
cmake --build --preset="${CMAKE_PRESET}"

ctest --preset="${CMAKE_PRESET}"

# cmake --build --preset="${CMAKE_PRESET}" --target doc
# cmake --build --preset="${CMAKE_PRESET}" --target install
