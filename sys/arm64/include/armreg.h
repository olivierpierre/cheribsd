/*-
 * Copyright (c) 2013, 2014 Andrew Turner
 * Copyright (c) 2015 The FreeBSD Foundation
 * All rights reserved.
 *
 * This software was developed by Andrew Turner under
 * sponsorship from the FreeBSD Foundation.
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
 *
 * $FreeBSD$
 */

#ifndef _MACHINE_ARMREG_H_
#define	_MACHINE_ARMREG_H_

#define	INSN_SIZE		4

#define	READ_SPECIALREG(reg)						\
({	uint64_t val;							\
	__asm __volatile("mrs	%0, " __STRING(reg) : "=&r" (val));	\
	val;								\
})
#define	WRITE_SPECIALREG(reg, val)					\
	__asm __volatile("msr	" __STRING(reg) ", %0" : : "r"((uint64_t)val))

/* CNTHCTL_EL2 - Counter-timer Hypervisor Control register */
#define	CNTHCTL_EVNTI_MASK	(0xf << 4) /* Bit to trigger event stream */
#define	CNTHCTL_EVNTDIR		(1 << 3) /* Control transition trigger bit */
#define	CNTHCTL_EVNTEN		(1 << 2) /* Enable event stream */
#define	CNTHCTL_EL1PCEN		(1 << 1) /* Allow EL0/1 physical timer access */
#define	CNTHCTL_EL1PCTEN	(1 << 0) /*Allow EL0/1 physical counter access*/

/* CPACR_EL1 */
#define	CPACR_FPEN_MASK		(0x3 << 20)
#define	 CPACR_FPEN_TRAP_ALL1	(0x0 << 20) /* Traps from EL0 and EL1 */
#define	 CPACR_FPEN_TRAP_EL0	(0x1 << 20) /* Traps from EL0 */
#define	 CPACR_FPEN_TRAP_ALL2	(0x2 << 20) /* Traps from EL0 and EL1 */
#define	 CPACR_FPEN_TRAP_NONE	(0x3 << 20) /* No traps */
#define	CPACR_TTA		(0x1 << 28)

/* CTR_EL0 - Cache Type Register */
#define	CTR_DLINE_SHIFT		16
#define	CTR_DLINE_MASK		(0xf << CTR_DLINE_SHIFT)
#define	CTR_DLINE_SIZE(reg)	(((reg) & CTR_DLINE_MASK) >> CTR_DLINE_SHIFT)
#define	CTR_ILINE_SHIFT		0
#define	CTR_ILINE_MASK		(0xf << CTR_ILINE_SHIFT)
#define	CTR_ILINE_SIZE(reg)	(((reg) & CTR_ILINE_MASK) >> CTR_ILINE_SHIFT)

/* DAIF - Interrupt Mask Bits */
#define	DAIF_D_MASKED		(1 << 9)
#define	DAIF_A_MASKED		(1 << 8)
#define	DAIF_I_MASKED		(1 << 7)
#define	DAIF_F_MASKED		(1 << 6)

/* DCZID_EL0 - Data Cache Zero ID register */
#define DCZID_DZP		(1 << 4) /* DC ZVA prohibited if non-0 */
#define DCZID_BS_SHIFT		0
#define DCZID_BS_MASK		(0xf << DCZID_BS_SHIFT)
#define	DCZID_BS_SIZE(reg)	(((reg) & DCZID_BS_MASK) >> DCZID_BS_SHIFT)

/* ESR_ELx */
#define	ESR_ELx_ISS_MASK	0x00ffffff
#define	 ISS_INSN_FnV		(0x01 << 10)
#define	 ISS_INSN_EA		(0x01 << 9)
#define	 ISS_INSN_S1PTW		(0x01 << 7)
#define	 ISS_INSN_IFSC_MASK	(0x1f << 0)
#define	 ISS_DATA_ISV		(0x01 << 24)
#define	 ISS_DATA_SAS_MASK	(0x03 << 22)
#define	 ISS_DATA_SSE		(0x01 << 21)
#define	 ISS_DATA_SRT_MASK	(0x1f << 16)
#define	 ISS_DATA_SF		(0x01 << 15)
#define	 ISS_DATA_AR		(0x01 << 14)
#define	 ISS_DATA_FnV		(0x01 << 10)
#define	 ISS_DATa_EA		(0x01 << 9)
#define	 ISS_DATa_CM		(0x01 << 8)
#define	 ISS_INSN_S1PTW		(0x01 << 7)
#define	 ISS_DATa_WnR		(0x01 << 6)
#define	 ISS_DATA_DFSC_MASK	(0x3f << 0)
#define	 ISS_DATA_DFSC_ASF_L0	(0x00 << 0)
#define	 ISS_DATA_DFSC_ASF_L1	(0x01 << 0)
#define	 ISS_DATA_DFSC_ASF_L2	(0x02 << 0)
#define	 ISS_DATA_DFSC_ASF_L3	(0x03 << 0)
#define	 ISS_DATA_DFSC_TF_L0	(0x04 << 0)
#define	 ISS_DATA_DFSC_TF_L1	(0x05 << 0)
#define	 ISS_DATA_DFSC_TF_L2	(0x06 << 0)
#define	 ISS_DATA_DFSC_TF_L3	(0x07 << 0)
#define	 ISS_DATA_DFSC_AFF_L1	(0x09 << 0)
#define	 ISS_DATA_DFSC_AFF_L2	(0x0a << 0)
#define	 ISS_DATA_DFSC_AFF_L3	(0x0b << 0)
#define	 ISS_DATA_DFSC_PF_L1	(0x0d << 0)
#define	 ISS_DATA_DFSC_PF_L2	(0x0e << 0)
#define	 ISS_DATA_DFSC_PF_L3	(0x0f << 0)
#define	 ISS_DATA_DFSC_EXT	(0x10 << 0)
#define	 ISS_DATA_DFSC_EXT_L0	(0x14 << 0)
#define	 ISS_DATA_DFSC_EXT_L1	(0x15 << 0)
#define	 ISS_DATA_DFSC_EXT_L2	(0x16 << 0)
#define	 ISS_DATA_DFSC_EXT_L3	(0x17 << 0)
#define	 ISS_DATA_DFSC_ECC	(0x18 << 0)
#define	 ISS_DATA_DFSC_ECC_L0	(0x1c << 0)
#define	 ISS_DATA_DFSC_ECC_L1	(0x1d << 0)
#define	 ISS_DATA_DFSC_ECC_L2	(0x1e << 0)
#define	 ISS_DATA_DFSC_ECC_L3	(0x1f << 0)
#define	 ISS_DATA_DFSC_ALIGN	(0x21 << 0)
#define	 ISS_DATA_DFSC_TLB_CONFLICT (0x30 << 0)
#define	ESR_ELx_IL		(0x01 << 25)
#define	ESR_ELx_EC_SHIFT	26
#define	ESR_ELx_EC_MASK		(0x3f << 26)
#define	ESR_ELx_EXCEPTION(esr)	(((esr) & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)
#define	 EXCP_UNKNOWN		0x00	/* Unkwn exception */
#define	 EXCP_FP_SIMD		0x07	/* VFP/SIMD trap */
#define	 EXCP_ILL_STATE		0x0e	/* Illegal execution state */
#define	 EXCP_SVC32		0x11	/* SVC trap for AArch32 */
#define	 EXCP_SVC64		0x15	/* SVC trap for AArch64 */
#define	 EXCP_MSR		0x18	/* MSR/MRS trap */
#define	 EXCP_INSN_ABORT_L	0x20	/* Instruction abort, from lower EL */
#define	 EXCP_INSN_ABORT	0x21	/* Instruction abort, from same EL */ 
#define	 EXCP_PC_ALIGN		0x22	/* PC alignment fault */
#define	 EXCP_DATA_ABORT_L	0x24	/* Data abort, from lower EL */
#define	 EXCP_DATA_ABORT	0x25	/* Data abort, from same EL */ 
#define	 EXCP_SP_ALIGN		0x26	/* SP slignment fault */
#define	 EXCP_TRAP_FP		0x2c	/* Trapped FP exception */
#define	 EXCP_SERROR		0x2f	/* SError interrupt */
#define	 EXCP_SOFTSTP_EL0	0x32	/* Software Step, from lower EL */
#define	 EXCP_SOFTSTP_EL1	0x33	/* Software Step, from same EL */
#define	 EXCP_WATCHPT_EL1	0x35	/* Watchpoint, from same EL */
#define	 EXCP_BRK		0x3c	/* Breakpoint */

