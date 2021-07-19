/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 Anton Titov <anton@pcloud.com>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUDCC_PSYNC_COMPILER_H_
#define PCLOUDCC_PSYNC_COMPILER_H_

#if defined(_MSC_VER)
#include <mmintrin.h>
#endif

#if !defined(__has_attribute)
#if defined(__GNUC__)
#define __has_attribute(x) 1
#else
#define __has_attribute(x) 0
#endif
#else
#if defined(__GNUC__) && !__has_attribute(malloc)
#undef __has_attribute
#define __has_attribute(x) 1
#endif
#endif

#ifndef __has_builtin
#if defined(__GNUC__)
#define __has_builtin(x) 1
#else
#define __has_builtin(x) 0
#endif
#endif

#if __has_builtin(__builtin_expect)
#define likely(expr) __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif

#if __has_builtin(__builtin_prefetch)
#define psync_prefetch(expr) __builtin_prefetch(expr)
#elif defined(_MSC_VER)
#define psync_prefetch(expr) _mm_prefetch((char *)(expr), _MM_HINT_T0)
#else
#define psync_prefetch(expr) ((void)0)
#endif

#if defined(_MSC_VER)
# define PSYNC_THREAD   __declspec(thread)
# define PSYNC_NOINLINE __declspec(noinline)
#else
#if __has_attribute(noinline)
# define PSYNC_NOINLINE __attribute__((noinline))
#else
# define PSYNC_NOINLINE
#endif
# define PSYNC_THREAD __thread
#endif

#if __has_attribute(malloc)
#define PSYNC_MALLOC __attribute__((malloc))
#else
#define PSYNC_MALLOC
#endif

#if __has_attribute(sentinel)
#define PSYNC_SENTINEL __attribute__ ((sentinel))
#else
#define PSYNC_SENTINEL
#endif

#if __has_attribute(pure)
#define PSYNC_PURE __attribute__ ((pure))
#else
#define PSYNC_PURE
#endif

#if __has_attribute(const)
#define PSYNC_CONST __attribute__ ((const))
#else
#define PSYNC_CONST
#endif

#if __has_attribute(cold)
#define PSYNC_COLD __attribute__ ((cold))
#else
#define PSYNC_COLD
#endif

#if __has_attribute(format)
#define PSYNC_FORMAT(a, b, c) __attribute__ ((format (a, b, c)))
#else
#define PSYNC_FORMAT(a, b, c)
#endif

#if __has_attribute(nonnull)
#define PSYNC_NONNULL(...) __attribute__ ((nonnull (__VA_ARGS__)))
#else
#define PSYNC_NONNULL(...)
#endif

#if __has_attribute(packed)
#define PSYNC_PACKED_STRUCT struct __attribute__ ((packed))
#elif defined(_MSC_VER)
#define PSYNC_PACKED_STRUCT __declspec(align(1)) struct
#else
#define PSYNC_PACKED_STRUCT struct
#endif

#if __has_attribute(weak)
/*! \brief Functions defined with PSYNC_WEAK export their
 *         symbols weakly (if supported). */
#define PSYNC_WEAK __attribute__ ((weak))
#else
#define PSYNC_WEAK
#endif

#if _MSC_VER >= 1500 && _MSC_VER < 1600
#define inline __inline
#define restrict __restrict
#elif __GNUC__ >= 3
#define inline __inline
#define restrict __restrict
#elif __STDC_VERSION__!=199901L
#define inline
#define restrict
#endif

#if defined(__clang__) || defined(_MSC_VER)
#define psync_alignof __alignof
#elif defined(__GNUC__)
#define psync_alignof __alignof__
#else
#define psync_alignof(t) offsetof(struct {char a; t b;}, b)
#endif

#endif /* PCLOUDCC_PSYNC_COMPILER_H_ */
