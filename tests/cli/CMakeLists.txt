# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(GoogleTest)

file(GLOB PCLOUD_CLI_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(cli_tests)
target_sources(cli_tests
  PRIVATE ${PCLOUD_TESTS_SOURCE_DIR}/main.cpp ${PCLOUD_CLI_TESTS})

target_include_directories(cli_tests
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${PCLOUD_TESTS_SOURCE_DIR}>
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/cli>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(cli_tests
  PRIVATE pcloud::cli
          gtest_main)

gtest_discover_tests(cli_tests
  TEST_PREFIX cli:
  PROPERTIES LABELS cli_tests)

set_property(GLOBAL APPEND PROPERTY PCLOUD_TESTS cli_tests)
