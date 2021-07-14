# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

set(allowed_tls_impls mbedtls openssl securetransport)
set(PCLOUD_TLS_IMPL "mbedtls" CACHE STRING "Used TLS implementation")
set_property(CACHE PCLOUD_TLS_IMPL PROPERTY STRINGS "${allowed_tls_impls}")

if(NOT PCLOUD_TLS_IMPL IN_LIST allowed_tls_impls)
  message(FATAL_ERROR "Invalid TLS implementation: ${PCLOUD_TLS_IMPL}")
else()
  message(STATUS "Using TLS implementation: ${PCLOUD_TLS_IMPL}")
endif()

unset(allowed_tls_impls)
