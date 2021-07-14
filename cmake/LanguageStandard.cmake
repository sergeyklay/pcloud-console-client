# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

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
