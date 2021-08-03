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

macro(generate_project_config TARGET_PATH)
  message(STATUS "Generating project config")
  configure_file("${PROJECT_SOURCE_DIR}/cmake/config.h.in"
                 "${TARGET_PATH}/config.h")
  message(STATUS "Generating project config - done")
endmacro()
