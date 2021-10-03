# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

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
  message(STATUS "Setting installation destination to: ${CMAKE_INSTALL_PREFIX}")
endif()

# Common CMake scripts
include(Bootstrap)
include(CompilerFlags)
include(LanguageStandard)
include(ConanHelper)
include(ProjectInfo)
include(ProjectConfig)

generate_project_config("${PROJECT_SOURCE_DIR}/src")

# Use a standard directory layout
include(GNUInstallDirs)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/CLI11/CMakeLists.txt")
  # Here, it provides the GIT_EXECUTABLE variable after searching for the git
  # binary in some standard/well-known locations for the current platform.
  find_package(Git REQUIRED)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} submodule update --init
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(NOT ${result} EQUAL 0)
    string(REPLACE "\n" "\n    " output "${output}")
    message(FATAL_ERROR
      "Failed to get required git modules.\n"
      "  Result: ${result}\n"
      "  Output:\n"
      "    ${output}\n")
  endif()

  unset(result)
  unset(output)
endif()

add_subdirectory(external)

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
