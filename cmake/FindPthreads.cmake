# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Find the FUSE includes and library
#
#  PTHREADS_INCLUDE_DIR  - Where to find pthread.h, etc.
#                          This will be empty on non-WIN32 systems, its OK.
#  PTHREADS_LIBRARY      - List of libraries when using pthread. This may be
#                          empty if the thread functions are provided by the
#                          system libraries and no special flags are needed to
#                          use them.
#  PTHREADS_FOUND        - True if a supported thread library was found.

if (MSVC)
  option(PTHREADS_PATH "Location of the PTHREADS-WIN32 library, e.g. C:\\Program Files\\pthreads\\pthreads-2.9.1" "")
endif()

if (WIN32)
  if("${PTHREADS_PATH}" STREQUAL "")
    message(FATAL_ERROR
      "PTHREADS_PATH is not set. Please set it to actual location of the "
      "pthreads library, e.g. C:\\Program Files\\pthreads\\pthreads-2.9.1")
  endif()

  if ("${PCLOUD_TARGET_ARCH}" STREQUAL "x86_64")
    set(pthreads_lib_path "${PTHREADS_PATH}/lib/x64")
    set(pthreads_dll_path "${PTHREADS_PATH}/dll/x64")
  elseif("${PCLOUD_TARGET_ARCH}" STREQUAL "i386")
    set(pthreads_lib_path "${PTHREADS_PATH}/lib/x86")
    set(pthreads_dll_path "${PTHREADS_PATH}/dll/x86")
  else ()
    message(FATAL_ERROR "Unsupported architecture: ${PCLOUD_TARGET_ARCH}")
  endif()

  set(PTHREADS_FOUND True)
  set(PTHREADS_INCLUDE_DIR "${PTHREADS_PATH}/include")
  set(PTHREADS_LIBRARY "${pthreads_lib_path}/pthreadVC2.lib")

  if(NOT EXISTS ${PTHREADS_LIBRARY})
    set(PTHREADS_FOUND False)
  endif()

  if(PTHREADS_FOUND)
    message(STATUS "Check for pthreads: ${PTHREADS_LIBRARY}")
    set(CMAKE_USE_WIN32_THREADS_INIT True)
    set(CMAKE_USE_PTHREADS_INIT True)
  else()
    if (Pthreads_FIND_REQUIRED)
      message(SEND_ERROR "Check for pthreads: not found")
    else()
      message(WARNING "Check for pthreads: not found")
    endif()
    set(PTHREADS_LIBRARY "" CACHE STRING "" FORCE) # delete it
  endif()

  install(
    FILES "${pthreads_dll_path}/pthreadVC2.dll"
    DESTINATION ${CMAKE_INSTALL_BINDIR})

  unset(pthreads_dll_path)
  unset(pthreads_lib_path)
else()
  set(CMAKE_THREAD_PREFER_PTHREAD True)
  find_package(Threads ${Pthreads_FIND_REQUIRED})

  set(PTHREADS_FOUND ${Threads_FOUND})
  set(PTHREADS_INCLUDE_DIR "")
  set(PTHREADS_LIBRARY ${CMAKE_THREAD_LIBS_INIT})
endif()
