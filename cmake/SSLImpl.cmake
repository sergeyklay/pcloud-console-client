# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

set(allowed_ss_impl mbedtls openssl securetransport)
set(PCLOUD_SSL_IMPL "mbedtls" CACHE STRING "Used SSL implementation")
set_property(CACHE PCLOUD_SSL_IMPL PROPERTY STRINGS "${allowed_ss_impl}")

if(NOT PCLOUD_SSL_IMPL IN_LIST allowed_ss_impl)
  message(FATAL_ERROR "Invalid SSL implementation: ${PCLOUD_SSL_IMPL}")
else()
  message(STATUS "Using SSL implementation: ${PCLOUD_SSL_IMPL}")
endif()

unset(allowed_ss_impl)
