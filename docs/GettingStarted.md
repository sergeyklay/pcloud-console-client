## Getting Started

These instructions will get you a copy of the project up and running on your
local machine for development and testing purposes.

### Prerequisites

To build pCloud Console Client you'll need the following requirements:
- [Pthread](https://www.gnu.org/software/pth/): The GNU Portable Threads
- [Fuse](https://github.com/libfuse/libfuse) >= 2.6, < 3.0: Filesystem in
  Userspace on Linux/UNIX (or [Dokany](https://github.com/dokan-dev/dokany)
  on Windows)

On Linux you'll need `pkg-config` to perform some system checks.

Also, you'll need the following build tools:
- A C99/C++14 compatible compiler such as
  [GCC](https://gcc.gnu.org),
  [Intel C++ Compiler](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/dpc-compiler.html),
  [Clang](https://clang.llvm.org),
  [Apple Clang](https://apps.apple.com/us/app/xcode/id497799835) or
  [Microsoft Visual Studio](https://visualstudio.microsoft.com)
- [CMake](https://cmake.org/) >= 3.12
- [GNU Make](https://www.gnu.org/software/make) >= 3.82
- [Conan](https://conan.io/) decentralized package manager with a
  client-server architecture

**Note:** Some parts of the client use GNU extensions to ISO C99 standard,
thus your compiler should support `-std=gnu99`. Also notice, MSVC does not
support C99, and only halfway supports the older versions of the C standard.

Optional prerequisites are:
- Documentation generation tool: [Doxygen](http://www.doxygen.org/)
- Graph visualization toolkit: [Graphviz](http://www.graphviz.org/)

#### Debian

On Debian and its derivatives you can install the required packages this way:
```sh
$ sudo apt install \
    gcc \
    g++ \
    make \
    cmake \
    fuse \
    libfuse-dev \
    libpthread-stubs0-dev \
    pkg-config
```

To install conan on Linux distros use pip:
```sh
$ pip3 install --user conan
```

**Note:** On some Linux systems the standard version of CMake is less than
3.12. To install at least 3.12 version use the following approach:
```sh
$ wget https://github.com/Kitware/CMake/releases/download/v3.12.0/cmake-3.12.0.tar.gz
$ tar -zxvf cmake-3.12.0.tar.gz
$ cd cmake-3.12.0
$ ./bootstrap
$ make
$ sudo make install
```

#### macOS

```
On macOS, you most likely have a bundled compiler with Xcode as well as
pthread library. If so, you'll need only these packages:
```sh
$ brew install \
    cmake \
    conan \
    macfuse
```

#### Windows

On Windows, you'll need to download and install Visual Studio as well as
Build Tools for Visual Studio from the followinf web page:
https://visualstudio.microsoft.com/downloads/ . During the installer dialog
select MSVC, CMake, Developer Tools and Classic C/C++ support.

The following dependencies can be installed using chocolatey:
```ps
C:\> choco install conan
```

Finally install [Dokany](https://github.com/dokan-dev/dokany).

### Build steps

#### Linux/macOS

First you'll need clone the project:
```sh
$ git clone https://github.com/sergeyklay/pcloud-console-client.git
$ cd pcloud-console-client
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