/* ICC_CTLR_EL1 */
#define	ICC_CTLR_EL1_EOIMODE	(1U << 1)

/* ICC_IAR1_EL1 */
#define	ICC_IAR1_EL1_SPUR	(0x03ff)

/* ICC_IGRPEN0_EL1 */
#define	ICC_IGRPEN0_EL1_EN	(1U << 0)

/* ICC_PMR_EL1 */
#define	ICC_PMR_EL1_PRIO_MASK	(0xFFUL)

/* ICC_SGI1R_EL1 */
#define	ICC_SGI1R_EL1_TL_MASK		0xffffUL
#define	ICC_SGI1R_EL1_AFF1_SHIFT	16
#define	ICC_SGI1R_EL1_SGIID_SHIFT	24
#define	ICC_SGI1R_EL1_AFF2_SHIFT	32
#define	ICC_SGI1R_EL1_AFF3_SHIFT	48
#define	ICC_SGI1R_EL1_SGIID_MASK	0xfUL
#define	ICC_SGI1R_EL1_IRM		(0x1UL << 40)

/* ICC_SRE_EL1 */
#define	ICC_SRE_EL1_SRE		(1U << 0)

/* ICC_SRE_EL2 */
#define	ICC_SRE_EL2_SRE		(1U << 0)
#define	ICC_SRE_EL2_EN		(1U << 3)

/* ID_AA64DFR0_EL1 */
#define	ID_AA64DFR0_MASK		0x0000000ff0f0fffful
#define	ID_AA64DFR0_DEBUG_VER_SHIFT	0
#define	ID_AA64DFR0_DEBUG_VER_MASK	(0xf << ID_AA64DFR0_DEBUG_VER_SHIFT)
#define	ID_AA64DFR0_DEBUG_VER(x)	((x) & ID_AA64DFR0_DEBUG_VER_MASK)
#define	 ID_AA64DFR0_DEBUG_VER_8	(0x6 << ID_AA64DFR0_DEBUG_VER_SHIFT)
#define	 ID_AA64DFR0_DEBUG_VER_8_VHE	(0x7 << ID_AA64DFR0_DEBUG_VER_SHIFT)
#define	 ID_AA64DFR0_DEBUG_VER_8_2	(0x8 << ID_AA64DFR0_DEBUG_VER_SHIFT)
#define	ID_AA64DFR0_TRACE_VER_SHIFT	4
#define	ID_AA64DFR0_TRACE_VER_MASK	(0xf << ID_AA64DFR0_TRACE_VER_SHIFT)
#define	ID_AA64DFR0_TRACE_VER(x)	((x) & ID_AA64DFR0_TRACE_VER_MASK)
#define	 ID_AA64DFR0_TRACE_VER_NONE	(0x0 << ID_AA64DFR0_TRACE_VER_SHIFT)
#define	 ID_AA64DFR0_TRACE_VER_IMPL	(0x1 << ID_AA64DFR0_TRACE_VER_SHIFT)
#define	ID_AA64DFR0_PMU_VER_SHIFT	8
#define	ID_AA64DFR0_PMU_VER_MASK	(0xf << ID_AA64DFR0_PMU_VER_SHIFT)
#define	ID_AA64DFR0_PMU_VER(x)		((x) & ID_AA64DFR0_PMU_VER_MASK)
#define	 ID_AA64DFR0_PMU_VER_NONE	(0x0 << ID_AA64DFR0_PMU_VER_SHIFT)
#define	 ID_AA64DFR0_PMU_VER_3		(0x1 << ID_AA64DFR0_PMU_VER_SHIFT)
#define	 ID_AA64DFR0_PMU_VER_3_1	(0x4 << ID_AA64DFR0_PMU_VER_SHIFT)
#define	 ID_AA64DFR0_PMU_VER_IMPL	(0xf << ID_AA64DFR0_PMU_VER_SHIFT)
#define	ID_AA64DFR0_BRPS_SHIFT		12
#define	ID_AA64DFR0_BRPS_MASK		(0xf << ID_AA64DFR0_BRPS_SHIFT)
#define	ID_AA64DFR0_BRPS(x)		\
    ((((x) >> ID_AA64DFR0_BRPS_SHIFT) & 0xf) + 1)
#define	ID_AA64DFR0_WRPS_SHIFT		20
#define	ID_AA64DFR0_WRPS_MASK		(0xf << ID_AA64DFR0_WRPS_SHIFT)
#define	ID_AA64DFR0_WRPS(x)		\
    ((((x) >> ID_AA64DFR0_WRPS_SHIFT) & 0xf) + 1)
#define	ID_AA64DFR0_CTX_CMPS_SHIFT	28
#define	ID_AA64DFR0_CTX_CMPS_MASK	(0xf << ID_AA64DFR0_CTX_CMPS_SHIFT)
#define	ID_AA64DFR0_CTX_CMPS(x)		\
    ((((x) >> ID_AA64DFR0_CTX_CMPS_SHIFT) & 0xf) + 1)
