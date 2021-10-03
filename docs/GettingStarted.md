## Getting Started

These instructions will get you a copy of the project up and running on your
local machine for development and testing purposes.

### Prerequisites

#### Build and runtime dependencies

To build pCloud Console Client you'll need a POSIX-compliant operating system
and the presence of the following libraries:
- [Pthread](https://www.gnu.org/software/pth/): The GNU Portable Threads
- [Fuse](https://github.com/libfuse/libfuse) >= 2.6, < 3.0: Filesystem in
  Userspace on Linux/UNIX ([macFUSE](https://osxfuse.github.io) on macOS)
- `pkg-config` on Linux to perform some system checks
- [SQLite](https://www.sqlite.org/index.html) 3: In-process library that implements a
  self-contained, serverless, zero-configuration, transactional SQL database engine
- [Zlib](https://zlib.net) >= 1.2.11: A massively spiffy yet delicately unobtrusive
  compression library

#### Build tools

To build pCloud Console Client you'll need the following build tools:
- A C99/C++14 compatible compiler such as
  [GCC](https://gcc.gnu.org),
  [Intel C++ Compiler](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/dpc-compiler.html),
  [Clang](https://clang.llvm.org) or
  [Apple Clang](https://apps.apple.com/us/app/xcode/id497799835)
- [CMake](https://cmake.org/) >= 3.14
- Any build tool supported by CMake like
  [GNU Make](https://www.gnu.org/software/make/),
  [Ninja](https://ninja-build.org/) and so on
- [Conan](https://conan.io/) decentralized package manager with a
  client-server architecture
- [Git](https://git-scm.com) - a free and open source distributed version control system

**Note:** Some parts of the client use GNU extensions to ISO C99 standard,
thus your compiler should support `-std=gnu99`.

#### Optional

Optional prerequisites are:
- Documentation generation tool: [Doxygen](http://www.doxygen.org/)
- Graph visualization toolkit: [Graphviz](http://www.graphviz.org/)

#### Debian

On Debian and its derivatives you can install the required packages this way:
```sh
$ sudo apt install \
    cmake \
    fuse \
    g++ \
    gcc \
    git \
    libfuse-dev \
    libpthread-stubs0-dev \
    libsqlite3-dev \
    make \
    pkg-config \
    zlib1g-dev
```

To install conan on Linux distros use [`pip`](https://pip.pypa.io/):
```sh
$ pip3 install --user conan
```

**Note:** On some Linux systems the standard version of CMake is less than
3.14 so, you'll need to upgrade CMake.

#### macOS

On macOS, you most likely have a bundled compiler with Xcode as well as
pthread library. If so, you'll need only these packages:
```sh
$ brew install \
    cmake \
    conan \
    git \
    macfuse \
    sqlite \
    zlib
```

### Build steps

First you'll need clone the project and fetch its modules:
```sh
$ git clone https://github.com/sergeyklay/pcloud-console-client.git
$ cd pcloud-console-client
$ git submodule init
$ git submodule update
```

And wait for load any required dependencies. For git 1.6.1 or above you can use
something similar to command bellow to pull latest of all submodules:

```sh
$ git submodule update --remote --merge
```

Next, initialize project with Conan - this is using the `conanfile.txt`
specifying that Conan should integrate with CMake:
```sh
$ conan install . -if=build --build=missing
```

This example establishes out-of-source `build/` directory, so that source
folder is not polluted. For a detailed instruction on how to use and
customize conan please refer
[here](https://docs.conan.io/en/latest/getting_started.html).

Next, generate the build files using CMake:
```sh
$ cd build
$ cmake ..
```

Finally, build client:
```sh
$ cmake --build .
```

#### Configure flags

To enable any build/run-time feature use CMake flags at configure time.
For example, to enable the feature called `FEATURE` use `-DFEATURE=ON` and to
disable the feature use `-DFEATURE=OFF`. Below are the special flags that are
recognized during the project configuration phase:

| Feature                               | Possible Value                                                      | Description                                                       |
| ------------------------------------- |---------------------------------------------------------------------|-------------------------------------------------------------------|
| `PCLOUD_WARNINGS_AS_ERRORS`           | `ON`, `OFF`                                                         | Turn all build warnings into errors.                              |
| `PCLOUD_TLS_IMPL`                     | `mbedtls`, `openssl`, `securetransport`                             | Implementation of the TLS and SSL protocols.                      |
| `PCLOUD_BUILD_DOC`                    | `ON`, `OFF`                                                         | ON to generate the API documentation, OFF to ignore it.           |
| `PCLOUD_MAINTAINER_LOGS`              | `ON`, `OFF`                                                         | ON to enable maintainer logs, OFF to disable logging.             |
| `PCLOUD_MAINTAINER_LOG_FILE`          | A path like `/tmp/pcloudcc.log`                                     | Maintainer log file.                                              |
| `PCLOUD_MAINTAINER_LOGS_LEVEL`        | 0, 1, 2, 3, 4, 5                                                    | Logs level. From 0 (trace), to 5 (fatal error).                   |
| `PCLOUD_WITH_LAUNCHD`                 | `ON`, `OFF`                                                         | Enable launchd integration.                                       |
| `PCLOUD_LAUNCHD_PLISTS_INSTALL_DIR`   | A path recognized by launchd                                        | System or user plist search path for systemd (see bellow).        |
| `PCLOUD_WITH_SYSTEMD`                 | `ON`, `OFF`                                                         | Enable systemd integration.                                       |
| `PCLOUD_SYSTEMD_SERVICES_INSTALL_DIR` | A path recognized by systemd                                        | System or user unit search path for systemd (see bellow).         |
| `PCLOUD_WITH_TESTS`                   | `ON`, `OFF`                                                         | Enable testing support (see bellow).                              |
| `CMAKE_BUILD_TYPE`                    | `Release`, `Debug`, `RelWithDebInfo`, `MinSizeRel`, `Asan`, `Ubsan` | CMake build mode (see bellow).                                    |
| `CMAKE_INSTALL_PREFIX`                | A path like `~/.local`, or `/opt`                                   | This directory is prepended onto all install directories.         |

##### Switching build modes in CMake

CMake supports different build modes, to allow the stripping of debug
information, or to add coverage information to the binaries.

The following modes are supported:

* **Release:** This generates the default code without any unnecessary
  information in the binary files.
* **Debug:** This generates debug information and disables optimization of
  the code.
* **RelWithDebInfo:** Is the same as **Release**, allowing
  you to have symbol files for debugging.
* **MinSizeRel:** Is the same as **Release**, with its
  optimization configuration just set to minimize size.
* **Asan:** This instruments the code with Address Sanitizer to check for
  memory errors.
* **Ubsan:** This instruments the code with Undefined Behavior Sanitizer to
  check for undefined behavior errors.

To enable any of these build mode use `-DCMAKE_BUILD_TYPE=MODE` at configure
phase, and then specify at build phase, e.g.:

```sh
# Configure client
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..

# Build client
cmake --build . --config Release
```

##### Generate the API documentation

Follow these steps:

1. Configure with code coverage instrumentation enabled
  `-DPCLOUD_BUILD_DOC=ON`
2. Build project
3. Call `cmake --build build --target doc` from the project root directory
4. Open `build/docs/html/index.html` in your browser to view the API
   documentation

#### Running the tests

To build with testing support, you have to configure project with special
flags:

```shell
# Configure client
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug -DPCLOUD_WITH_TESTS=ON ..

# Build client
$ cmake --build . --config Release
```

The run tests as follows:
```sh
# Under the build directory:
$ cmake --build . --target check
```

### Install

To install client after the build stage simple use `install` target:

```sh
# Under the build directory:
$ sudo cmake --build . --target install
```

To install client in a non-standard location you'll need to change the
installation prefix. Use `-DCMAKE_INSTALL_PREFIX=/new/location` to change it
at client configure time as follows:

```sh
# Configure build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=~/.local ..

# Build client
$ cmake --build .

# Install client (this will use custom prefix now)
$ cmake --build . --target install
```

To install systemd unit use `-DPCLOUD_WITH_SYSTEMD=ON`. You can also pass
`-DPCLOUD_SYSTEMD_SERVICES_INSTALL_DIR=/unit/location` to change unit search
path for systemd. For example:

```sh
# Configure build
$ cd build
$ cmake \
  -DCMAKE_INSTALL_PREFIX=~/.local \
  -DPCLOUD_WITH_SYSTEMD=ON \
  -DPCLOUD_SYSTEMD_SERVICES_INSTALL_DIR==~/.config/systemd/user \
  ..

# Build client
$ cmake --build .

# Install client (this will use custom prefix now)
$ cmake --build . --target install
```

To install launchd agent use `-DPCLOUD_WITH_LAUNCHD=ON`. You can also pass
`-DPCLOUD_LAUNCHD_PLISTS_INSTALL_DIR=/agent/location` to change agent load
path for launchd. For example:

```sh
# Configure build
$ cd build
$ cmake \
  -DCMAKE_INSTALL_PREFIX=~/.local \
  -DPCLOUD_WITH_LAUNCHD=ON \
  -DPCLOUD_LAUNCHD_PLISTS_INSTALL_DIR=~/Library/LaunchAgents \
  ..

# Build client
$ cmake --build .

# Install client (this will use custom prefix now)
$ cmake --build . --target install
```
