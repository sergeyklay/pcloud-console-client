# Changelog

This file contains a brief summary of new features and dependency changes or
releases, in reverse chronological order.

## 3.0.0 (2021-XX-XX)

### Removals

* Code cleanup. Removed not used, old and duplicate stuff to avoid confusion.
* Removed hardcoded mbedtls library.
* Removed hardcoded sqlite3 library.

### Features

* Build project using single CMake command only.
* Introduced the CI process through GitHub Actions.
* Provided systemd service example.
* Install build and run time dependencies using Conan.
* Provided ability to use custom installation path using `CMAKE_INSTALL_PREFIX`.

### Bugfix

* Fixed typos and misspellings in the documentation.
* Fixed redefinition of `_GNU_SOURCE` and `_DARWIN_USE_64_BIT_INODE`.
* Fixed `control_tools` function signatures.
* Add missed return for `clib::pclcli::start_crypto`,
  `clib::pclcli::stop_crypto` and `clib::pclcli::finalize`.

### Changes

* Improved documentation.
* CMake < 3.10.2 is no longer supported.
* Reorganized project structures to follow modern CMake layout.
* Instructed the compiler to use `-std=gnu99` when it is needed.
* Project build no longer uses `pcloudcc_lib` shared library and from now the
  entire runtime is in one file.
* The previous version of the client used `lsb_release` on registration phase.
  However, this could lead to internal data structure overflow and was unstable
  due to the fact that `lsb_release` could return a string of arbitrary length.
  This behavior was disabled in the current client version. Now the client send
  only the OS name and client version.
* Bumped Mbed TLS from 1.3.10 to 2.25.0
* Bumped SQLite3 from 3.12.0 to 3.36.0