#define	ID_AA64DFR0_PMS_VER_SHIFT	32
#define	ID_AA64DFR0_PMS_VER_MASK	(0xful << ID_AA64DFR0_PMS_VER_SHIFT)
#define	ID_AA64DFR0_PMS_VER(x)	((x) & ID_AA64DFR0_PMS_VER_MASK)
#define	 ID_AA64DFR0_PMS_VER_NONE	(0x0ul << ID_AA64DFR0_PMS_VER_SHIFT)
#define	 ID_AA64DFR0_PMS_VER_V1		(0x1ul << ID_AA64DFR0_PMS_VER_SHIFT)

/* ID_AA64ISAR0_EL1 */
#define	ID_AA64ISAR0_MASK		0x0000fffff0fffff0ul
#define	ID_AA64ISAR0_AES_SHIFT		4
#define	ID_AA64ISAR0_AES_MASK		(0xf << ID_AA64ISAR0_AES_SHIFT)
#define	ID_AA64ISAR0_AES(x)		((x) & ID_AA64ISAR0_AES_MASK)
#define	 ID_AA64ISAR0_AES_NONE		(0x0 << ID_AA64ISAR0_AES_SHIFT)
#define	 ID_AA64ISAR0_AES_BASE		(0x1 << ID_AA64ISAR0_AES_SHIFT)
#define	 ID_AA64ISAR0_AES_PMULL		(0x2 << ID_AA64ISAR0_AES_SHIFT)
#define	ID_AA64ISAR0_SHA1_SHIFT		8
#define	ID_AA64ISAR0_SHA1_MASK		(0xf << ID_AA64ISAR0_SHA1_SHIFT)
#define	ID_AA64ISAR0_SHA1(x)		((x) & ID_AA64ISAR0_SHA1_MASK)
#define	 ID_AA64ISAR0_SHA1_NONE		(0x0 << ID_AA64ISAR0_SHA1_SHIFT)
#define	 ID_AA64ISAR0_SHA1_BASE		(0x1 << ID_AA64ISAR0_SHA1_SHIFT)
#define	ID_AA64ISAR0_SHA2_SHIFT		12
#define	ID_AA64ISAR0_SHA2_MASK		(0xf << ID_AA64ISAR0_SHA2_SHIFT)
#define	ID_AA64ISAR0_SHA2(x)		((x) & ID_AA64ISAR0_SHA2_MASK)
#define	 ID_AA64ISAR0_SHA2_NONE		(0x0 << ID_AA64ISAR0_SHA2_SHIFT)
#define	 ID_AA64ISAR0_SHA2_BASE		(0x1 << ID_AA64ISAR0_SHA2_SHIFT)
#define	 ID_AA64ISAR0_SHA2_512		(0x2 << ID_AA64ISAR0_SHA2_SHIFT)
#define	ID_AA64ISAR0_CRC32_SHIFT	16
#define	ID_AA64ISAR0_CRC32_MASK		(0xf << ID_AA64ISAR0_CRC32_SHIFT)
#define	ID_AA64ISAR0_CRC32(x)		((x) & ID_AA64ISAR0_CRC32_MASK)
#define	 ID_AA64ISAR0_CRC32_NONE	(0x0 << ID_AA64ISAR0_CRC32_SHIFT)
#define	 ID_AA64ISAR0_CRC32_BASE	(0x1 << ID_AA64ISAR0_CRC32_SHIFT)
#define	ID_AA64ISAR0_ATOMIC_SHIFT	20
#define	ID_AA64ISAR0_ATOMIC_MASK	(0xf << ID_AA64ISAR0_ATOMIC_SHIFT)
#define	ID_AA64ISAR0_ATOMIC(x)		((x) & ID_AA64ISAR0_ATOMIC_MASK)
#define	 ID_AA64ISAR0_ATOMIC_NONE	(0x0 << ID_AA64ISAR0_ATOMIC_SHIFT)
#define	 ID_AA64ISAR0_ATOMIC_IMPL	(0x2 << ID_AA64ISAR0_ATOMIC_SHIFT)
#define	ID_AA64ISAR0_RDM_SHIFT		28
#define	ID_AA64ISAR0_RDM_MASK		(0xf << ID_AA64ISAR0_RDM_SHIFT)
#define	ID_AA64ISAR0_RDM(x)		((x) & ID_AA64ISAR0_RDM_MASK)
#define	 ID_AA64ISAR0_RDM_NONE		(0x0 << ID_AA64ISAR0_RDM_SHIFT)
#define	 ID_AA64ISAR0_RDM_IMPL		(0x1 << ID_AA64ISAR0_RDM_SHIFT)
#define	ID_AA64ISAR0_SHA3_SHIFT		32
#define	ID_AA64ISAR0_SHA3_MASK		(0xful << ID_AA64ISAR0_SHA3_SHIFT)
#define	ID_AA64ISAR0_SHA3(x)		((x) & ID_AA64ISAR0_SHA3_MASK)
#define	 ID_AA64ISAR0_SHA3_NONE		(0x0ul << ID_AA64ISAR0_SHA3_SHIFT)
#define	 ID_AA64ISAR0_SHA3_IMPL		(0x1ul << ID_AA64ISAR0_SHA3_SHIFT)
#define	ID_AA64ISAR0_SM3_SHIFT		36
#define	ID_AA64ISAR0_SM3_MASK		(0xful << ID_AA64ISAR0_SM3_SHIFT)
#define	ID_AA64ISAR0_SM3(x)		((x) & ID_AA64ISAR0_SM3_MASK)
#define	 ID_AA64ISAR0_SM3_NONE		(0x0ul << ID_AA64ISAR0_SM3_SHIFT)
#define	 ID_AA64ISAR0_SM3_IMPL		(0x1ul << ID_AA64ISAR0_SM3_SHIFT)
#define	ID_AA64ISAR0_SM4_SHIFT		40
#define	ID_AA64ISAR0_SM4_MASK		(0xful << ID_AA64ISAR0_SM4_SHIFT)
#define	ID_AA64ISAR0_SM4(x)		((x) & ID_AA64ISAR0_SM4_MASK)
#define	 ID_AA64ISAR0_SM4_NONE		(0x0ul << ID_AA64ISAR0_SM4_SHIFT)
#define	 ID_AA64ISAR0_SM4_IMPL		(0x1ul << ID_AA64ISAR0_SM4_SHIFT)
#define	ID_AA64ISAR0_DP_SHIFT		44
#define	ID_AA64ISAR0_DP_MASK		(0xful << ID_AA64ISAR0_DP_SHIFT)
#define	ID_AA64ISAR0_DP(x)		((x) & ID_AA64ISAR0_DP_MASK)
#define	 ID_AA64ISAR0_DP_NONE		(0x0ul << ID_AA64ISAR0_DP_SHIFT)
#define	 ID_AA64ISAR0_DP_IMPL		(0x1ul << ID_AA64ISAR0_DP_SHIFT)

