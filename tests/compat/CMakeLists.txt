# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(GoogleTest)

file(GLOB COMPAT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(compat_tests)
target_sources(compat_tests
  PRIVATE ${PROJECT_TESTS_SOURCE_DIR}/main.cpp ${COMPAT_TESTS})

target_include_directories(compat_tests
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${PROJECT_TESTS_SOURCE_DIR}>
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(compat_tests
  PRIVATE pcloud::psync
          CONAN_PKG::gtest)

gtest_discover_tests(compat_tests
  TEST_PREFIX compat:
  PROPERTIES LABELS compat_tests)

set_property(GLOBAL APPEND PROPERTY PROJECT_TESTS compat_tests)
