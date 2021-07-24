# Changelog

This file contains a brief summary of new features and dependency changes or
releases, in reverse chronological order.

## 3.0.0 (2021-XX-XX)

### Removals

* Code cleanup. Removed not used, old and duplicate stuff to avoid confusion.
* Removed hardcoded Mbed TLS library.
* Removed hardcoded SQLite3 library.
* Removed useless logs. Were removed macro from the `PRINT_RETURN` family.
* The original version of client, when some errors occurred with the database,
  sent debug information to the pCloud's servers. This debug information
  included psync library version, file name, line number, function name, and
  the error message itself. This project isn't maintained by the pCloud's
  employees, so this functionality has been removed.
* Removed `finalize` command from commands mode as it was almost useless and
  errors prone. For example, we were forced to hard terminate the program,
  w/o response on socket request and this has always been regarded as a
  communication error. To stop already running daemon just stop it in the
  same way as you started it - using standard system tools.
* Removed never used and no longer required `PSYNC_CRYPTO_*_NOT_SUPPORTED` defines.

### Features

* Build project using single CMake command only.
* Introduced the CI process through GitHub Actions.
* Provided systemd integration on Linux platforms.
* Install project dependencies using Conan.
* Provided ability to use custom installation path using `CMAKE_INSTALL_PREFIX`.
* Provide ability to enable maintainer logs using `log.c` library.
* Added possibility to use EU region servers.
* Provided ability to generate the API documentation.
* Provided a brand-new libpsync API to get and set device information used
  during the pCloud API calls.
* Reworked commands mode to provide better user experience.
* Added `-V`, `--version` program options to display program version information.

### Bugfix

* Fixed typos and misspellings in the documentation.
* Fixed redefinition of some macro.
* Added return result to functions where it was skipped by mistake.
* Fixed incorrect function signatures detected by the static analyzer.
* Fixed device name by removing extra characters from output `lsb_release`
  on some systems. For example double quotes in `"Gentoo Base System release 2.7"`
* Fixed reading response message while processing commands leading to a segfault.
* The previous version of the client output the user's crypto password in cleartext
  to the standard output. This behavior has been changed.
* Previously, when trying to start a crypto session with an incorrect password, the
  client reported success, although the crypto session didn't start.
  This has been fixed.
* Previously, when trying to stop not started crypto session, client reported
  success. This has been fixed.

### Changes

* Improved documentation.
* CMake < 3.12 is no longer supported.
* Reorganized project structure to follow modern C/C++ projects structure.
* Instructed the compiler to use `-std=gnu99` when this is needed.
* Rename `pcloudcc_lib` static library to shared `libpsync`.
* Bumped `libpsync` soversion from 1 to 2.
* Bumped Mbed TLS from 1.3.10 to 2.25.0.
* Bumped SQLite3 from 3.12.0 to 3.36.0.
* Replaced deprecated `readdir_r()` function with `readdir()`.
* Replaced deprecated semaphores implementation on macOS with GCD semaphores.
* Updated macOS versions list to identity caller on API calls.
* Move Debian control files to the `tools` directory.
* Reworked creation of overlay socket on Linux. Now the client will try to
  create a socket in `XDG_RUNTIME_DIR` or `TMPDIR`. The `/tmp` will be used
  if there are no such environment variables.
* Provided ability to run tests (although there are still few of them, there
  is a technical possibility).
* The default mount point on macOS where drive to be mounted is "pCloud Drive" now.
* `psync_check_result()` now is function, not macro.
* Reduce code duplication by merging `poverlay-linux.c` and `poverlay-macos.c`
  into `poverlay-posix.c`.
* Moved `PSYNC_CRYPTO_*` defines from `psynclib.h` to `pcloudcrypto.h`.
* Replace `boost::program_options` by more lightweight and specialized library `CLI11`.
