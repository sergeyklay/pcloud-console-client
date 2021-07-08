#!/usr/bin/env bash

pushd pCloudCC/lib/pclsync || exit 1
  echo "Build pclsync"
  make clean fs
popd || exit 1

pushd pCloudCC/lib/mbedtls || exit 1
  echo "Configure Mbed TLS"
  cmake -S . -B . -DCMAKE_C_STANDARD=99 -DCMAKE_BUILD_TYPE=Release

  echo "Build Mbed TLS"
  cmake --build . --config Release
popd || exit 1

pushd pCloudCC || exit 1
  echo "Configure client"
  cmake -S . -B ../.build -DCMAKE_BUILD_TYPE=Release

  echo "Build client"
  cmake --build ../.build --config Release

  echo "Install client"
  sudo make -C ../.build install
  sudo ldconfig
popd || exit 1
