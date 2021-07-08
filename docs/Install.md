# Installing pCloud Console Client

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
- [CMake](https://cmake.org/) >= 3.3
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
$ git clone https://github.com/sergeyklay/pcloud-console-client.git
$ cd console-client
```

Finally, configure and build project as follows:

```sh
$ cd src/lib/pclsync
$ make clean fs

$ cd ../mbedtls
$ cmake .
$ make clean all

$ cd ../..
$ cmake -S . -B .build
$ cmake --build .build
```

### Configure flags

To enable any build/run-time feature use CMake flags at configure time.
For example, to enable the feature called `FEATURE` use `-DFEATURE=ON` and to
disable the feature use `-DFEATURE=OFF`. Below are the special flags that are
recognized during the project configuration phase:

| Flag                            | Description                                               |
| ------------------------------- |-----------------------------------------------------------|
| `PCLSYNC_WARNINGS_AS_ERRORS`    | Turn all build warnings into errors.                      |
| `CMAKE_EXPORT_COMPILE_COMMANDS` | Enable output of compile commands during generation.      |

## Install

To install client after the build stage simple use `install` target:

```sh
$ sudo cmake --build .build --target install
```

To install client in a non-standard location you'll need to change the
installation prefix. Use `-DCMAKE_INSTALL_PREFIX=/new/location` to change it
at client configure time as follows:

```sh
$ # Configure build
$ cmake -S . -B .build -DCMAKE_INSTALL_PREFIX=~/.local

# Build client
$ cmake --build .build

# Install client (this will use custom prefix now)
$ cmake --build .build --target install
```
