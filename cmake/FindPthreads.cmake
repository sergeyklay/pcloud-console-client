# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Find the FUSE includes and library
#
#  PTHREADS_INCLUDE_DIR  - Where to find pthread.h, etc.
#                          May be empty on non-WIN32 systems, its OK.
#  PTHREAD_LIBRARY       - List of libraries when using pthread.
#  PTHREAD_FOUND         - True if pthread lib is found.

if (MSVC)
  option(PTHREADS_PATH "Location of the PTHREADS-WIN32 library, e.g. C:\\Program Files\\pthreads\\pthreads-2.9.1" "")
endif()

if (WIN32)
  find_package(Threads)
  if (Threads_FOUND)
    set(PTHREAD_FOUND ${Threads_FOUND})
    set(PTHREAD_LIBRARY ${CMAKE_USE_WIN32_THREADS_INIT})
  else()
    if("${PTHREADS_PATH}" STREQUAL "")
      message(FATAL_ERROR
        "PTHREADS_PATH is not set. Please set it to actual location of the "
        "pthreads library, e.g. C:\\Program Files\\pthreads\\pthreads-2.9.1")
    endif()

    if ("${PCLOUD_TARGET_ARCH}" STREQUAL "x86_64")
      set(PTHREAD_LIB_PATH "${PTHREADS_PATH}/lib/x64")
    elseif("${PCLOUD_TARGET_ARCH}" STREQUAL "i386")
      set(PTHREAD_LIB_PATH "${PTHREADS_PATH}/lib/x86")
    else ()
      message(FATAL_ERROR "Unsupported architecture: ${PCLOUD_TARGET_ARCH}")
    endif()

    set(PTHREAD_FOUND True)
    set(PTHREADS_INCLUDE_DIR "${PTHREADS_PATH}/include")
    set(PTHREAD_LIBRARY "${PTHREAD_LIB_PATH}/pthreadVC2.lib")

    if(NOT EXISTS ${PTHREAD_LIBRARY})
      set(PTHREAD_FOUND False)
    endif()
  endif()
else()
  set(CMAKE_THREAD_PREFER_PTHREAD True)
  find_package(Threads REQUIRED)

  set(PTHREAD_FOUND ${Threads_FOUND})
  set(PTHREADS_INCLUDE_DIR "")
  set(PTHREAD_LIBRARY ${CMAKE_THREAD_LIBS_INIT})
endif()
