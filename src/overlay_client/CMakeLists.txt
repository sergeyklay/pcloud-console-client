# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# Add definition for overlay_client library
add_library(overlay_client STATIC overlay_client.c debug.c)
add_library(pcloud::overlay_client ALIAS overlay_client)

if(APPLE)
  target_compile_definitions(overlay_client PRIVATE P_OS_MACOSX)
endif()
