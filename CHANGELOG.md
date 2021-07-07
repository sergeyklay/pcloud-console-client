# Changelog

This file contains a brief summary of new features and dependency changes or
releases, in reverse chronological order.

## 3.0.0 (2021-XX-XX)

### Removals

* Code cleanup. Removed not used, old and duplicate stuff to avoid confusion.

### Features

* Introduced CI process through GitHub Actions.

### Bugfix

* Fixed typos and misspelling in the documentation.
* Fixed redefinition of `_GNU_SOURCE` and `_DARWIN_USE_64_BIT_INODE`.

### Changes

* Improved documentation.
* CMake < 2.8.12 is no longer supported.
* Some parts of the client use GNU extensions to ISO C99 standard.
  Instruct the compiler to use `-std=gnu99`.
