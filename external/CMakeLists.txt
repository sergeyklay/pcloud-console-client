# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

add_subdirectory(CLI11)

add_library(log_c STATIC log.c/src/log.c)
add_library(log_c::log_c ALIAS log_c)

target_include_directories(log_c PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/log.c/src")
