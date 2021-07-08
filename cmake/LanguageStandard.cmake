# pCloud Console Client
#
# Copyright (c) 2021 Serghei Iakovlev.
#
# This source file is subject to the New BSD License that is bundled with this
# project in the file LICENSE.
#
# If you did not receive a copy of the license and are unable to obtain it
# through the world-wide-web, please send an email to egrep@protonmail.ch so
# we can send you a copy immediately.

# Require C99. Some parts of the client use GNU extensions to ISO C99 standard,
# thus the compiler should support `-std=gnu99`
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Require C++11. We don't use GNU extensions to ISO C++11 standard, thus make
# sure we're able build project using ISO C++.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  # MSVC does not support /std:c99
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11")
endif(MSVC)
