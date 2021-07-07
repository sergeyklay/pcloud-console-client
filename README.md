# pCloud Console Client

A simple Linux console client for [pCloud](https://pcloud.com) cloud storage.

## Prerequisites

To build pCloud Console Client you'll need the following requirements:

- [Zlib](http://zlib.net/) >= 1.1.4: A software library used for data compression
- [Boost](http://www.boost.org/) >= 1.58: Boost system and boost program options libraries used for console client
- [Pthread](https://www.gnu.org/software/pth/): The GNU Portable Threads
- [Fuse](https://github.com/libfuse/libfuse) >= 2.6, < 3.0: Filesystem in Userspace
- [SQLite](https://www.sqlite.org/index.html) >= 3.0

Also, you'll need the following build tools:

- A C99/C++11 compatible compiler such as
  [GCC](https://gcc.gnu.org),
  [Intel C++ Compiler](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/dpc-compiler.html),
  [Clang](https://clang.llvm.org) or
  [Apple Clang](https://apps.apple.com/us/app/xcode/id497799835)
- [CMake](https://cmake.org/) >= 2.8.12
- [GNU Make](https://www.gnu.org/software/make) >= 3.82

**Note:** Some parts of the client use GNU extensions to ISO C99 standard,
thus your compiler should support `-std=gnu99`.

On Debian and its derivatives you can install the required packages this way:

```sh
$ sudo apt install \
    build-essential \
    cmake \
    fuse \
    libboost-program-options-dev \
    libboost-system-dev \
    libfuse-dev \
    libpthread-stubs0-dev \
    libudev-dev \
    zlib1g-dev
```
On macOS, you most likely have a bundled with Xcode compiler as well as pthread:

```sh
$ brew install \
    cmake \
    macfuse \
    boost \
    zlib
```

## Build steps

First you'll need clone the project:

```sh
$ git clone https://github.com/pcloudcom/console-client.git
$ cd console-client
```

Finally, configure and build project as follows:

```sh
$ cd pCloudCC/lib/pclsync
$ make clean fs

$ cd ../mbedtls
$ cmake .
$ make clean
$ make

$ cd ../..
$ cmake .
$ make

$ sudo make install
$ sudo ldconfig

$ pcloudcc -u username -p
```

## Usage

See [Maintaining pCloud Console Client](https://github.com/sergeyklay/console-client/blob/master/doc/Usage.md).

## Packages

### Current stable packages

- Ubuntu 18.04 64 bit
  [pcloudcc_2.1.0-1_amd64_ubuntu.18.04.deb](https://my.pcloud.com/publink/show?code=XZvLyi7Zsz7t1H0aYIFiawL4LSgN3uxLBUJX)
- Debian 9.9 64 bit
  [pcloudcc_2.1.0-1_amd64_debian.9.9.deb](https://my.pcloud.com/publink/show?code=XZYVyi7ZseHyB89XXK0lVAdyy0AwQYl7osU7)
- Debian 9.9 32 bit
  [pcloudcc_2.1.0-1_i386_debian.9.9.deb](https://my.pcloud.com/publink/show?code=XZuVyi7ZLevxTwQKGrSrxp8uIrQodBwDfX67)

### Older pre-built packages

- Binary package 64 bit
  [pcloudcc_2.0.1-1_amd64.deb](https://my.pcloud.com/publink/show?code=XZv1aQ7ZkEd1Vr0gj3hTteoDtujd481o7amk)
- Ubuntu 17.10 64 bit
  [pcloudcc_2.0.1-1_amd64_ubuntu.17.10.deb](https://my.pcloud.com/publink/show?code=XZFeaQ7ZH1nHUfK4MLzGdeCvmmJywBUFANyy)
- Ubuntu 14.04 64 bit
  [pcloudcc_2.0.1-1_amd64_ubuntu.14.04.deb](https://my.pcloud.com/publink/show?code=XZSeaQ7ZFPq1g8oowJXyXap7KKzTtSKoACHy)

### Build package

To create a Debian package form the source use:

```sh
$ debuild -i -us -uc -b
```
