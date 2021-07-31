# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(CMakeDependentOption)

option(PCLOUD_WARNINGS_AS_ERRORS "Turn all build warnings into errors")

add_library(compilerflags INTERFACE)
add_library(pcloud::compilerflags ALIAS compilerflags)

# Clang / GCC
# For "-Werror" see target_compile_options() bellow
set(unix-warnings
    -Wall             # Baseline reasonable warnings
    -Wextra           # Reasonable and standard
    -Wpedantic        # Warn if non-standard C++ is used
    -Wshadow          # Warn if a variable declaration shadows one from a parent context
    -Wsign-conversion # Warn for implicit conversions that may change the sign of an integer value
    -Wswitch-enum)    # Warn whenever a "switch" lacks a "case"

# MSVC
if(MSVC AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.15.0")
  cmake_policy(SET CMP0092 NEW)
endif(MSVC)

# For "/WX" see target_compile_options() bellow
set(msvc-warnings
    /W4) # Baseline reasonable warnings

# This is recognized as a valid compiler flag only by GCC
if(CMAKE_COMPILER_IS_GNUCXX)
  # Warn for constructs that violate guidelines in Effective C++
  list(APPEND unix-warnings -Weffc++)
endif()

# Enable all flags
target_compile_options(
    compilerflags
    INTERFACE $<$<CXX_COMPILER_ID:MSVC>:${msvc-warnings}
              $<$<BOOL:${PCL_WARNINGS_AS_ERRORS}>:/WX>>
              $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:${unix-warnings}
              $<$<BOOL:${PCL_WARNINGS_AS_ERRORS}>:-Werror>>)

# Verify compiler flags
string(TOUPPER ${CMAKE_BUILD_TYPE} _config)
get_property(_languages GLOBAL PROPERTY ENABLED_LANGUAGES)

foreach(_lang IN LISTS _languages)
  message(STATUS "Common compiler flags for ${_lang}: ${CMAKE_${_lang}_FLAGS}")
  message(STATUS "${CMAKE_BUILD_TYPE} compiler flags for ${_lang}: ${CMAKE_${_lang}_FLAGS_${_config}}")
endforeach()

unset(_config)
unset(_languages)
