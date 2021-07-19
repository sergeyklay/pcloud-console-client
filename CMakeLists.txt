# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(pcloudcc LANGUAGES C CXX)

# Provide path for additional CMake scripts
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Determine if Stars is built as a subproject (using add_subdirectory) or if
# it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
  # Print the version number of CMake if this is the main project
  message(STATUS "CMake version: ${CMAKE_VERSION}")
  message(STATUS "Used generator: ${CMAKE_GENERATOR}")
endif()

# Common CMake scripts
include(Bootstrap)
include(CompilerFlags)
include(LanguageStandard)

# Conan helpers
include(ConanHelper)

# Project specific variables
include(ProjectInfo)

message(STATUS "Setting installation destination to: ${CMAKE_INSTALL_PREFIX}")

if(UNIX)
  # Use a standard directory layout
  include(GNUInstallDirs)
elseif(WIN32)
  set(${CMAKE_INSTALL_LIBDIR} "lib")
  set(${CMAKE_INSTALL_DATADIR} "share")
  set(${CMAKE_INSTALL_INCLUDEDIR} "include")
  set(${CMAKE_INSTALL_BINDIR} "bin")
else()
  message(FATAL_ERROR "System not UNIX nor WIN32. You OS is not be supported yet")
endif()

add_subdirectory(src)
add_subdirectory(extras)

option(PCLOUD_WITH_TESTS "Enable testing support" OFF)
if(MASTER_PROJECT)
  include(CTest)
  add_subdirectory(docs)

  if(BUILD_TESTING AND PCLOUD_WITH_TESTS)
    add_subdirectory(tests)
  endif()
endif()
