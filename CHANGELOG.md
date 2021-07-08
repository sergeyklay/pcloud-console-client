# Changelog

This file contains a brief summary of new features and dependency changes or
releases, in reverse chronological order.

## 3.0.0 (2021-XX-XX)

### Removals

* Code cleanup. Removed not used, old and duplicate stuff to avoid confusion.

### Features

* Introduced the CI process through GitHub Actions.
* Provided systemd service example.

### Bugfix

* Fixed typos and misspelling in the documentation.
* Fixed redefinition of `_GNU_SOURCE` and `_DARWIN_USE_64_BIT_INODE`.

### Changes

* Improved documentation.
* CMake < 3.3 is no longer supported.
* Instructed the compiler to use `-std=gnu99` when it is needed.
* Reorganized project structures to follow modern CMake layout.
* Provided ability to use custom installation path using `CMAKE_INSTALL_PREFIX`.
* Project building no longer generates shared objects
  (`*.so`, `*.dlyb`, `*.dll`) and from now the entire runtime is in one file
  (`pcloudcc`).
