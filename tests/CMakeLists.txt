# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

define_property(
  GLOBAL
  PROPERTY PROJECT_TESTS
  BRIEF_DOCS "All project tests"
  FULL_DOCS "The full list of all project tests")

set(PROJECT_TESTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(compat)

get_property(PROJECT_TESTS GLOBAL PROPERTY PROJECT_TESTS)
add_custom_target(
  check
  COMMAND ctest --output-on-failure
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS ${PROJECT_TESTS}
  COMMENT "Run all tests...")
