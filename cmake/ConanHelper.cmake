# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
#
# This source file is subject to the New BSD License that is bundled with this
# project in the file LICENSE.
#
# If you did not receive a copy of the license and are unable to obtain it
# through the world-wide-web, please send an email to egrep@protonmail.ch so
# we can send you a copy immediately.

message(STATUS "Searching conanbuildinfo.cmake in ${PROJECT_BINARY_DIR}")
find_file(CONANBUILDINFO "conanbuildinfo.cmake" PATHS ${PROJECT_BINARY_DIR})

if(NOT CONANBUILDINFO)
  message(FATAL_ERROR "The \"conanbuildinfo.cmake\" file is missing!"
      " You must run conan install first.")
endif()

set(CONAN_CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CONAN_CMAKE_CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS})

include(${CONANBUILDINFO})

conan_basic_setup(TARGETS KEEP_RPATHS)
