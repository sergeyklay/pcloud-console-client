# Changelog

This file contains a brief summary of new features and dependency changes or
releases, in reverse chronological order.

## 3.0.0-a3 (2021-XX-XX)

### Removals

* [#95](https://github.com/sergeyklay/pcloud-console-client/issues/95):
  Removed conan dependency.

### Changes

* CMake < 3.14 is no longer supported.


## 3.0.0-a2 (2021-08-28)

### Removals

* Removed `psync_rendir` macro.
* Removed `psync_file_rename` function.
* Removed `requirements.txt` file from the project. It was only needed to
  install conan at CI phase.
* Drop Windows support. In fact, this project never built natively on Windows.
  All users could do is to build it using Cygwin or Msys. However, this project
  depends on some components (for ex. FUSE, POSIX threads and UNIX sockets) that
  either don't work under Windows, or have poor support, or require significant
  support costs. Given the fact that this project is intended for UNIX/Linux users,
  Windows support becomes an unaffordable luxury. Anyway, in the WSL/Docker era,
  things get a little easier, even for Windows users.
* Removed no longer used `PSYNC_THREAD` macro in favor of `__thread`.
* Removed no longer used `PSYNC_DEFAULT_WINDOWS_DBNAME` macro.
* Removed no longer used `PSYNC_DEFAULT_WINDOWS_DIR` macro.
* Removed no longer used `psync_32to64` macro.
* Removed no longer used `psync_bool_to_zero` macro.
* Removed no longer used `P_PRI_D64` macro.
* Removed no longer used `psync_stat_ctime` macro.
* Removed no longer used `psync_inode_supported` macro.
* Removed no longer used `psync_sock_err_t` typedef.
* Removed no longer used `PSYNC_DIRECTORY_SEPARATOR` macro.
* Removed no longer used `PSYNC_DIRECTORY_SEPARATORC` macro.
* Removed no longer used `P_SEEK_CUR` macro.
* Removed no longer used `P_SEEK_END` macro.

### Features

* Added `-dumpversion` program option alias for `--dumpversion`.
* Amended tests.
* Provided launchd integration for macOS.
* Provided support to build project under arm64/aarch64 architectures.

### Bugfix

* [#87](https://github.com/sergeyklay/pcloud-console-client/issues/87):
  Define `_FILE_OFFSET_BITS` to `64` on arm64 architectures.
  For more info about this macro refer to
  https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html.

### Changes

* Renamed `overlay_callback` pointer to function to `poverlay_callback`.
* Renamed `overlay_message_t` overlay data type to `poverlay_message_t`.
* Restructured the layout of CLI application.
* Bumped minimal C++ language version from C++11 to C++14.
* `psync_check_result()` now is macro, not function.
* Created a dedicated namespace for compat functions.
  Now they will all be in `pcloudcc/compat/*.h`
  * `pcloudcc/compat/string.h`: Some extra string manipulation functions
    not present in all standard C runtimes.
  * `pcloudcc/compat/compiler.h`: Compilers compatibility constants

## 3.0.0-a1 (2021-07-25)

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
* Added `--dumpversion` program option to print the version of the client and
  don't do anything else.
* Added `--vernum` program option to print the version of the client as integer
  and quit.

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
* Renamed `--commands_only` CLI option to `--commands-only`.
* Renamed `message` overlay data type to `overlay_message_t`.
* Renamed `get_answer_to_request()` to `psync_overlay_process_request()`.
* Renamed `psync_add_overlay_callback()` to `psync_overlay_add_callback()`.
* Split `poverlay.h` into public (`pcloudcc/psync/overlay.h`) and private part.
