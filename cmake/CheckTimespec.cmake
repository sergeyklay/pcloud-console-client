# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

macro(check_timespec_exists VARIABLE)
  if(NOT DEFINED HAVE_${VARIABLE})
    # Tell the user what is happening
    message(STATUS "Check if the system includes timespec in <time.h>")

    # Write a temporary file containing the test
    set(tmp_time_file
      "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/TestTimespec.cpp")
    file(WRITE "${tmp_time_file}"
      "#include <time.h>\n"
      "int main(int argc, char* argv[]) {\n"
      "  timespec ts;\n"
      "  ts.tv_sec = 0;\n"
      "  ts.tv_nsec = 0;\n"
      "  return 0;\n"
      "}\n")

    # Try to build the file
    try_compile(HAVE_${VARIABLE}
      "${CMAKE_BINARY_DIR}"
      "${tmp_time_file}")

    # Determine whether we have timespec in time.h or not
    if(HAVE_${VARIABLE})
      set(${VARIABLE} 1 CACHE INTERNAL "Result of check_timespec_exists" FORCE)
      message(STATUS "Check if the system includes timespec in <time.h> - yes")
    else()
      # time.h does not have timespec
      set(${VARIABLE} 0 CACHE INTERNAL "Result of check_timespec_exists" FORCE)
      message(STATUS "Check if the system includes timespec in <time.h> - no")
    endif()
  endif()
endmacro()
