#!/usr/bin/env bash
#
# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

if [ "$1" = "gcc" ]; then
  echo "Installing gcc-${2}, g++-${2}..."
  sudo apt-get install --no-install-recommends -q -y "gcc-${2}" "g++-${2}"

  if [ ! -f "/usr/bin/gcc-${2}" ] || [ ! -f "/usr/bin/g++-${2}" ]; then
    echo "No such version gcc/g++ installed" 1>&2
    exit 1
  fi

  sudo update-alternatives --remove-all gcc || true
  sudo update-alternatives --install /usr/bin/gcc gcc "/usr/bin/gcc-${2}" 90
  sudo update-alternatives --set gcc "/usr/bin/gcc-${2}"

  sudo update-alternatives --remove-all cc || true
  sudo update-alternatives --install /usr/bin/cc cc "/usr/bin/gcc-${2}" 90
  sudo update-alternatives --set cc "/usr/bin/gcc-${2}"

  sudo update-alternatives --remove-all g++ || true
  sudo update-alternatives --install /usr/bin/g++ g++ "/usr/bin/g++-${2}" 90
  sudo update-alternatives --set g++ "/usr/bin/g++-${2}"

  sudo update-alternatives --remove-all c++ || true
  sudo update-alternatives --install /usr/bin/c++ c++ "/usr/bin/g++-${2}" 90
  sudo update-alternatives --set c++ "/usr/bin/g++-${2}"

  echo "CC=gcc" >> "$GITHUB_PATH"
  echo "CXX=g++" >> "$GITHUB_PATH"
else
  echo "Installing clang-${2}, llvm++-${2}..."
  sudo apt-get install --no-install-recommends -q -y "clang-${2}" "llvm-${2}"

  if [ ! -f "/usr/bin/clang-${2}" ] || [ ! -f "/usr/bin/clang++-${2}" ]; then
    echo "No such version clang/clang++ installed" 1>&2
    exit 1
  fi

  sudo update-alternatives --remove-all clang || true
  sudo update-alternatives --install /usr/bin/clang clang "/usr/bin/clang-${2}" 90
  sudo update-alternatives --set clang "/usr/bin/clang-${2}"

  sudo update-alternatives --remove-all cc || true
  sudo update-alternatives --install /usr/bin/cc cc "/usr/bin/clang-${2}" 90
  sudo update-alternatives --set cc "/usr/bin/clang-${2}"

  sudo update-alternatives --remove-all clang++ || true
  sudo update-alternatives --install /usr/bin/clang++ clang++ "/usr/bin/clang++-${2}" 90
  sudo update-alternatives --set clang++ "/usr/bin/clang++-${2}"

  sudo update-alternatives --remove-all c++ || true
  sudo update-alternatives --install /usr/bin/c++ c++ "/usr/bin/clang++-${2}" 90
  sudo update-alternatives --set c++ "/usr/bin/clang++-${2}"

  sudo update-alternatives --install /usr/bin/llvm-cov llvm-cov \
    "/usr/bin/llvm-cov-${2}" 90

  echo "CC=clang" >> "$GITHUB_PATH"
  echo "CXX=clang++" >> "$GITHUB_PATH"
fi
