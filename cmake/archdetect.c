/* Copyright (c) 2012 Petroules Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with
 *    the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Based on the Qt 6 processor detection code, so should be very accurate
 * https://github.com/qt/qtbase/blob/6.2/src/corelib/global/qprocessordetection.h
 *
 * Currently handles:
 * - arm (v5, v6, v7, v8)
 * - x86 (32/64)
 * - ia64
 * - ppc (32/64)
 */

#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
#if defined(__ARM64_ARCH_8__) || defined(__aarch64__) || defined(__ARMv8__) || \
    defined(__ARMv8_A__) || defined(_M_ARM64)
#error cmake_ARCH armv8
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) ||      \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) ||       \
    (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM - 0 >= 7) || \
    (defined(_M_ARM) && _M_ARM - 0 >= 7)
#error cmake_ARCH armv7
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) ||      \
    defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) ||      \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) ||      \
    defined(__ARM_ARCH_6M__) ||                                   \
    (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM - 0 >= 6) || \
    (defined(_M_ARM) && _M_ARM - 0 >= 6)
#error cmake_ARCH armv6
#elif defined(__ARM_ARCH_5TEJ__) ||                               \
    (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM - 0 >= 5) || \
    (defined(_M_ARM) && _M_ARM - 0 >= 5)
#error cmake_ARCH armv5
#else
#error cmake_ARCH arm
#endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#error cmake_ARCH i386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || \
    defined(_M_X64)
#error cmake_ARCH x86_64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#error cmake_ARCH ia64
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) ||   \
    defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC) || \
    defined(_M_MPPC) || defined(_M_PPC)
#if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#error cmake_ARCH ppc64
#else
#error cmake_ARCH ppc
#endif
#endif
#error cmake_ARCH unknown
