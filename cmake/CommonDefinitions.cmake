# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

# The additional / is important to remove the last character from the path.
# Note that it does not matter if the OS uses / or \, because we are only
# saving the path size.
string(LENGTH "${PROJECT_SOURCE_DIR}/" PCLOUD_SOURCE_PATH_SIZE)
add_definitions("-DPCLOUD_SOURCE_PATH_SIZE=${PCLOUD_SOURCE_PATH_SIZE}")
add_definitions("-DPCLOUD_SOURCE_PATH=\"${PROJECT_SOURCE_DIR}\"")
