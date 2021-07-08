# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
# Copyright (c) 2013-2016 Anton Titov.
# Copyright (c) 2013-2016 pCloud Ltd.
#
# This source file is subject to the New BSD License that is bundled with this
# project in the file LICENSE.
#
# If you did not receive a copy of the license and are unable to obtain it
# through the world-wide-web, please send an email to egrep@protonmail.ch so
# we can send you a copy immediately.

include(BuildType)

# Make sure developers do not run cmake in the main project directory,
# to keep build artifacts from becoming clutter
get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

if("${srcdir}" STREQUAL "${bindir}")
  message(FATAL_ERROR
      "In-source builds not allowed. They are unsupportable in practice, they spill "
      "their guts into .gitignore, and they turn buildsystem bugs into damage to sources. "
      "Instead, build the project out-of source: Create a "
      "separate directory for the build *outside of the source folder*, and run "
      "cmake <path to the source dir> and build from there."
      "You may need to remove CMakeCache.txt.")
endif()

include(ProcessorCount)

ProcessorCount(N)
if(NOT N EQUAL 0)
  set(BUILD_JOBS ${N})
else()
  message(WARNING
      "There's a problem determining the processor count. Set fallback to: 2")
  set(BUILD_JOBS 2)
endif()

# When you create a shared library or DLL with the MSVC compiler, you
# have to tell it what symbols you want exported from the library.
# UNIX compilers default to exporting all symbols.  Windows defaults
# to NONE.   When you create a DLL, you get two files a <LIB>.dll and
# IF you have exports a <LIB>.lib.
#
# The following config will enable UNIX-like behavior in Windows and
# forces to export all symbols.
#
# For more see:
# https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.4")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Generate a "compile_commands.json" file containing the exact compiler
# calls for all translation units of the project in machine-readable form.
# This is useful  for all sorts of tools ("clang-tidy", "cppcheck", "oclint",
# "include-what-you-use", etc).
#
# Note This option is implemented only by Makefile Generators and the Ninja.
# It is ignored on other generators.
#
# For more see:
# https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.5.0")
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()
