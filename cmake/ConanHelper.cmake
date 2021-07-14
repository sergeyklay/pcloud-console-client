# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

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
