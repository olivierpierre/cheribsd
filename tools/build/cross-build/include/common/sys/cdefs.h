/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright 2018-2020 Alex Richardson <arichardson@FreeBSD.org>
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory (Department of Computer Science and
 * Technology) under DARPA contract HR0011-18-C-0016 ("ECATS"), as part of the
 * DARPA SSITH research programme.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#pragma once
/* musl libc does not provide a sys/cdefs.h header */
#if __has_include_next(<sys/cdefs.h>)
#include_next <sys/cdefs.h>
#else
/* No sys/cdefs.h header exists so we have to provide some basic macros */
#ifdef  __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#endif

#ifndef __FBSDID
#define __FBSDID(id)
#endif

#ifndef __IDSTRING
#define __IDSTRING(name,string)
#endif

#ifndef rounddown
// needed by libsbuf
#define	rounddown(x, y)	(((x)/(y))*(y))
#define	rounddown2(x, y) ((x)&(~((y)-1)))          /* if y is power of two */
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))  /* to any y */
#define	roundup2(x, y)	(((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#define powerof2(x)	((((x)-1)&(x))==0)
#endif

#ifndef __pure
#define __pure __attribute__((__pure__))
#endif
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif
#ifndef __dead2
#define	__dead2		__attribute__((__noreturn__))
#endif
#ifndef __pure2
#define	__pure2		__attribute__((__const__))
#endif
#ifndef __used
#define	__used		__attribute__((__used__))
#endif
#ifndef __aligned
#define	__aligned(x)	__attribute__((__aligned__(x)))
#endif
#ifndef __section
#define	__section(x)	__attribute__((__section__(x)))
#endif

#ifndef __alloc_size
#define	__alloc_size(...)	__attribute__((__alloc_size__(__VA_ARGS__)))
#endif
#ifndef __alloc_size2
#define	__alloc_size2(n, x)	__attribute__((__alloc_size__(n, x)))
#endif
#ifndef __alloc_align
#define	__alloc_align(x)	__attribute__((__alloc_align__(x)))
#endif
#ifndef __result_use_check
#define	__result_use_check	__attribute__((__warn_unused_result__))
#endif
#ifndef __printflike
#define	__printflike(fmtarg, firstvararg) \
	    __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif
#ifndef __printf0like
#define	__printf0like(fmtarg, firstvararg) \
	    __attribute__((__format__ (__printf0__, fmtarg, firstvararg)))
#endif

#ifndef __predict_true
#define	__predict_true(exp)     __builtin_expect((exp), 1)
#endif
#ifndef __predict_false
#define	__predict_false(exp)    __builtin_expect((exp), 0)
#endif

#ifndef __weak_reference
#ifdef __ELF__
#define	__weak_reference(sym,alias)	\
	__asm__(".weak " #alias);	\
	__asm__(".equ "  #alias ", " #sym)
#else
#define __weak_reference(sym,alias)	\
    static int alias() __attribute__ ((weakref (#sym)));
#endif
#endif

#ifndef __malloc_like
#define	__malloc_like	__attribute__((__malloc__))
#endif

#ifndef nitems
// https://stackoverflow.com/questions/1598773/is-there-a-standard-function-in-c-that-would-return-the-length-of-an-array/1598827#1598827
#define nitems(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif

#ifndef __min_size
#if !defined(__cplusplus)
#define __min_size(x)   static (x)
#else
#define __min_size(x)   (x)
#endif
#endif

#ifndef __unused
#define __unused __attribute__((unused))
#endif
#define __format_arg(fmtarg)    __attribute__((__format_arg__ (fmtarg)))

#ifndef __exported
#define	__exported	__attribute__((__visibility__("default")))
#endif
#ifndef __hidden
#define	__hidden	__attribute__((__visibility__("hidden")))
#endif

#ifndef __unreachable
#define	__unreachable()	__builtin_unreachable()
#endif

#ifndef __clang__
/* GCC doesn't like the printf0 format specifier. Clang treats it the same as
 * printf so add the compatibility macro here. */
#define __printf0__ __printf__
#endif


/*
 * These should probably be in sys/types.h but mtree expects them to exist
 * without including <sys/types.h>
 */
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

/* This is needed so that BSNMP doesn't redeclare an incompatible version */
#define HAVE_STRLCPY 1
/* The compiler supports __func__ */
#define HAVE_DECL___FUNC__ 1

/* On MacOS __CONCAT is defined as x ## y, which won't expand macros */
#undef __CONCAT
#define	__CONCAT1(x,y)	x ## y
#define	__CONCAT(x,y)	__CONCAT1(x,y)

#ifndef __STRING
#define	__STRING(x)	#x		/* stringify without expanding x */
#endif
#ifndef __XSTRING
#define	__XSTRING(x)	__STRING(x)	/* expand x, then stringify */
#endif


#ifndef __has_feature
#define __has_feature(...) 0
#endif

#ifndef __has_builtin
#define __has_builtin(...) 0
#endif

/*
 * Nullability qualifiers: currently only supported by Clang.
 */
#if !(defined(__clang__) && __has_feature(nullability))
#define	_Nonnull
#define	_Nullable
#define	_Null_unspecified
#define	__NULLABILITY_PRAGMA_PUSH
#define	__NULLABILITY_PRAGMA_POP
#else
#define	__NULLABILITY_PRAGMA_PUSH _Pragma("clang diagnostic push")	\
	_Pragma("clang diagnostic ignored \"-Wnullability-completeness\"")
#define	__NULLABILITY_PRAGMA_POP _Pragma("clang diagnostic pop")
#endif

#ifndef __offsetof
#define	__offsetof(type, field)	 __builtin_offsetof(type, field)
#endif

#define	__rangeof(type, start, end) \
	(__offsetof(type, end) - __offsetof(type, start))

#ifndef __containerof
#define	__containerof(x, s, m) ({					\
	const volatile __typeof(((s *)0)->m) *__x = (x);		\
	__DEQUALIFY(s *, (const volatile char *)__x - __offsetof(s, m));\
})
#endif


#ifndef __RCSID
#define __RCSID(x)
#endif
#ifndef __FBSDID
#define __FBSDID(x)
#endif
#ifndef __RCSID
#define __RCSID(x)
#endif
#ifndef __RCSID_SOURCE
#define __RCSID_SOURCE(x)
#endif
#ifndef __SCCSID
#define __SCCSID(x)
#endif
#ifndef __COPYRIGHT
#define __COPYRIGHT(x)
#endif
#ifndef __DECONST
#define __DECONST(type, var)	((type)(__uintptr_t)(const void *)(var))
#endif

#ifndef __DEVOLATILE
#define __DEVOLATILE(type, var)	((type)(__uintptr_t)(volatile void *)(var))
#endif

#ifndef __DEQUALIFY
#define __DEQUALIFY(type, var)	((type)(__uintptr_t)(const volatile void *)(var))
#endif

// CHERI extensions:
#ifndef __no_subobject_bounds
#define __no_subobject_bounds
#endif