/* ID_AA64ISAR1_EL1 */
#define	ID_AA64ISAR1_MASK		0xffffffff
#define	ID_AA64ISAR1_DPB_SHIFT		0
#define	ID_AA64ISAR1_DPB_MASK		(0xf << ID_AA64ISAR1_DPB_SHIFT)
#define	ID_AA64ISAR1_DPB(x)		((x) & ID_AA64ISAR1_DPB_MASK)
#define	 ID_AA64ISAR1_DPB_NONE		(0x0 << ID_AA64ISAR1_DPB_SHIFT)
#define	 ID_AA64ISAR1_DPB_IMPL		(0x1 << ID_AA64ISAR1_DPB_SHIFT)
#define	ID_AA64ISAR1_APA_SHIFT		4
#define	ID_AA64ISAR1_APA_MASK		(0xf << ID_AA64ISAR1_APA_SHIFT)
#define	ID_AA64ISAR1_APA(x)		((x) & ID_AA64ISAR1_APA_MASK)
#define	 ID_AA64ISAR1_APA_NONE		(0x0 << ID_AA64ISAR1_APA_SHIFT)
#define	 ID_AA64ISAR1_APA_IMPL		(0x1 << ID_AA64ISAR1_APA_SHIFT)
#define	ID_AA64ISAR1_API_SHIFT		8
#define	ID_AA64ISAR1_API_MASK		(0xf << ID_AA64ISAR1_API_SHIFT)
#define	ID_AA64ISAR1_API(x)		((x) & ID_AA64ISAR1_API_MASK)
#define	 ID_AA64ISAR1_API_NONE		(0x0 << ID_AA64ISAR1_API_SHIFT)
#define	 ID_AA64ISAR1_API_IMPL		(0x1 << ID_AA64ISAR1_API_SHIFT)
#define	ID_AA64ISAR1_JSCVT_SHIFT	12
#define	ID_AA64ISAR1_JSCVT_MASK		(0xf << ID_AA64ISAR1_JSCVT_SHIFT)
#define	ID_AA64ISAR1_JSCVT(x)		((x) & ID_AA64ISAR1_JSCVT_MASK)
#define	 ID_AA64ISAR1_JSCVT_NONE	(0x0 << ID_AA64ISAR1_JSCVT_SHIFT)
#define	 ID_AA64ISAR1_JSCVT_IMPL	(0x1 << ID_AA64ISAR1_JSCVT_SHIFT)
#define	ID_AA64ISAR1_FCMA_SHIFT		16
#define	ID_AA64ISAR1_FCMA_MASK		(0xf << ID_AA64ISAR1_FCMA_SHIFT)
#define	ID_AA64ISAR1_FCMA(x)		((x) & ID_AA64ISAR1_FCMA_MASK)
#define	 ID_AA64ISAR1_FCMA_NONE		(0x0 << ID_AA64ISAR1_FCMA_SHIFT)
#define	 ID_AA64ISAR1_FCMA_IMPL		(0x1 << ID_AA64ISAR1_FCMA_SHIFT)
#define	ID_AA64ISAR1_LRCPC_SHIFT	20
#define	ID_AA64ISAR1_LRCPC_MASK		(0xf << ID_AA64ISAR1_LRCPC_SHIFT)
#define	ID_AA64ISAR1_LRCPC(x)		((x) & ID_AA64ISAR1_LRCPC_MASK)
#define	 ID_AA64ISAR1_LRCPC_NONE	(0x0 << ID_AA64ISAR1_LRCPC_SHIFT)
#define	 ID_AA64ISAR1_LRCPC_IMPL	(0x1 << ID_AA64ISAR1_LRCPC_SHIFT)
#define	ID_AA64ISAR1_GPA_SHIFT		24
#define	ID_AA64ISAR1_GPA_MASK		(0xf << ID_AA64ISAR1_GPA_SHIFT)
#define	ID_AA64ISAR1_GPA(x)		((x) & ID_AA64ISAR1_GPA_MASK)
#define	 ID_AA64ISAR1_GPA_NONE		(0x0 << ID_AA64ISAR1_GPA_SHIFT)
#define	 ID_AA64ISAR1_GPA_IMPL		(0x1 << ID_AA64ISAR1_GPA_SHIFT)
#define	ID_AA64ISAR1_GPI_SHIFT		28
#define	ID_AA64ISAR1_GPI_MASK		(0xf << ID_AA64ISAR1_GPI_SHIFT)
#define	ID_AA64ISAR1_GPI(x)		((x) & ID_AA64ISAR1_GPI_MASK)
#define	 ID_AA64ISAR1_GPI_NONE		(0x0 << ID_AA64ISAR1_GPI_SHIFT)
#define	 ID_AA64ISAR1_GPI_IMPL		(0x1 << ID_AA64ISAR1_GPI_SHIFT)

