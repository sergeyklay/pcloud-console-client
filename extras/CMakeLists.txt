# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

option(PCLOUD_WITH_SYSTEMD "Enable systemd integration" OFF)
if(PCLOUD_WITH_SYSTEMD)
  add_subdirectory(systemd)
endif()

option(PCLOUD_WITH_LAUNCHD "Enable launchd integration" OFF)
if(PCLOUD_WITH_LAUNCHD)
  add_subdirectory(launchd)
endif()
