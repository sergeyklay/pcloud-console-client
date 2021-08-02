# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(CheckSymbolExists)
include(CheckTimespec)

message(STATUS "Checking symbols")

# macOS, *BSD, Cygwin, musl
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)
check_timespec_exists(STRUCT_TIMESPEC)

message(STATUS "Checking symbols - done")