/* ID_AA64MMFR0_EL1 */
#define	ID_AA64MMFR0_MASK		0xffffffff
#define	ID_AA64MMFR0_PA_RANGE_SHIFT	0
#define	ID_AA64MMFR0_PA_RANGE_MASK	(0xf << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	ID_AA64MMFR0_PA_RANGE(x)	((x) & ID_AA64MMFR0_PA_RANGE_MASK)
#define	 ID_AA64MMFR0_PA_RANGE_4G	(0x0 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_64G	(0x1 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_1T	(0x2 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_4T	(0x3 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_16T	(0x4 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_256T	(0x5 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	 ID_AA64MMFR0_PA_RANGE_4P	(0x6 << ID_AA64MMFR0_PA_RANGE_SHIFT)
#define	ID_AA64MMFR0_ASID_BITS_SHIFT	4
#define	ID_AA64MMFR0_ASID_BITS_MASK	(0xf << ID_AA64MMFR0_ASID_BITS_SHIFT)
#define	ID_AA64MMFR0_ASID_BITS(x)	((x) & ID_AA64MMFR0_ASID_BITS_MASK)
#define	 ID_AA64MMFR0_ASID_BITS_8	(0x0 << ID_AA64MMFR0_ASID_BITS_SHIFT)
#define	 ID_AA64MMFR0_ASID_BITS_16	(0x2 << ID_AA64MMFR0_ASID_BITS_SHIFT)
#define	ID_AA64MMFR0_BIGEND_SHIFT	8
#define	ID_AA64MMFR0_BIGEND_MASK	(0xf << ID_AA64MMFR0_BIGEND_SHIFT)
#define	ID_AA64MMFR0_BIGEND(x)		((x) & ID_AA64MMFR0_BIGEND_MASK)
#define	 ID_AA64MMFR0_BIGEND_FIXED	(0x0 << ID_AA64MMFR0_BIGEND_SHIFT)
#define	 ID_AA64MMFR0_BIGEND_MIXED	(0x1 << ID_AA64MMFR0_BIGEND_SHIFT)
#define	ID_AA64MMFR0_S_NS_MEM_SHIFT	12
#define	ID_AA64MMFR0_S_NS_MEM_MASK	(0xf << ID_AA64MMFR0_S_NS_MEM_SHIFT)
#define	ID_AA64MMFR0_S_NS_MEM(x)	((x) & ID_AA64MMFR0_S_NS_MEM_MASK)
#define	 ID_AA64MMFR0_S_NS_MEM_NONE	(0x0 << ID_AA64MMFR0_S_NS_MEM_SHIFT)
#define	 ID_AA64MMFR0_S_NS_MEM_DISTINCT	(0x1 << ID_AA64MMFR0_S_NS_MEM_SHIFT)
#define	ID_AA64MMFR0_BIGEND_EL0_SHIFT	16
#define	ID_AA64MMFR0_BIGEND_EL0_MASK	(0xf << ID_AA64MMFR0_BIGEND_EL0_SHIFT)
#define	ID_AA64MMFR0_BIGEND_EL0(x)	((x) & ID_AA64MMFR0_BIGEND_EL0_MASK)
#define	 ID_AA64MMFR0_BIGEND_EL0_FIXED	(0x0 << ID_AA64MMFR0_BIGEND_EL0_SHIFT)
#define	 ID_AA64MMFR0_BIGEND_EL0_MIXED	(0x1 << ID_AA64MMFR0_BIGEND_EL0_SHIFT)
#define	ID_AA64MMFR0_TGRAN16_SHIFT	20
#define	ID_AA64MMFR0_TGRAN16_MASK	(0xf << ID_AA64MMFR0_TGRAN16_SHIFT)
#define	ID_AA64MMFR0_TGRAN16(x)		((x) & ID_AA64MMFR0_TGRAN16_MASK)
#define	 ID_AA64MMFR0_TGRAN16_NONE	(0x0 << ID_AA64MMFR0_TGRAN16_SHIFT)
#define	 ID_AA64MMFR0_TGRAN16_IMPL	(0x1 << ID_AA64MMFR0_TGRAN16_SHIFT)
#define	ID_AA64MMFR0_TGRAN64_SHIFT	24
#define	ID_AA64MMFR0_TGRAN64_MASK	(0xf << ID_AA64MMFR0_TGRAN64_SHIFT)
#define	ID_AA64MMFR0_TGRAN64(x)		((x) & ID_AA64MMFR0_TGRAN64_MASK)
#define	 ID_AA64MMFR0_TGRAN64_IMPL	(0x0 << ID_AA64MMFR0_TGRAN64_SHIFT)
#define	 ID_AA64MMFR0_TGRAN64_NONE	(0xf << ID_AA64MMFR0_TGRAN64_SHIFT)
#define	ID_AA64MMFR0_TGRAN4_SHIFT	28
#define	ID_AA64MMFR0_TGRAN4_MASK	(0xf << ID_AA64MMFR0_TGRAN4_SHIFT)
#define	ID_AA64MMFR0_TGRAN4(x)		((x) & ID_AA64MMFR0_TGRAN4_MASK)
#define	 ID_AA64MMFR0_TGRAN4_IMPL	(0x0 << ID_AA64MMFR0_TGRAN4_SHIFT)
#define	 ID_AA64MMFR0_TGRAN4_NONE	(0xf << ID_AA64MMFR0_TGRAN4_SHIFT)

/* ID_AA64MMFR1_EL1 */
#define	ID_AA64MMFR1_MASK		0xffffffff
#define	ID_AA64MMFR1_HAFDBS_SHIFT	0
#define	ID_AA64MMFR1_HAFDBS_MASK	(0xf << ID_AA64MMFR1_HAFDBS_SHIFT)
#define	ID_AA64MMFR1_HAFDBS(x)		((x) & ID_AA64MMFR1_HAFDBS_MASK)
#define	 ID_AA64MMFR1_HAFDBS_NONE	(0x0 << ID_AA64MMFR1_HAFDBS_SHIFT)
#define	 ID_AA64MMFR1_HAFDBS_AF		(0x1 << ID_AA64MMFR1_HAFDBS_SHIFT)
#define	 ID_AA64MMFR1_HAFDBS_AF_DBS	(0x2 << ID_AA64MMFR1_HAFDBS_SHIFT)
#define	ID_AA64MMFR1_VMIDBITS_SHIFT	4
#define	ID_AA64MMFR1_VMIDBITS_MASK	(0xf << ID_AA64MMFR1_VMIDBITS_SHIFT)
#define	ID_AA64MMFR1_VMIDBITS(x)	((x) & ID_AA64MMFR1_VMIDBITS_MASK)
#define	 ID_AA64MMFR1_VMIDBITS_8	(0x0 << ID_AA64MMFR1_VMIDBITS_SHIFT)
#define	 ID_AA64MMFR1_VMIDBITS_16	(0x2 << ID_AA64MMFR1_VMIDBITS_SHIFT)
#define	ID_AA64MMFR1_VH_SHIFT		8
#define	ID_AA64MMFR1_VH_MASK		(0xf << ID_AA64MMFR1_VH_SHIFT)
#define	ID_AA64MMFR1_VH(x)		((x) & ID_AA64MMFR1_VH_MASK)
#define	 ID_AA64MMFR1_VH_NONE		(0x0 << ID_AA64MMFR1_VH_SHIFT)
#define	 ID_AA64MMFR1_VH_IMPL		(0x1 << ID_AA64MMFR1_VH_SHIFT)
#define	ID_AA64MMFR1_HPDS_SHIFT		12
#define	ID_AA64MMFR1_HPDS_MASK		(0xf << ID_AA64MMFR1_HPDS_SHIFT)
#define	ID_AA64MMFR1_HPDS(x)		((x) & ID_AA64MMFR1_HPDS_MASK)
#define	 ID_AA64MMFR1_HPDS_NONE		(0x0 << ID_AA64MMFR1_HPDS_SHIFT)
#define	 ID_AA64MMFR1_HPDS_HPD		(0x1 << ID_AA64MMFR1_HPDS_SHIFT)
#define	 ID_AA64MMFR1_HPDS_TTPBHA	(0x2 << ID_AA64MMFR1_HPDS_SHIFT)
#define	ID_AA64MMFR1_LO_SHIFT		16
#define	ID_AA64MMFR1_LO_MASK		(0xf << ID_AA64MMFR1_LO_SHIFT)
#define	ID_AA64MMFR1_LO(x)		((x) & ID_AA64MMFR1_LO_MASK)
#define	 ID_AA64MMFR1_LO_NONE		(0x0 << ID_AA64MMFR1_LO_SHIFT)
#define	 ID_AA64MMFR1_LO_IMPL		(0x1 << ID_AA64MMFR1_LO_SHIFT)
#define	ID_AA64MMFR1_PAN_SHIFT		20
#define	ID_AA64MMFR1_PAN_MASK		(0xf << ID_AA64MMFR1_PAN_SHIFT)
#define	ID_AA64MMFR1_PAN(x)		((x) & ID_AA64MMFR1_PAN_MASK)
#define	 ID_AA64MMFR1_PAN_NONE		(0x0 << ID_AA64MMFR1_PAN_SHIFT)
#define	 ID_AA64MMFR1_PAN_IMPL		(0x1 << ID_AA64MMFR1_PAN_SHIFT)
#define	 ID_AA64MMFR1_PAN_ATS1E1	(0x2 << ID_AA64MMFR1_PAN_SHIFT)
#define	ID_AA64MMFR1_SPEC_SEI_SHIFT	24
#define	ID_AA64MMFR1_SPEC_SEI_MASK	(0xf << ID_AA64MMFR1_SPEC_SEI_SHIFT)
#define	ID_AA64MMFR1_SPEC_SEI(x)	((x) & ID_AA64MMFR1_SPEC_SEI_MASK)
#define	 ID_AA64MMFR1_SPEC_SEI_NONE	(0x0 << ID_AA64MMFR1_SPEC_SEI_SHIFT)
#define	 ID_AA64MMFR1_SPEC_SEI_IMPL	(0x1 << ID_AA64MMFR1_SPEC_SEI_SHIFT)
#define	ID_AA64MMFR1_XNX_SHIFT		28
#define	ID_AA64MMFR1_XNX_MASK		(0xf << ID_AA64MMFR1_XNX_SHIFT)
#define	ID_AA64MMFR1_XNX(x)		((x) & ID_AA64MMFR1_XNX_MASK)
#define	 ID_AA64MMFR1_XNX_NONE		(0x0 << ID_AA64MMFR1_XNX_SHIFT)
#define	 ID_AA64MMFR1_XNX_IMPL		(0x1 << ID_AA64MMFR1_XNX_SHIFT)

