/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2011-2018 Robert N. M. Watson
 * All rights reserved.
 * Copyright (c) 2020 John Baldwin
 *
 * Portions of this software were developed by SRI International and
 * the University of Cambridge Computer Laboratory under DARPA/AFRL
 * contract (FA8750-10-C-0237) ("CTSRD"), as part of the DARPA CRASH
 * research programme.
 *
 * Portions of this software were developed by SRI International and
 * the University of Cambridge Computer Laboratory (Department of
 * Computer Science and Technology) under DARPA contract
 * HR0011-18-C-0016 ("ECATS"), as part of the DARPA SSITH research
 * programme.
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

#ifndef _MACHINE_CHERIREG_H_
#define	_MACHINE_CHERIREG_H_

/*
 * CHERI ISA-defined constants for capabilities -- suitable for inclusion from
 * assembly source code.
 *
 * XXXRW: CHERI_UNSEALED is not currently considered part of the perms word,
 * but perhaps it should be.
 */
#define	CHERI_PERM_GLOBAL			(1 << 0)	/* 0x00000001 */
#define	CHERI_PERM_EXECUTE			(1 << 1)	/* 0x00000002 */
#define	CHERI_PERM_LOAD				(1 << 2)	/* 0x00000004 */
#define	CHERI_PERM_STORE			(1 << 3)	/* 0x00000008 */
#define	CHERI_PERM_LOAD_CAP			(1 << 4)	/* 0x00000010 */
#define	CHERI_PERM_STORE_CAP			(1 << 5)	/* 0x00000020 */
#define	CHERI_PERM_STORE_LOCAL_CAP		(1 << 6)	/* 0x00000040 */
#define	CHERI_PERM_SEAL				(1 << 7)	/* 0x00000080 */
#define	CHERI_PERM_CCALL			(1 << 8)	/* 0x00000100 */
#define	CHERI_PERM_UNSEAL			(1 << 9)	/* 0x00000200 */
#define	CHERI_PERM_SYSTEM_REGS			(1 << 10)	/* 0x00000400 */
#define	CHERI_PERM_SET_CID			(1 << 11)	/* 0x00000800 */

/* User-defined permission bits. */
#define	CHERI_PERM_SW0			(1 << 15)	/* 0x00008000 */
#define	CHERI_PERM_SW1			(1 << 16)	/* 0x00010000 */
#define	CHERI_PERM_SW2			(1 << 17)	/* 0x00020000 */
#define	CHERI_PERM_SW3			(1 << 18)	/* 0x00040000 */

/*
 * CHERI_PERMS_SWALL: Mask of all available software-defined permissions
 * CHERI_PERMS_HWALL: Mask of all available hardware-defined permissions
 */
#define	CHERI_PERMS_SWALL						\
	(CHERI_PERM_SW0 | CHERI_PERM_SW1 | CHERI_PERM_SW2 |		\
	CHERI_PERM_SW3)

#define	CHERI_PERMS_HWALL						\
	(CHERI_PERM_GLOBAL | CHERI_PERM_EXECUTE |			\
	CHERI_PERM_LOAD | CHERI_PERM_STORE | CHERI_PERM_LOAD_CAP |	\
	CHERI_PERM_STORE_CAP | CHERI_PERM_STORE_LOCAL_CAP |		\
	CHERI_PERM_SEAL | CHERI_PERM_CCALL | CHERI_PERM_UNSEAL |	\
	CHERI_PERM_SYSTEM_REGS | CHERI_PERM_SET_CID)

/*
 * Basic userspace permission mask; CHERI_PERM_EXECUTE will be added for
 * executable capabilities ($pcc); CHERI_PERM_STORE, CHERI_PERM_STORE_CAP,
 * and CHERI_PERM_STORE_LOCAL_CAP will be added for data permissions ($c0).
 *
 * All user software permissions are included along with
 * CHERI_PERM_SYSCALL.  CHERI_PERM_CHERIABI_VMMAP will be added for
 * permissions returned from mmap().
 */
#define	CHERI_PERMS_USERSPACE						\
	(CHERI_PERM_GLOBAL | CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP |	\
	CHERI_PERM_CCALL | (CHERI_PERMS_SWALL & ~CHERI_PERM_CHERIABI_VMMAP))

#define	CHERI_PERMS_USERSPACE_CODE					\
	(CHERI_PERMS_USERSPACE | CHERI_PERM_EXECUTE)

#define	CHERI_PERMS_USERSPACE_SEALCAP					\
	(CHERI_PERM_GLOBAL | CHERI_PERM_SEAL | CHERI_PERM_UNSEAL)

/*
 * _DATA includes _VMMAP so that we can derive the MMAP cap from it.
 *
 * XXX: We may not want to include VMMAP here and instead only in
 * CHERI_CAP_USER_MMAP_PERMS
 */
#define	CHERI_PERMS_USERSPACE_DATA					\
				(CHERI_PERMS_USERSPACE |		\
				CHERI_PERM_STORE |			\
				CHERI_PERM_STORE_CAP |			\
				CHERI_PERM_STORE_LOCAL_CAP |		\
				CHERI_PERM_CHERIABI_VMMAP)

/*
 * Corresponding permission masks for kernel code and data; these are
 * currently a bit broad, and should be narrowed over time as the kernel
 * becomes more capability-aware.
 */
#define	CHERI_PERMS_KERNEL						\
	(CHERI_PERM_GLOBAL | CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP)	\

#define	CHERI_PERMS_KERNEL_CODE						\
	(CHERI_PERMS_KERNEL | CHERI_PERM_EXECUTE | CHERI_PERM_SYSTEM_REGS)

#define	CHERI_PERMS_KERNEL_DATA				       		\
	(CHERI_PERMS_KERNEL | CHERI_PERM_STORE | CHERI_PERM_STORE_CAP | \
	CHERI_PERM_STORE_LOCAL_CAP)

#define	CHERI_PERMS_KERNEL_SEALCAP					\
	(CHERI_PERM_GLOBAL | CHERI_PERM_SEAL | CHERI_PERM_UNSEAL)

/*
 * CHERI_BASELEN_BITS is used in cheritest_cheriabi.c.  The others are
 * unused.
 */
#define	CHERI_BASELEN_BITS	10
#define	CHERI_SEAL_BASELEN_BITS	5
#define	CHERI_ADDR_BITS		64
#define	CHERI_SEAL_MIN_ALIGN	12

#endif /* !_MACHINE_CHERIREG_H_ */
