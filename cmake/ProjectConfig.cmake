# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(CheckSymbolExists)

message(STATUS "Checking symbols")

# macOS, *BSD, Cygwin, musl
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)

message(STATUS "Checking symbols - done")

message(STATUS "Generating config: ${PROJECT_SOURCE_DIR}/src/config.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}>/src/config.h")
message(STATUS "Generating config - done")
