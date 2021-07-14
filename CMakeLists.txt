# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

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

# Use a standard directory layout
include(GNUInstallDirs)

# We prefer libraries over frameworks
set(CMAKE_FIND_FRAMEWORK LAST)

add_subdirectory(src)