/* ID_AA64MMFR2_EL1 */
#define	ID_AA64MMFR2_EL1		S3_0_C0_C7_2
#define	ID_AA64MMFR2_MASK		0x0fffffff
#define	ID_AA64MMFR2_CNP_SHIFT		0
#define	ID_AA64MMFR2_CNP_MASK		(0xf << ID_AA64MMFR2_CNP_SHIFT)
#define	ID_AA64MMFR2_CNP(x)		((x) & ID_AA64MMFR2_CNP_MASK)
#define	 ID_AA64MMFR2_CNP_NONE		(0x0 << ID_AA64MMFR2_CNP_SHIFT)
#define	 ID_AA64MMFR2_CNP_IMPL		(0x1 << ID_AA64MMFR2_CNP_SHIFT)
#define	ID_AA64MMFR2_UAO_SHIFT		4
#define	ID_AA64MMFR2_UAO_MASK		(0xf << ID_AA64MMFR2_UAO_SHIFT)
#define	ID_AA64MMFR2_UAO(x)		((x) & ID_AA64MMFR2_UAO_MASK)
#define	 ID_AA64MMFR2_UAO_NONE		(0x0 << ID_AA64MMFR2_UAO_SHIFT)
#define	 ID_AA64MMFR2_UAO_IMPL		(0x1 << ID_AA64MMFR2_UAO_SHIFT)
#define	ID_AA64MMFR2_LSM_SHIFT		8
#define	ID_AA64MMFR2_LSM_MASK		(0xf << ID_AA64MMFR2_LSM_SHIFT)
#define	ID_AA64MMFR2_LSM(x)		((x) & ID_AA64MMFR2_LSM_MASK)
#define	 ID_AA64MMFR2_LSM_NONE		(0x0 << ID_AA64MMFR2_LSM_SHIFT)
#define	 ID_AA64MMFR2_LSM_IMPL		(0x1 << ID_AA64MMFR2_LSM_SHIFT)
#define	ID_AA64MMFR2_IESB_SHIFT		12
#define	ID_AA64MMFR2_IESB_MASK		(0xf << ID_AA64MMFR2_IESB_SHIFT)
#define	ID_AA64MMFR2_IESB(x)		((x) & ID_AA64MMFR2_IESB_MASK)
#define	 ID_AA64MMFR2_IESB_NONE		(0x0 << ID_AA64MMFR2_IESB_SHIFT)
#define	 ID_AA64MMFR2_IESB_IMPL		(0x1 << ID_AA64MMFR2_IESB_SHIFT)
#define	ID_AA64MMFR2_VA_RANGE_SHIFT	16
#define	ID_AA64MMFR2_VA_RANGE_MASK	(0xf << ID_AA64MMFR2_VA_RANGE_SHIFT)
#define	ID_AA64MMFR2_VA_RANGE(x)	((x) & ID_AA64MMFR2_VA_RANGE_MASK)
#define	 ID_AA64MMFR2_VA_RANGE_48	(0x0 << ID_AA64MMFR2_VA_RANGE_SHIFT)
#define	 ID_AA64MMFR2_VA_RANGE_52	(0x1 << ID_AA64MMFR2_VA_RANGE_SHIFT)
#define	ID_AA64MMFR2_CCIDX_SHIFT	20
#define	ID_AA64MMFR2_CCIDX_MASK		(0xf << ID_AA64MMFR2_CCIDX_SHIFT)
#define	ID_AA64MMFR2_CCIDX(x)		((x) & ID_AA64MMFR2_CCIDX_MASK)
#define	 ID_AA64MMFR2_CCIDX_32		(0x0 << ID_AA64MMFR2_CCIDX_SHIFT)
#define	 ID_AA64MMFR2_CCIDX_64		(0x1 << ID_AA64MMFR2_CCIDX_SHIFT)
#define	ID_AA64MMFR2_NV_SHIFT		24
#define	ID_AA64MMFR2_NV_MASK		(0xf << ID_AA64MMFR2_NV_SHIFT)
#define	ID_AA64MMFR2_NV(x)		((x) & ID_AA64MMFR2_NV_MASK)
#define	 ID_AA64MMFR2_NV_NONE		(0x0 << ID_AA64MMFR2_NV_SHIFT)
#define	 ID_AA64MMFR2_NV_IMPL		(0x1 << ID_AA64MMFR2_NV_SHIFT)

