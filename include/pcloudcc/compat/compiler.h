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

#ifndef PCLOUDCC_COMPAT_COMPILER_H_
#define PCLOUDCC_COMPAT_COMPILER_H_

/*! \file compiler.h
 *  \brief Compilers compatibility constants.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Evaluates to 1 if \a attr is a supported attribute.
 *
 * The special operator `__has_attribute` may be used in `#if` and `#elif`
 * expressions to test whether the expression referenced by its \a attr
 * is recognized by the compiler. If compiler doesn't have `__has_attribute`
 * support, the value of zero will be used.
 */
#ifndef __has_attribute
#define __has_attribute(attr) 0
#endif

/*! \brief Evaluates to 1 if \a sym is a supported builtin.
 *
 * The special operator `__has_builtin` may be used in constant integer
 * contexts and in preprocessor ‘#if’ and ‘#elif’ expressions to test whether
 * the symbol named by its \a sym is recognized as a built-in function by the
 * compiler in the current language and conformance mode. If compiler doesn't
 * have `__has_builtin` support, the value of zero will be used.
 */
#ifndef __has_builtin
#define __has_builtin(sym) 0
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
#else
#define psync_prefetch(expr) ((void)0)
#endif

#if __has_attribute(noreturn)
#define PSYNC_NO_RETURN __attribute__((noreturn))
#else
#define PSYNC_NO_RETURN
#endif

#if __has_attribute(noinline)
#define PSYNC_NOINLINE __attribute__((noinline))
#else
#define PSYNC_NOINLINE
#endif

#if __has_attribute(malloc)
#define PSYNC_MALLOC __attribute__((malloc))
#else
#define PSYNC_MALLOC
#endif

#if __has_attribute(sentinel)
#define PSYNC_SENTINEL __attribute__((sentinel))
#else
#define PSYNC_SENTINEL
#endif

#if __has_attribute(pure)
#define PSYNC_PURE __attribute__((pure))
#else
#define PSYNC_PURE
#endif

#if __has_attribute(const)
#define PSYNC_CONST __attribute__((const))
#else
#define PSYNC_CONST
#endif

#if __has_attribute(cold)
#define PSYNC_COLD __attribute__((cold))
#else
#define PSYNC_COLD
#endif

#if __has_attribute(format)
#define PSYNC_FORMAT(a, b, c) __attribute__((format(a, b, c)))
#else
#define PSYNC_FORMAT(a, b, c)
#endif

#if __has_attribute(nonnull)
#define PSYNC_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define PSYNC_NONNULL(...)
#endif

#if __has_attribute(packed)
#define PSYNC_PACKED_STRUCT struct __attribute__((packed))
#else
#define PSYNC_PACKED_STRUCT struct
#endif

#if __has_attribute(weak)
/*! \brief Functions defined with PSYNC_WEAK export their
 *         symbols weakly (if supported). */
#define PSYNC_WEAK __attribute__((weak))
#else
#define PSYNC_WEAK
#endif

/* TODO: Do we still need this? */
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define inline __inline
#define restrict __restrict
#elif __STDC_VERSION__ != 199901L
#define inline
#define restrict
#endif

#if defined(__clang__)
#define psync_alignof __alignof
#elif defined(__GNUC__)
#define psync_alignof __alignof__
#else
#define psync_alignof(t) \
  offsetof(              \
      struct {           \
        char a;          \
        t b;             \
      },                 \
      b                  \
  )
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PCLOUDCC_COMPAT_COMPILER_H_ */
