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

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(pcloudcc LANGUAGES C CXX)

# Provide path for additional CMake scripts
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# CMake Script Includes
include(Bootstrap)
include(CompilerFlags)
include(LanguageStandard)
include(ProjectInfo)

# Determine if Stars is built as a subproject (using add_subdirectory) or if
# it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
  # Print the version number of CMake if this is the main project
  message(STATUS "CMake version: ${CMAKE_VERSION}")
  message(STATUS "Used generator: ${CMAKE_GENERATOR}")
endif()

# Use a standard directory layout
include(GNUInstallDirs)

add_subdirectory(src)