/* ID_AA64PFR0_EL1 */
#define	ID_AA64PFR0_MASK		0x0000000ffffffffful
#define	ID_AA64PFR0_EL0_SHIFT		0
#define	ID_AA64PFR0_EL0_MASK		(0xf << ID_AA64PFR0_EL0_SHIFT)
#define	ID_AA64PFR0_EL0(x)		((x) & ID_AA64PFR0_EL0_MASK)
#define	 ID_AA64PFR0_EL0_64		(1 << ID_AA64PFR0_EL0_SHIFT)
#define	 ID_AA64PFR0_EL0_64_32		(2 << ID_AA64PFR0_EL0_SHIFT)
#define	ID_AA64PFR0_EL1_SHIFT		4
#define	ID_AA64PFR0_EL1_MASK		(0xf << ID_AA64PFR0_EL1_SHIFT)
#define	ID_AA64PFR0_EL1(x)		((x) & ID_AA64PFR0_EL1_MASK)
#define	 ID_AA64PFR0_EL1_64		(1 << ID_AA64PFR0_EL1_SHIFT)
#define	 ID_AA64PFR0_EL1_64_32		(2 << ID_AA64PFR0_EL1_SHIFT)
#define	ID_AA64PFR0_EL2_SHIFT		8
#define	ID_AA64PFR0_EL2_MASK		(0xf << ID_AA64PFR0_EL2_SHIFT)
#define	ID_AA64PFR0_EL2(x)		((x) & ID_AA64PFR0_EL2_MASK)
#define	 ID_AA64PFR0_EL2_NONE		(0 << ID_AA64PFR0_EL2_SHIFT)
#define	 ID_AA64PFR0_EL2_64		(1 << ID_AA64PFR0_EL2_SHIFT)
#define	 ID_AA64PFR0_EL2_64_32		(2 << ID_AA64PFR0_EL2_SHIFT)
#define	ID_AA64PFR0_EL3_SHIFT		12
#define	ID_AA64PFR0_EL3_MASK		(0xf << ID_AA64PFR0_EL3_SHIFT)
#define	ID_AA64PFR0_EL3(x)		((x) & ID_AA64PFR0_EL3_MASK)
#define	 ID_AA64PFR0_EL3_NONE		(0 << ID_AA64PFR0_EL3_SHIFT)
#define	 ID_AA64PFR0_EL3_64		(1 << ID_AA64PFR0_EL3_SHIFT)
#define	 ID_AA64PFR0_EL3_64_32		(2 << ID_AA64PFR0_EL3_SHIFT)
#define	ID_AA64PFR0_FP_SHIFT		16
#define	ID_AA64PFR0_FP_MASK		(0xf << ID_AA64PFR0_FP_SHIFT)
#define	ID_AA64PFR0_FP(x)		((x) & ID_AA64PFR0_FP_MASK)
#define	 ID_AA64PFR0_FP_IMPL		(0x0 << ID_AA64PFR0_FP_SHIFT)
#define	 ID_AA64PFR0_FP_HP		(0x1 << ID_AA64PFR0_FP_SHIFT)
#define	 ID_AA64PFR0_FP_NONE		(0xf << ID_AA64PFR0_FP_SHIFT)
#define	ID_AA64PFR0_ADV_SIMD_SHIFT	20
#define	ID_AA64PFR0_ADV_SIMD_MASK	(0xf << ID_AA64PFR0_ADV_SIMD_SHIFT)
#define	ID_AA64PFR0_ADV_SIMD(x)		((x) & ID_AA64PFR0_ADV_SIMD_MASK)
#define	 ID_AA64PFR0_ADV_SIMD_IMPL	(0x0 << ID_AA64PFR0_ADV_SIMD_SHIFT)
#define	 ID_AA64PFR0_ADV_SIMD_HP	(0x1 << ID_AA64PFR0_ADV_SIMD_SHIFT)
#define	 ID_AA64PFR0_ADV_SIMD_NONE	(0xf << ID_AA64PFR0_ADV_SIMD_SHIFT)
#define	ID_AA64PFR0_GIC_BITS		0x4 /* Number of bits in GIC field */
#define	ID_AA64PFR0_GIC_SHIFT		24
#define	ID_AA64PFR0_GIC_MASK		(0xf << ID_AA64PFR0_GIC_SHIFT)
#define	ID_AA64PFR0_GIC(x)		((x) & ID_AA64PFR0_GIC_MASK)
#define	 ID_AA64PFR0_GIC_CPUIF_NONE	(0x0 << ID_AA64PFR0_GIC_SHIFT)
#define	 ID_AA64PFR0_GIC_CPUIF_EN	(0x1 << ID_AA64PFR0_GIC_SHIFT)
#define	ID_AA64PFR0_RAS_SHIFT		28
#define	ID_AA64PFR0_RAS_MASK		(0xf << ID_AA64PFR0_RAS_SHIFT)
#define	ID_AA64PFR0_RAS(x)		((x) & ID_AA64PFR0_RAS_MASK)
#define	 ID_AA64PFR0_RAS_NONE		(0x0 << ID_AA64PFR0_RAS_SHIFT)
#define	 ID_AA64PFR0_RAS_V1		(0x1 << ID_AA64PFR0_RAS_SHIFT)
#define	ID_AA64PFR0_SVE_SHIFT		32
#define	ID_AA64PFR0_SVE_MASK		(0xful << ID_AA64PFR0_SVE_SHIFT)
#define	ID_AA64PFR0_SVE(x)		((x) & ID_AA64PFR0_SVE_MASK)
#define	 ID_AA64PFR0_SVE_NONE		(0x0ul << ID_AA64PFR0_SVE_SHIFT)
#define	 ID_AA64PFR0_SVE_IMPL		(0x1ul << ID_AA64PFR0_SVE_SHIFT)

/* MAIR_EL1 - Memory Attribute Indirection Register */
#define	MAIR_ATTR_MASK(idx)	(0xff << ((n)* 8))
#define	MAIR_ATTR(attr, idx) ((attr) << ((idx) * 8))
#define	 MAIR_DEVICE_nGnRnE	0x00
#define	 MAIR_NORMAL_NC		0x44
#define	 MAIR_NORMAL_WT		0xbb
#define	 MAIR_NORMAL_WB		0xff

/* PAR_EL1 - Physical Address Register */
#define	PAR_F_SHIFT		0
#define	PAR_F			(0x1 << PAR_F_SHIFT)
#define	PAR_SUCCESS(x)		(((x) & PAR_F) == 0)
/* When PAR_F == 0 (success) */
#define	PAR_SH_SHIFT		7
#define	PAR_SH_MASK		(0x3 << PAR_SH_SHIFT)
#define	PAR_NS_SHIFT		9
#define	PAR_NS_MASK		(0x3 << PAR_NS_SHIFT)
#define	PAR_PA_SHIFT		12
#define	PAR_PA_MASK		0x0000fffffffff000
#define	PAR_ATTR_SHIFT		56
#define	PAR_ATTR_MASK		(0xff << PAR_ATTR_SHIFT)
/* When PAR_F == 1 (aborted) */
#define	PAR_FST_SHIFT		1
#define	PAR_FST_MASK		(0x3f << PAR_FST_SHIFT)
#define	PAR_PTW_SHIFT		8
#define	PAR_PTW_MASK		(0x1 << PAR_PTW_SHIFT)
#define	PAR_S_SHIFT		9
#define	PAR_S_MASK		(0x1 << PAR_S_SHIFT)

/* SCTLR_EL1 - System Control Register */
#define	SCTLR_RES0	0xc8222440	/* Reserved ARMv8.0, write 0 */
#define	SCTLR_RES1	0x30d00800	/* Reserved ARMv8.0, write 1 */

