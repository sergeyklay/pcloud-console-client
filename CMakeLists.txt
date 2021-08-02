# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

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
include(CommonDefinitions)
include(LanguageStandard)
include(Utils)
include(ConanHelper)
include(ProjectInfo)
include(CheckSymbols)

message(STATUS "Setting installation destination to: ${CMAKE_INSTALL_PREFIX}")

get_target_architecture(PCLOUD_TARGET_ARCH)
message(STATUS "Target architecture: ${PCLOUD_TARGET_ARCH}")

# Use a standard directory layout
include(GNUInstallDirs)

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
