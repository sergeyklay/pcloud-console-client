# Compatibility with CMake < 2.8.12 will be removed from a future version of
# modern CMake. So tell CMake that the project does not need compatibility
# with older versions.
cmake_minimum_required(VERSION 2.8.12)

project(pcloudcc LANGUAGES C CXX)

#INCLUDE(CPack)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)

find_package(Boost COMPONENTS system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
#set (CMAKE_CXX_FLAGS "-static -Wl,-Bstatic")
#set (CMAKE_CXX_FLAGS "-Wl,-Bstatic")
set (CMAKE_C_FLAGS "-fPIC")
set (PCLSYNC_PATH ${CMAKE_SOURCE_DIR}/lib/pclsync)
set (MBEDTLS_PATH ${CMAKE_SOURCE_DIR}/lib/mbedtls)
set (SQLITE3_PATH ${CMAKE_SOURCE_DIR}/lib/sqlite)

# Require C99. Some parts of the client use GNU extensions to ISO C99 standard,
# thus teh compiler should support `-std=gnu99`
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.1.0")
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# Require C++11. We don't use GNU extensions to ISO C++11 standard, thus make
# sure we're able build project using ISO C++.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.1.0")
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.5.0")
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()

#add_subdirectory(${MBEDTLS_PATH})

include_directories(${SQLITE3_PATH})
# add_custom_target(
#   sqlite3_lib
#   COMMAND ./configure && make
#   WORKING_DIRECTORY ${SQLITE3_PATH}
# )

include_directories(${PCLSYNC_PATH})
# add_custom_target(
#   pclsync_clean
#   COMMAND make clean
#   WORKING_DIRECTORY ${PCLSYNC_PATH}
# )

add_custom_target(
  pclsync
  COMMAND make fs
  WORKING_DIRECTORY ${PCLSYNC_PATH}
)

add_custom_target(
  mbedtls
  COMMAND cmake . && make
  WORKING_DIRECTORY ${MBEDTLS_PATH}
)

set (OVERLAY_CLENT_PATH ${CMAKE_SOURCE_DIR}/lib/poverlay_linux)
include_directories(${OVERLAY_CLENT_PATH})

add_library(sqlite3 STATIC ${SQLITE3_PATH}/sqlite3.c)
target_link_libraries(sqlite3 z dl)

add_library(pcloudcc_lib SHARED pclsync_lib_c.cpp pclsync_lib.cpp control_tools.cpp ${OVERLAY_CLENT_PATH}/overlay_client.c ${OVERLAY_CLENT_PATH}/debug.c )

target_link_libraries(pcloudcc_lib ${PCLSYNC_PATH}/libpsynclib.a  ${MBEDTLS_PATH}/library/libmbedtls.a fuse pthread sqlite3
)

add_executable(pcloudcc main.cpp)

target_link_libraries(pcloudcc pcloudcc_lib)

target_link_libraries(pcloudcc ${Boost_LIBRARIES})

link_directories(${PCLSYNC_PATH} ${MBEDTLS_PATH}/library ${SQLITE3_PATH} ${OVERLAY_CLENT_PATH})

#add_dependencies(pclsync sqlite3_lib)

#add_dependencies(pcloudcc sqlite3_lib mbedtls pclsync )

install(TARGETS pcloudcc RUNTIME DESTINATION bin)
install(TARGETS pcloudcc_lib LIBRARY DESTINATION lib)
