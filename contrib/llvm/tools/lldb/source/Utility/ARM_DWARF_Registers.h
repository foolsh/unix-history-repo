//===-- ARM_DWARF_Registers.h -----------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef utility_ARM_DWARF_Registers_h_
#define utility_ARM_DWARF_Registers_h_

#include "lldb/lldb-private.h"

enum
{
    dwarf_r0 = 0,
    dwarf_r1,
    dwarf_r2,
    dwarf_r3,
    dwarf_r4,
    dwarf_r5,
    dwarf_r6,
    dwarf_r7,
    dwarf_r8,
    dwarf_r9,
    dwarf_r10,
    dwarf_r11,
    dwarf_r12,
    dwarf_sp,
    dwarf_lr,
    dwarf_pc,
    dwarf_cpsr,

    dwarf_s0 = 64,
    dwarf_s1,
    dwarf_s2,
    dwarf_s3,
    dwarf_s4,
    dwarf_s5,
    dwarf_s6,
    dwarf_s7,
    dwarf_s8,
    dwarf_s9,
    dwarf_s10,
    dwarf_s11,
    dwarf_s12,
    dwarf_s13,
    dwarf_s14,
    dwarf_s15,
    dwarf_s16,
    dwarf_s17,
    dwarf_s18,
    dwarf_s19,
    dwarf_s20,
    dwarf_s21,
    dwarf_s22,
    dwarf_s23,
    dwarf_s24,
    dwarf_s25,
    dwarf_s26,
    dwarf_s27,
    dwarf_s28,
    dwarf_s29,
    dwarf_s30,
    dwarf_s31,

    // FPA Registers 0-7
    dwarf_f0 = 96,
    dwarf_f1,
    dwarf_f2,
    dwarf_f3,
    dwarf_f4,
    dwarf_f5,
    dwarf_f6,
    dwarf_f7,

    // Intel wireless MMX general purpose registers 0 - 7
    dwarf_wCGR0 = 104,
    dwarf_wCGR1,
    dwarf_wCGR2,
    dwarf_wCGR3,
    dwarf_wCGR4,
    dwarf_wCGR5,
    dwarf_wCGR6,
    dwarf_wCGR7,

    // XScale accumulator register 0 - 7 (they do overlap with wCGR0 - wCGR7)
    dwarf_ACC0 = 104,
    dwarf_ACC1,
    dwarf_ACC2,
    dwarf_ACC3,
    dwarf_ACC4,
    dwarf_ACC5,
    dwarf_ACC6,
    dwarf_ACC7,

    // Intel wireless MMX data registers 0 - 15
    dwarf_wR0 = 112,
    dwarf_wR1,
    dwarf_wR2,
    dwarf_wR3,
    dwarf_wR4,
    dwarf_wR5,
    dwarf_wR6,
    dwarf_wR7,
    dwarf_wR8,
    dwarf_wR9,
    dwarf_wR10,
    dwarf_wR11,
    dwarf_wR12,
    dwarf_wR13,
    dwarf_wR14,
    dwarf_wR15,

    dwarf_spsr = 128,
    dwarf_spsr_fiq,
    dwarf_spsr_irq,
    dwarf_spsr_abt,
    dwarf_spsr_und,
    dwarf_spsr_svc,

    dwarf_r8_usr = 144,
    dwarf_r9_usr,
    dwarf_r10_usr,
    dwarf_r11_usr,
    dwarf_r12_usr,
    dwarf_r13_usr,
    dwarf_r14_usr,
    dwarf_r8_fiq,
    dwarf_r9_fiq,
    dwarf_r10_fiq,
    dwarf_r11_fiq,
    dwarf_r12_fiq,
    dwarf_r13_fiq,
    dwarf_r14_fiq,
    dwarf_r13_irq,
    dwarf_r14_irq,
    dwarf_r13_abt,
    dwarf_r14_abt,
    dwarf_r13_und,
    dwarf_r14_und,
    dwarf_r13_svc,
    dwarf_r14_svc,

    // Intel wireless MMX control register in co-processor 0 - 7
    dwarf_wC0 = 192,
    dwarf_wC1,
    dwarf_wC2,
    dwarf_wC3,
    dwarf_wC4,
    dwarf_wC5,
    dwarf_wC6,
    dwarf_wC7,

    // VFP-v3/Neon
    dwarf_d0 = 256,
    dwarf_d1,
    dwarf_d2,
    dwarf_d3,
    dwarf_d4,
    dwarf_d5,
    dwarf_d6,
    dwarf_d7,
    dwarf_d8,
    dwarf_d9,
    dwarf_d10,
    dwarf_d11,
    dwarf_d12,
    dwarf_d13,
    dwarf_d14,
    dwarf_d15,
    dwarf_d16,
    dwarf_d17,
    dwarf_d18,
    dwarf_d19,
    dwarf_d20,
    dwarf_d21,
    dwarf_d22,
    dwarf_d23,
    dwarf_d24,
    dwarf_d25,
    dwarf_d26,
    dwarf_d27,
    dwarf_d28,
    dwarf_d29,
    dwarf_d30,
    dwarf_d31,

    // Neon quadword registers
    dwarf_q0 = 288,
    dwarf_q1,
    dwarf_q2,
    dwarf_q3,
    dwarf_q4,
    dwarf_q5,
    dwarf_q6,
    dwarf_q7,
    dwarf_q8,
    dwarf_q9,
    dwarf_q10,
    dwarf_q11,
    dwarf_q12,
    dwarf_q13,
    dwarf_q14,
    dwarf_q15
};

const char *
GetARMDWARFRegisterName (unsigned reg_num);
    
bool
GetARMDWARFRegisterInfo (unsigned reg_num, 
                         lldb_private::RegisterInfo &reg_info);

#endif // utility_ARM_DWARF_Registers_h_