#define	SCTLR_M		0x00000001
#define	SCTLR_A		0x00000002
#define	SCTLR_C		0x00000004
#define	SCTLR_SA	0x00000008
#define	SCTLR_SA0	0x00000010
#define	SCTLR_CP15BEN	0x00000020
/* Bit 6 is reserved */
#define	SCTLR_ITD	0x00000080
#define	SCTLR_SED	0x00000100
#define	SCTLR_UMA	0x00000200
/* Bit 10 is reserved */
/* Bit 11 is reserved */
#define	SCTLR_I		0x00001000
#define	SCTLR_EnDB	0x00002000 /* ARMv8.3 */
#define	SCTLR_DZE	0x00004000
#define	SCTLR_UCT	0x00008000
#define	SCTLR_nTWI	0x00010000
/* Bit 17 is reserved */
#define	SCTLR_nTWE	0x00040000
#define	SCTLR_WXN	0x00080000
/* Bit 20 is reserved */
#define	SCTLR_IESB	0x00200000 /* ARMv8.2 */
/* Bit 22 is reserved */
#define	SCTLR_SPAN	0x00800000 /* ARMv8.1 */
#define	SCTLR_EOE	0x01000000
#define	SCTLR_EE	0x02000000
#define	SCTLR_UCI	0x04000000
#define	SCTLR_EnDA	0x08000000 /* ARMv8.3 */
#define	SCTLR_nTLSMD	0x10000000 /* ARMv8.2 */
#define	SCTLR_LSMAOE	0x20000000 /* ARMv8.2 */
#define	SCTLR_EnIB	0x40000000 /* ARMv8.3 */
#define	SCTLR_EnIA	0x80000000 /* ARMv8.3 */

/* SPSR_EL1 */
/*
 * When the exception is taken in AArch64:
 * M[3:2] is the exception level
 * M[1]   is unused
 * M[0]   is the SP select:
 *         0: always SP0
 *         1: current ELs SP
 */
#define	PSR_M_EL0t	0x00000000
#define	PSR_M_EL1t	0x00000004
#define	PSR_M_EL1h	0x00000005
#define	PSR_M_EL2t	0x00000008
#define	PSR_M_EL2h	0x00000009
#define	PSR_M_MASK	0x0000000f

#define	PSR_AARCH32	0x00000010
#define	PSR_F		0x00000040
#define	PSR_I		0x00000080
#define	PSR_A		0x00000100
#define	PSR_D		0x00000200
#define	PSR_IL		0x00100000
#define	PSR_SS		0x00200000
#define	PSR_V		0x10000000
#define	PSR_C		0x20000000
#define	PSR_Z		0x40000000
#define	PSR_N		0x80000000
#define	PSR_FLAGS	0xf0000000

/* TCR_EL1 - Translation Control Register */
#define	TCR_ASID_16	(1 << 36)

#define	TCR_IPS_SHIFT	32
#define	TCR_IPS_32BIT	(0 << TCR_IPS_SHIFT)
#define	TCR_IPS_36BIT	(1 << TCR_IPS_SHIFT)
#define	TCR_IPS_40BIT	(2 << TCR_IPS_SHIFT)
#define	TCR_IPS_42BIT	(3 << TCR_IPS_SHIFT)
#define	TCR_IPS_44BIT	(4 << TCR_IPS_SHIFT)
#define	TCR_IPS_48BIT	(5 << TCR_IPS_SHIFT)

#define	TCR_TG1_SHIFT	30
#define	TCR_TG1_16K	(1 << TCR_TG1_SHIFT)
#define	TCR_TG1_4K	(2 << TCR_TG1_SHIFT)
#define	TCR_TG1_64K	(3 << TCR_TG1_SHIFT)

#define	TCR_SH1_SHIFT	28
#define	TCR_SH1_IS	(0x3UL << TCR_SH1_SHIFT)
#define	TCR_ORGN1_SHIFT	26
#define	TCR_ORGN1_WBWA	(0x1UL << TCR_ORGN1_SHIFT)
#define	TCR_IRGN1_SHIFT	24
#define	TCR_IRGN1_WBWA	(0x1UL << TCR_IRGN1_SHIFT)
#define	TCR_SH0_SHIFT	12
#define	TCR_SH0_IS	(0x3UL << TCR_SH0_SHIFT)
#define	TCR_ORGN0_SHIFT	10
#define	TCR_ORGN0_WBWA	(0x1UL << TCR_ORGN0_SHIFT)
#define	TCR_IRGN0_SHIFT	8
#define	TCR_IRGN0_WBWA	(0x1UL << TCR_IRGN0_SHIFT)

#define	TCR_CACHE_ATTRS	((TCR_IRGN0_WBWA | TCR_IRGN1_WBWA) |\
				(TCR_ORGN0_WBWA | TCR_ORGN1_WBWA))

#ifdef SMP
#define	TCR_SMP_ATTRS	(TCR_SH0_IS | TCR_SH1_IS)
#else
#define	TCR_SMP_ATTRS	0
#endif

#define	TCR_T1SZ_SHIFT	16
#define	TCR_T0SZ_SHIFT	0
#define	TCR_T1SZ(x)	((x) << TCR_T1SZ_SHIFT)
#define	TCR_T0SZ(x)	((x) << TCR_T0SZ_SHIFT)
#define	TCR_TxSZ(x)	(TCR_T1SZ(x) | TCR_T0SZ(x))

/* Saved Program Status Register */
#define	DBG_SPSR_SS	(0x1 << 21)

/* Monitor Debug System Control Register */
#define	DBG_MDSCR_SS	(0x1 << 0)
#define	DBG_MDSCR_KDE	(0x1 << 13)
#define	DBG_MDSCR_MDE	(0x1 << 15)

/* Perfomance Monitoring Counters */
#define	PMCR_E		(1 << 0) /* Enable all counters */
#define	PMCR_P		(1 << 1) /* Reset all counters */
#define	PMCR_C		(1 << 2) /* Clock counter reset */
#define	PMCR_D		(1 << 3) /* CNTR counts every 64 clk cycles */
#define	PMCR_X		(1 << 4) /* Export to ext. monitoring (ETM) */
#define	PMCR_DP		(1 << 5) /* Disable CCNT if non-invasive debug*/
#define	PMCR_LC		(1 << 6) /* Long cycle count enable */
#define	PMCR_IMP_SHIFT	24 /* Implementer code */
#define	PMCR_IMP_MASK	(0xff << PMCR_IMP_SHIFT)
#define	PMCR_IDCODE_SHIFT	16 /* Identification code */
#define	PMCR_IDCODE_MASK	(0xff << PMCR_IDCODE_SHIFT)
#define	 PMCR_IDCODE_CORTEX_A57	0x01
#define	 PMCR_IDCODE_CORTEX_A72	0x02
#define	 PMCR_IDCODE_CORTEX_A53	0x03
#define	PMCR_N_SHIFT	11       /* Number of counters implemented */
#define	PMCR_N_MASK	(0x1f << PMCR_N_SHIFT)

#endif /* !_MACHINE_ARMREG_H_ */
