// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Loads all opcodes.
 */
TAsmOpcodeManager::TAsmOpcodeManager()
{
    // Load Intel Opcodes
    this->LoadIntelOpcodes();
}
/**
 * Loads all known opcodes.
 */
void TAsmOpcodeManager::LoadIntelOpcodes()
{
    // Prepare vector
    this->opcode_.reserve(550);

    // 3 byte opcodes
    this->AddOpcode(3, 0x0F, 0x01, 0xC1, "vmcall", "vmcall", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call to VM Monitor");
    this->AddOpcode(3, 0x0F, 0x01, 0xC2, "vmlaunch", "vmlaunch", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Launch Virtual Machine");
    this->AddOpcode(3, 0x0F, 0x01, 0xC3, "vmresume", "vmresume", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Resume Virtual Machine");
    this->AddOpcode(3, 0x0F, 0x01, 0xC4, "vmxoff", "vmxoff", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Leave VMX Operation");
    this->AddOpcode(3, 0x0F, 0x01, 0xC8, "monitor", "monitor", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set Up Monitor Address");
    this->AddOpcode(3, 0x0F, 0xAE, 0xF8, "sfence", "sfence", false, PT_NONE, DONTCARE, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store fence");
    // 2 byte opcodes
    // aad
    this->AddOpcode(2, 0xD5, 0x0A, "aad", "aad", "ASCII adjust after division");
    // aam
    this->AddOpcode(2, 0xD4, 0x0A, "aam", "aam", "ASCII adjust after multiplication");
    // bsf
    this->AddOpcode(2, 0x0F, 0xBC, HE_OPEXT_NONE, "bsf %s, %s", "bsf %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Bit scan foreward");
    // bsr
    this->AddOpcode(2, 0x0F, 0xBD, HE_OPEXT_NONE, "bsr %s, %s", "bsr %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Bit scan reverse");
    // bswap
    this->AddOpcode(2, 0x0F, 0xC8, "bswap ax", "bswap eax", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xC9, "bswap cx", "bswap ecx", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCA, "bswap dx", "bswap edx", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCB, "bswap bx", "bswap ebx", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCC, "bswap sp", "bswap esp", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCD, "bswap bp", "bswap ebp", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCE, "bswap si", "bswap esi", "Byte swap");
    this->AddOpcode(2, 0x0F, 0xCF, "bswap di", "bswap edi", "Byte swap");
    // bt
    this->AddOpcode(2, 0x0F, 0xA3, HE_OPEXT_NONE, "bt %s, %s", "bt %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Bit test");
    this->AddOpcode(2, 0x0F, 0xBA, HE_OPEXT_0100, "bt %s, %s", "bt %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Bit test");
    // btc
    this->AddOpcode(2, 0x0F, 0xBB, HE_OPEXT_NONE, "btc %s, %s", "btc %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Bit test and complement");
    this->AddOpcode(2, 0x0F, 0xBA, HE_OPEXT_0111, "btc %s, %s", "btc %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Bit test and complement");
    // btr
    this->AddOpcode(2, 0x0F, 0xB3, HE_OPEXT_NONE, "btr %s, %s", "btr %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Bit test and reset");
    this->AddOpcode(2, 0x0F, 0xBA, HE_OPEXT_0110, "btr %s, %s", "btr %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Bit test and reset");
    // bts
    this->AddOpcode(2, 0x0F, 0xAB, HE_OPEXT_NONE, "bts %s, %s", "bts %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Bit test and set");
    this->AddOpcode(2, 0x0F, 0xBA, HE_OPEXT_0101, "bts %s, %s", "bts %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Bit test and set");
    // clts
    this->AddOpcode(2, 0x0F, 0x06, "clts", "clts", "Clear task-switched flag in CR0");
    // cmov
    this->AddOpcode(2, 0x0F, 0x40, HE_OPEXT_NONE, "cmovo %s, %s", "cmovo %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move on overflow ");
    this->AddOpcode(2, 0x0F, 0x41, HE_OPEXT_NONE, "cmovno %s, %s", "cmovno %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move on not overflow ");
    this->AddOpcode(2, 0x0F, 0x42, HE_OPEXT_NONE, "cmovb %s, %s", "cmovb %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if below");
    this->AddOpcode(2, 0x0F, 0x43, HE_OPEXT_NONE, "cmovnb %s, %s", "cmovnb %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if not below");
    this->AddOpcode(2, 0x0F, 0x44, HE_OPEXT_NONE, "cmove %s, %s", "cmove %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if equal");
    this->AddOpcode(2, 0x0F, 0x45, HE_OPEXT_NONE, "cmovne %s, %s", "cmovne %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if not equal");
    this->AddOpcode(2, 0x0F, 0x46, HE_OPEXT_NONE, "cmovbe %s, %s", "cmovbe %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if below or equal");
    this->AddOpcode(2, 0x0F, 0x47, HE_OPEXT_NONE, "cmova %s, %s", "cmova %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if above");
    this->AddOpcode(2, 0x0F, 0x48, HE_OPEXT_NONE, "cmovs %s, %s", "cmovs %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if signed (negative)");
    this->AddOpcode(2, 0x0F, 0x49, HE_OPEXT_NONE, "cmovns %s, %s", "cmovns %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if not signed (positive)");
    this->AddOpcode(2, 0x0F, 0x4A, HE_OPEXT_NONE, "cmovp %s, %s", "cmovp %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if parity even");
    this->AddOpcode(2, 0x0F, 0x4B, HE_OPEXT_NONE, "cmovnp %s, %s", "cmovnp %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if parity odd");
    this->AddOpcode(2, 0x0F, 0x4C, HE_OPEXT_NONE, "cmovl %s, %s", "cmovl %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if less");
    this->AddOpcode(2, 0x0F, 0x4D, HE_OPEXT_NONE, "cmovnl %s, %s", "cmovnl %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if not less");
    this->AddOpcode(2, 0x0F, 0x4E, HE_OPEXT_NONE, "cmovle %s, %s", "cmovle %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if less or equal");
    this->AddOpcode(2, 0x0F, 0x4F, HE_OPEXT_NONE, "cmovg %s, %s", "cmovg %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move if greater");
    // cmpxchg
    this->AddOpcode(2, 0x0F, 0xB0, HE_OPEXT_NONE, "cmpxchg %s, %s", "cmpxchg %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Compare and exchange");
    this->AddOpcode(2, 0x0F, 0xB1, HE_OPEXT_NONE, "cmpxchg %s, %s", "cmpxchg %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Compare and exchange");
    // cmpxchg8b
    this->AddOpcode(2, 0x0F, 0xC7, HE_OPEXT_0001, "cmpxchg8b %s", "cmpxchg8b %s", true, PT_REG_MEM, BIT64, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Compare and exchange 8 bytes");
    // cpuid
    this->AddOpcode(2, 0x0F, 0xA2, "cpuid", "cpuid", "CPU identification");
    // f2xm1
    this->AddOpcode(2, 0xD9, 0xF0, "f2xm1", "f2xm1", "Compute 2^x-1");
    // fabs
    this->AddOpcode(2, 0xD9, 0xE1, "fabs", "fabs", "Absolute value");
    // fchs
    this->AddOpcode(2, 0xD9, 0xE0, "fchs", "fchs", "Change sign");
    // fnclex
    this->AddOpcode(2, 0xDB, 0xE2, "fnclex", "fnclex", "Clear floating-point exceptions");
    // fcos
    this->AddOpcode(2, 0xD9, 0xFF, "fcos", "fcos", "Cosine");
    // fdecstp
    this->AddOpcode(2, 0xD9, 0xF6, "fdecstp", "fdecstp", "Decrement stack top pointer");
    // fincstp
    this->AddOpcode(2, 0xD9, 0xF7, "fincstp", "fincstp", "Increment stack top pointer");
    // fninit
    this->AddOpcode(2, 0xDB, 0xE3, "fninit", "fninit", "Initiate floating-point unit");
    // fld1
    this->AddOpcode(2, 0xD9, 0xE8, "fld1", "fld1", "Push +1.0 onto the FPU stack");
    // fldl2t
    this->AddOpcode(2, 0xD9, 0xE9, "fldl2t", "fldl2t", "Push log 2/10 onto the FPU stack");
    // fldl2e
    this->AddOpcode(2, 0xD9, 0xEA, "fldl2e", "fldl2e", "Push log e/10 onto the FPU stack");
    // fldpi
    this->AddOpcode(2, 0xD9, 0xEB, "fldpi", "fldpi", "Push pi onto the FPU stack");
    // fldlg2
    this->AddOpcode(2, 0xD9, 0xEC, "fldlg2", "fldlg2", "Push log 10/2 onto the FPU stack");
    // fldln2
    this->AddOpcode(2, 0xD9, 0xED, "fldln2", "fldln2", "Push log e/2 onto the FPU stack");
    // fldz
    this->AddOpcode(2, 0xD9, 0xEE, "fldz", "fldz", "Push +0.0 onto the FPU stack");
    // fnop
    this->AddOpcode(2, 0xD9, 0xD0, "fnop", "fnop", "No operation");
    // fpatan
    this->AddOpcode(2, 0xD9, 0xF3, "fpatan", "fpatan", "Partitial arctangent");
    // fprem
    this->AddOpcode(2, 0xD9, 0xF8, "fprem", "fprem", "Partitial remainder");
    // fprem1
    this->AddOpcode(2, 0xD9, 0xF5, "fprem1", "fprem1", "IEEE Standard partitial remainder");
    // fptan
    this->AddOpcode(2, 0xD9, 0xF2, "fptan", "fptan", "Partitial tangent");
    // frndint
    this->AddOpcode(2, 0xD9, 0xFC, "frndint", "frndint", "Round to integer");
    // fscale
    this->AddOpcode(2, 0xD9, 0xFD, "fscale", "fscale", "Scale by a power of 2");
    // fsin
    this->AddOpcode(2, 0xD9, 0xFE, "fsin", "fsin", "Sine");
    // fsincos
    this->AddOpcode(2, 0xD9, 0xFB, "fsincos", "fsincos", "Sine and Cosine");
    // fsqrt
    this->AddOpcode(2, 0xD9, 0xFA, "fsqrt", "fsqrt", "Square root");
    // ftst
    this->AddOpcode(2, 0xD9, 0xE4, "ftst", "ftst", "Test");
    // fxam
    this->AddOpcode(2, 0xD9, 0xE5, "fxam", "fxam", "Examine");
    // fxtract
    this->AddOpcode(2, 0xD9, 0xF4, "fxtract", "fxtract", "Extract exponent and significand");
    // fyl2x
    this->AddOpcode(2, 0xD9, 0xF1, "fyl2x", "fyl2x", "Compute y times log2x");
    // fyl2xp1
    this->AddOpcode(2, 0xD9, 0xF9, "fyl2xp1", "fyl2xp1", "Compute y times log2(x+1)");
    // invd
    this->AddOpcode(2, 0x0F, 0x08, "invd", "invd", "Invalid interal caches");
    // jxx
    this->AddOpcode(2, 0x0F, 0x80, HE_OPEXT_NONE, "jo %s", "jo %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump on overflow ");
    this->AddOpcode(2, 0x0F, 0x81, HE_OPEXT_NONE, "jno %s", "jno %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump on not overflow ");
    this->AddOpcode(2, 0x0F, 0x82, HE_OPEXT_NONE, "jb %s", "jb %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if below");
    this->AddOpcode(2, 0x0F, 0x83, HE_OPEXT_NONE, "jnb %s", "jnb %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not below");
    this->AddOpcode(2, 0x0F, 0x84, HE_OPEXT_NONE, "je %s", "je %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if equal");
    this->AddOpcode(2, 0x0F, 0x85, HE_OPEXT_NONE, "jne %s", "jne %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not equal");
    this->AddOpcode(2, 0x0F, 0x86, HE_OPEXT_NONE, "jbe %s", "jbe %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if below or equal");
    this->AddOpcode(2, 0x0F, 0x87, HE_OPEXT_NONE, "ja %s", "ja %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if above");
    this->AddOpcode(2, 0x0F, 0x88, HE_OPEXT_NONE, "js %s", "js %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if signed (negative)");
    this->AddOpcode(2, 0x0F, 0x89, HE_OPEXT_NONE, "jns %s", "jns %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not signed (positive)");
    this->AddOpcode(2, 0x0F, 0x8A, HE_OPEXT_NONE, "jp %s", "jp %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if parity even");
    this->AddOpcode(2, 0x0F, 0x8B, HE_OPEXT_NONE, "jnp %s", "jnp %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if parity odd");
    this->AddOpcode(2, 0x0F, 0x8C, HE_OPEXT_NONE, "jl %s", "jl %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if less");
    this->AddOpcode(2, 0x0F, 0x8D, HE_OPEXT_NONE, "jnl %s", "jnl %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not less");
    this->AddOpcode(2, 0x0F, 0x8E, HE_OPEXT_NONE, "jle %s", "jle %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if less or equal");
    this->AddOpcode(2, 0x0F, 0x8F, HE_OPEXT_NONE, "jg %s", "jg %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if greater");
    // imul
    this->AddOpcode(2, 0x0F, 0xAF, HE_OPEXT_NONE, "imul %s, %s", "imul %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Signed integer multiply");
    // lgdt/lidt
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0010, "lgdt %s", "lgdt %s", true, PT_REG_MEM, BIT48, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Load Global Descriptor Table Register");
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0011, "lidt %s", "lidt %s", true, PT_REG_MEM, BIT48, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Load Interrupt Descriptor Table Register");
    // lgs/lss/lfs
    this->AddOpcode(2, 0x0F, 0xB2, HE_OPEXT_NONE, "lss %s, %s", "lss %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load full pointer using ss");
    this->AddOpcode(2, 0x0F, 0xB4, HE_OPEXT_NONE, "lfs %s, %s", "lfs %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load full pointer using fs");
    this->AddOpcode(2, 0x0F, 0xB5, HE_OPEXT_NONE, "lgs %s, %s", "lgs %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load full pointer using gs");
    // lldt
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0010, "lldt %s", "lldt %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Load Local Descriptor Table Register");
    // lmsw
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0110, "lmsw %s", "lmsw %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Load machine status word");
    // lsl
    this->AddOpcode(2, 0x0F, 0x03, HE_OPEXT_NONE, "lsl %s, %s", "lsl %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load segment limit");
    // ltr
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0011, "ltr %s", "ltr %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Load task register");
    // lar
    this->AddOpcode(2, 0x0F, 0x02, HE_OPEXT_NONE, "lar %s, %s", "lar %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load access rights byte");
    // mov (Control Registers)
    this->AddOpcode(2, 0x0F, 0x20, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT32, PT_CREG, BIT32, PT_NONE, DONTCARE, "Move data from control register");
    this->AddOpcode(2, 0x0F, 0x22, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_CREG, BIT32, PT_REG_MEM, BIT32, PT_NONE, DONTCARE, "Move data to control register");
    // mov (Debug Registers)
    this->AddOpcode(2, 0x0F, 0x21, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT32, PT_DREG, BIT32, PT_NONE, DONTCARE, "Move data from debug register");
    this->AddOpcode(2, 0x0F, 0x23, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_DREG, BIT32, PT_REG_MEM, BIT32, PT_NONE, DONTCARE, "Move data to debug register");
    // mov (Test Registers)
    this->AddOpcode(2, 0x0F, 0x24, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT32, PT_TREG, BIT32, PT_NONE, DONTCARE, "Move data from test register");
    this->AddOpcode(2, 0x0F, 0x26, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_TREG, BIT32, PT_REG_MEM, BIT32, PT_NONE, DONTCARE, "Move data to test register");
    // movsx
    this->AddOpcode(2, 0x0F, 0xBE, HE_OPEXT_NONE, "movsx %s, %s", "movsx %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT8, PT_NONE, BIT8, "Move with sign extension");
    this->AddOpcode(2, 0x0F, 0xBF, HE_OPEXT_NONE, "movsx %s, %s", "movsx %s, %s", true, PT_REG, BIT32, PT_REG_MEM, BIT16, PT_NONE, BIT16, "Move with sign extension");
    // movsz
    this->AddOpcode(2, 0x0F, 0xB6, HE_OPEXT_NONE, "movzx %s, %s", "movzx %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT8, PT_NONE, BIT8, "Move with zero extension");
    this->AddOpcode(2, 0x0F, 0xB7, HE_OPEXT_NONE, "movzx %s, %s", "movzx %s, %s", true, PT_REG, BIT32, PT_REG_MEM, BIT16, PT_NONE, BIT16, "Move with zero extension");
    // nop
    this->AddOpcode(2, 0x0f, 0x1f, HE_OPEXT_NONE, "nop %s", "nop %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "No operation");
    // rdmsr
    this->AddOpcode(2, 0x0F, 0x32, "rdmsr", "rdmsr", "Read from model-specific register");
    // rdmsr
    this->AddOpcode(2, 0x0F, 0x33, "rdpmc", "rdpmc", "Read performance-monitoring counters");
    // rdtsc
    this->AddOpcode(2, 0x0F, 0x31, "rdtsc", "rdtsc", "Read time-stamp counters into EDX:EAX");
    // pop
    this->AddOpcode(2, 0x0F, 0xA1, "pop fs", "pop fs", "Pop a word from the stack");
    this->AddOpcode(2, 0x0F, 0xA9, "pop gs", "pop gs", "Pop a word from the stack");
    // push
    this->AddOpcode(2, 0x0F, 0xA0, "push fs", "push fs", "Push operand onto the stack");
    this->AddOpcode(2, 0x0F, 0xA8, "push gs", "push gs", "Push operand onto the stack");
    // rsm
    this->AddOpcode(2, 0x0F, 0xAA, "rsm", "rsm", "Resume from System Management Mode");
    // Setcc
    this->AddOpcode(2, 0x0F, 0x97, HE_OPEXT_NONE, "seta %s", "seta %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if above");
    this->AddOpcode(2, 0x0F, 0x93, HE_OPEXT_NONE, "setae %s", "setae %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if above or equal");
    this->AddOpcode(2, 0x0F, 0x92, HE_OPEXT_NONE, "setb %s", "setb %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if below");
    this->AddOpcode(2, 0x0F, 0x96, HE_OPEXT_NONE, "setbe %s", "setbe %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if below or equal");
    this->AddOpcode(2, 0x0F, 0x94, HE_OPEXT_NONE, "sete %s", "sete %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if equal");
    this->AddOpcode(2, 0x0F, 0x9F, HE_OPEXT_NONE, "setg %s", "setg %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if greater");
    this->AddOpcode(2, 0x0F, 0x9D, HE_OPEXT_NONE, "setge %s", "setge %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if greater or equal");
    this->AddOpcode(2, 0x0F, 0x9C, HE_OPEXT_NONE, "setl %s", "setl %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if less");
    this->AddOpcode(2, 0x0F, 0x9E, HE_OPEXT_NONE, "setle %s", "setle %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if less or equal");
    this->AddOpcode(2, 0x0F, 0x95, HE_OPEXT_NONE, "setne %s", "setne %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if not equal");
    this->AddOpcode(2, 0x0F, 0x91, HE_OPEXT_NONE, "setno %s", "setno %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if not overflow");
    this->AddOpcode(2, 0x0F, 0x9B, HE_OPEXT_NONE, "setnp %s", "setnp %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if parity odd");
    this->AddOpcode(2, 0x0F, 0x99, HE_OPEXT_NONE, "setns %s", "setns %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if not signed");
    this->AddOpcode(2, 0x0F, 0x90, HE_OPEXT_NONE, "seto %s", "seto %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if overflow");
    this->AddOpcode(2, 0x0F, 0x9A, HE_OPEXT_NONE, "setp %s", "setp %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if parity even");
    this->AddOpcode(2, 0x0F, 0x98, HE_OPEXT_NONE, "sets %s", "sets %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Set byte if signed");
    // sgdt/sgit
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0000, "sgdt %s", "sgdt %s", true, PT_REG_MEM, BIT48, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store global descriptor table");
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0001, "sidt %s", "sidt %s", true, PT_REG_MEM, BIT48, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store interrupt descriptor table");
    // shld
    this->AddOpcode(2, 0x0F, 0xA4, HE_OPEXT_NONE, "shld %s, %s, %s", "shld %s, %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_IMMVAL, BIT8, "Double precision shift left");
    this->AddOpcode(2, 0x0F, 0xA5, HE_OPEXT_NONE, "shld %s, %s, cl", "shld %s, %s, cl", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Double precision shift left");
    // shrd
    this->AddOpcode(2, 0x0F, 0xAC, HE_OPEXT_NONE, "shrd %s, %s, %s", "shrd %s, %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_IMMVAL, BIT8, "Double precision shift right");
    this->AddOpcode(2, 0x0F, 0xAD, HE_OPEXT_NONE, "shrd %s, %s, cl", "shrd %s, %s, cl", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Double precision shift right");
    // sldt
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0000, "sldt %s", "sldt %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store Local Descriptor Table Register");
    // smsw
    this->AddOpcode(2, 0x0F, 0x01, HE_OPEXT_0100, "smsw %s", "smsw %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store Machine Status Word");
    // str
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0001, "str %s", "str %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Store Task Register");
    // ver
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0100, "verr %s", "verr %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Verify segment for reading");
    this->AddOpcode(2, 0x0F, 0x00, HE_OPEXT_0101, "verw %s", "verw %s", true, PT_REG_MEM, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Verify segment for writing");
    // wbinvd
    this->AddOpcode(2, 0x0F, 0x09, "wbinvd", "wbinvd", "Write back and invalidate cache");
    // wrmsr
    this->AddOpcode(2, 0x0F, 0x30, "wrmsr", "wrmsr", "Write to model-specific register");
    // xadd
    this->AddOpcode(2, 0x0F, 0xC0, HE_OPEXT_NONE, "xadd %s, %s", "xadd %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Exchange and add");
    this->AddOpcode(2, 0x0F, 0xC1, HE_OPEXT_NONE, "xadd %s, %s", "xadd %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Exchange and add");
    // MMX INSTRUCTION SET (32 bit only)
    // emms
    this->AddOpcode(2, 0x0F, 0x77, "emms", "emms", "Empty MMX state");
    // movd
    this->AddOpcode(2, 0x0F, 0x6E, HE_OPEXT_NONE, "movd %s, %s", "movd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT32, PT_NONE, DONTCARE, "Move 32 bits");
    this->AddOpcode(2, 0x0F, 0x7E, HE_OPEXT_NONE, "movd %s, %s", "movd %s, %s", true, PT_REG_MEM, BIT32, PT_REG, BIT64128, PT_NONE, DONTCARE, "Move 32 bits");
    // moveq
    this->AddOpcode(2, 0x0F, 0x6F, HE_OPEXT_NONE, "movq %s, %s", "movd %s, %s", true, PT_REG, BIT64, PT_REG_MEM, BIT64, PT_NONE, DONTCARE, "Move 64 bits");
    this->AddOpcode(2, 0x0F, 0x7F, HE_OPEXT_NONE, "movq %s, %s", "movd %s, %s", true, PT_REG_MEM, BIT64, PT_REG, BIT64, PT_NONE, DONTCARE, "Move 64 bits");
    // packsswb/dw
    this->AddOpcode(2, 0x0F, 0x63, HE_OPEXT_NONE, "packsswb %s, %s", "packsswb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Pack with signed saturation (byte)");
    this->AddOpcode(2, 0x0F, 0x6B, HE_OPEXT_NONE, "packssdw %s, %s", "packssdw %s, %s", true, PT_REG_MEM, BIT64128, PT_REG, BIT64128, PT_NONE, DONTCARE, "Pack with signed saturation (word)");
    // packuswb
    this->AddOpcode(2, 0x0F, 0x67, HE_OPEXT_NONE, "packuswb %s, %s", "packuswb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Pack with unsigned saturation");
    // padd
    this->AddOpcode(2, 0x0F, 0xFC, HE_OPEXT_NONE, "paddb %s, %s", "paddb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add (byte)");
    this->AddOpcode(2, 0x0F, 0xFD, HE_OPEXT_NONE, "paddw %s, %s", "paddw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add (word)");
    this->AddOpcode(2, 0x0F, 0xFE, HE_OPEXT_NONE, "paddd %s, %s", "paddd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add (dword)");
    this->AddOpcode(2, 0x0F, 0xD4, HE_OPEXT_NONE, "paddq %s, %s", "paddq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add (qword)");
    // padds
    this->AddOpcode(2, 0x0F, 0xEC, HE_OPEXT_NONE, "paddsb %s, %s", "paddsb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add with saturation (byte)");
    this->AddOpcode(2, 0x0F, 0xED, HE_OPEXT_NONE, "paddsw %s, %s", "paddsw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add with saturation (word)");
    // paddus
    this->AddOpcode(2, 0x0F, 0xDC, HE_OPEXT_NONE, "paddusb %s, %s", "paddusb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add unsigned with saturation (byte)");
    this->AddOpcode(2, 0x0F, 0xDD, HE_OPEXT_NONE, "paddusw %s, %s", "paddusw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed add unsigned with saturation (word)");
    // pand
    this->AddOpcode(2, 0x0F, 0xDB, HE_OPEXT_NONE, "pand %s, %s", "pand %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Bitwise logical AND");
    // pandn
    this->AddOpcode(2, 0x0F, 0xDF, HE_OPEXT_NONE, "pandn %s, %s", "pandn %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Bitwise logical AND NOT");
    // pcmpeq
    this->AddOpcode(2, 0x0F, 0x74, HE_OPEXT_NONE, "pcmpeqb %s, %s", "pcmpeqb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for equal (byte)");
    this->AddOpcode(2, 0x0F, 0x75, HE_OPEXT_NONE, "pcmpeqw %s, %s", "pcmpeqw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for equal (word)");
    this->AddOpcode(2, 0x0F, 0x76, HE_OPEXT_NONE, "pcmpeqd %s, %s", "pcmpeqd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for equal (dword)");
    // pcmpgt
    this->AddOpcode(2, 0x0F, 0x64, HE_OPEXT_NONE, "pcmpgtb %s, %s", "pcmpgtb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for greater than (byte)");
    this->AddOpcode(2, 0x0F, 0x65, HE_OPEXT_NONE, "pcmpgtw %s, %s", "pcmpgtw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for greater than (word)");
    this->AddOpcode(2, 0x0F, 0x66, HE_OPEXT_NONE, "pcmpgtd %s, %s", "pcmpgtd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed compare for greater than (dword)");
    // pmaddwd
    this->AddOpcode(2, 0x0F, 0xF5, HE_OPEXT_NONE, "pmaddwd %s, %s", "pmaddwd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed multiply and add");
    // pmulhw
    this->AddOpcode(2, 0x0F, 0xE5, HE_OPEXT_NONE, "pmulhw %s, %s", "pmulhw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed multiply high (word)");
    // pmullw
    this->AddOpcode(2, 0x0F, 0xD5, HE_OPEXT_NONE, "pmullw %s, %s", "pmullw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed multiply low (word)");
    // por
    this->AddOpcode(2, 0x0F, 0xEB, HE_OPEXT_NONE, "por %s, %s", "por %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Bitwise logical OR");
    // psll
    this->AddOpcode(2, 0x0F, 0xF1, HE_OPEXT_NONE, "psllw %s, %s", "psllw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift left logical (word)");
    this->AddOpcode(2, 0x0F, 0x71, HE_OPEXT_0110, "psllw %s, %s", "psllw %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift left logical (word)");
    this->AddOpcode(2, 0x0F, 0xF2, HE_OPEXT_NONE, "pslld %s, %s", "pslld %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift left logical (dword)");
    this->AddOpcode(2, 0x0F, 0x72, HE_OPEXT_0110, "pslld %s, %s", "pslld %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift left logical (dword)");
    this->AddOpcode(2, 0x0F, 0xF3, HE_OPEXT_NONE, "psllq %s, %s", "psllq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift left logical (qword)");
    this->AddOpcode(2, 0x0F, 0x73, HE_OPEXT_0110, "psllq %s, %s", "psllq %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift left logical (qword)");
    // psra
    this->AddOpcode(2, 0x0F, 0xE1, HE_OPEXT_NONE, "psraw %s, %s", "psraw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift right arithmetic (word)");
    this->AddOpcode(2, 0x0F, 0x71, HE_OPEXT_0100, "psraw %s, %s", "psraw %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift right arithmetic (word)");
    this->AddOpcode(2, 0x0F, 0xE2, HE_OPEXT_NONE, "psrad %s, %s", "psrad %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift right arithmetic (dword)");
    this->AddOpcode(2, 0x0F, 0x72, HE_OPEXT_0100, "psrad %s, %s", "psrad %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift right arithmetic (dword)");
    // psrl
    this->AddOpcode(2, 0x0F, 0xD1, HE_OPEXT_NONE, "psrlw %s, %s", "psrlw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift right logical (word)");
    this->AddOpcode(2, 0x0F, 0x71, HE_OPEXT_0010, "psrlw %s, %s", "psrlw %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift right logical (word)");
    this->AddOpcode(2, 0x0F, 0xD2, HE_OPEXT_NONE, "psrld %s, %s", "psrld %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift right logical (dword)");
    this->AddOpcode(2, 0x0F, 0x72, HE_OPEXT_0010, "psrld %s, %s", "psrld %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift right logical (dword)");
    this->AddOpcode(2, 0x0F, 0xD3, HE_OPEXT_NONE, "psrlq %s, %s", "psrlq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed shift right logical (qword)");
    this->AddOpcode(2, 0x0F, 0x73, HE_OPEXT_0010, "psrlq %s, %s", "psrlq %s, %s", true, PT_REG, BIT64128, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Packed shift right logical (qword)");
    // psub
    this->AddOpcode(2, 0x0F, 0xF8, HE_OPEXT_NONE, "psubb %s, %s", "psubb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract (byte)");
    this->AddOpcode(2, 0x0F, 0xF9, HE_OPEXT_NONE, "psubw %s, %s", "psubw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract (word)");
    this->AddOpcode(2, 0x0F, 0xFA, HE_OPEXT_NONE, "psubd %s, %s", "psubd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract (dword)");
    // psubq
    this->AddOpcode(2, 0x0F, 0xFB, HE_OPEXT_NONE, "psubq %s, %s", "psubq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract (byte)");
    // psubs
    this->AddOpcode(2, 0x0F, 0xE8, HE_OPEXT_NONE, "psubsb %s, %s", "psubsb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract with saturation (byte)");
    this->AddOpcode(2, 0x0F, 0xE9, HE_OPEXT_NONE, "psubsw %s, %s", "psubsw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract with saturation (word)");
    // psubus
    this->AddOpcode(2, 0x0F, 0xD8, HE_OPEXT_NONE, "psubusb %s, %s", "psubusb %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract unsigned with saturation (byte)");
    this->AddOpcode(2, 0x0F, 0xD9, HE_OPEXT_NONE, "psubusw %s, %s", "psubusw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Packed subtract unsigned with saturation (word)");
    // punpckh
    this->AddOpcode(2, 0x0F, 0x68, HE_OPEXT_NONE, "punpckhbw %s, %s", "punpckhbw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Unpack high packed data");
    this->AddOpcode(2, 0x0F, 0x69, HE_OPEXT_NONE, "punpckhwd %s, %s", "punpckhwd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Unpack high packed data");
    this->AddOpcode(2, 0x0F, 0x6A, HE_OPEXT_NONE, "punpckhdq %s, %s", "punpckhdq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Unpack high packed data");
    // punpckl
    this->AddOpcode(2, 0x0F, 0x60, HE_OPEXT_NONE, "punpcklbw %s, %s", "punpcklbw %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT32128, PT_NONE, DONTCARE, "Unpack low packed data");
    this->AddOpcode(2, 0x0F, 0x61, HE_OPEXT_NONE, "punpcklwd %s, %s", "punpcklwd %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT32128, PT_NONE, DONTCARE, "Unpack low packed data");
    this->AddOpcode(2, 0x0F, 0x62, HE_OPEXT_NONE, "punpckldq %s, %s", "punpckldq %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT32128, PT_NONE, DONTCARE, "Unpack low packed data");
    // pxor
    this->AddOpcode(2, 0x0F, 0xEF, HE_OPEXT_NONE, "pxor %s, %s", "pxor %s, %s", true, PT_REG, BIT64128, PT_REG_MEM, BIT64128, PT_NONE, DONTCARE, "Bitwise logical exclusive OR");

    // Pentium II Instruction set
    // sysenter
    this->AddOpcode(2, 0x0F, 0x34, "sysenter", "sysenter", "Fast Transition to System Call Entry Point");
    // sysexit
    this->AddOpcode(2, 0x0F, 0x35, "sysexit", "sysexit", "Fast Transition from System Call Entry Point");

    // Pentium III Instruction set
    // prefetch
    this->AddOpcode(2, 0x0F, 0x18, HE_OPEXT_0001, "prefetch0 %s", "prefetch0 %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch data");
    this->AddOpcode(2, 0x0F, 0x18, HE_OPEXT_0010, "prefetch1 %s", "prefetch1 %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch data");
    this->AddOpcode(2, 0x0F, 0x18, HE_OPEXT_0011, "prefetch2 %s", "prefetch2 %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch data");
    this->AddOpcode(2, 0x0F, 0x18, HE_OPEXT_0000, "prefetchnta %s", "prefetchnta %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch data");

    // 3DNOW INSTRUCTION SET (32 bit only)
    // femms
    this->AddOpcode(2, 0x0F, 0x0E, "femms", "femms", "Faster enter/exit MMX state");

    // prefetch
    this->AddOpcode(2, 0x0F, 0x0D, HE_OPEXT_0000, "prefetch %s", "prefetch %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch processor cache line into L1 data cache");
    this->AddOpcode(2, 0x0F, 0x0D, HE_OPEXT_0001, "prefetchw %s", "prefetchw %s", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Prefetch processor cache line into L1 data cache");

    // 1 byte opcodes
    // aaa
    this->AddOpcode(1, 0x37, 0x00, "aaa", "aaa", "ASCII adjust after addition");
    // aas
    this->AddOpcode(1, 0x3F, 0x00, "aas", "aas", "ASCII adjust after subtraction");
    // adc
    this->AddOpcode(1, 0x14, 0x00, HE_OPEXT_NONE, "adc al, %s", "adc al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x15, 0x00, HE_OPEXT_NONE, "adc ax, %s", "adc eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0010, "adc %s, %s", "adc %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0010, "adc %s, %s", "adc %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0010, "adc %s, %s", "adc %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x10, 0x00, HE_OPEXT_NONE, "adc %s, %s", "adc %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x11, 0x00, HE_OPEXT_NONE, "adc %s, %s", "adc %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x12, 0x00, HE_OPEXT_NONE, "adc %s, %s", "adc %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Add integers with carry");
    this->AddOpcode(1, 0x13, 0x00, HE_OPEXT_NONE, "adc %s, %s", "adc %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Add integers with carry");
    // add
    this->AddOpcode(1, 0x04, 0x00, HE_OPEXT_NONE, "add al, %s", "add al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x05, 0x00, HE_OPEXT_NONE, "add ax, %s", "add eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0000, "add %s, %s", "add %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0000, "add %s, %s", "add %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0000, "add %s, %s", "add %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x00, 0x00, HE_OPEXT_NONE, "add %s, %s", "add %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x01, 0x00, HE_OPEXT_NONE, "add %s, %s", "add %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x02, 0x00, HE_OPEXT_NONE, "add %s, %s", "add %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Add integers");
    this->AddOpcode(1, 0x03, 0x00, HE_OPEXT_NONE, "add %s, %s", "add %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Add integers");
    // and
    this->AddOpcode(1, 0x24, 0x00, HE_OPEXT_NONE, "and al, %s", "and al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x25, 0x00, HE_OPEXT_NONE, "and ax, %s", "and eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0100, "and %s, %s", "and %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0100, "and %s, %s", "and %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0100, "and %s, %s", "and %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x20, 0x00, HE_OPEXT_NONE, "and %s, %s", "and %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x21, 0x00, HE_OPEXT_NONE, "and %s, %s", "and %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x22, 0x00, HE_OPEXT_NONE, "and %s, %s", "and %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Logical AND");
    this->AddOpcode(1, 0x23, 0x00, HE_OPEXT_NONE, "and %s, %s", "and %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Logical AND");
    // arpl
    this->AddOpcode(1, 0x63, 0x00, HE_OPEXT_NONE, "arpl %s, %s", "arpl %s, %s", true, PT_REG_MEM, BIT16, PT_REG, BIT16, PT_NONE, DONTCARE, "Adjust requester privilege level of selector");
    // bound
    this->AddOpcode(1, 0x62, 0x00, HE_OPEXT_NONE, "bound %s, %s", "bound %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Check array index against bounds");
    // call
    this->AddOpcode(1, 0xE8, 0x00, HE_OPEXT_NONE, "call %s", "call %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call procedure");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0010, "call %s", "call %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call procedure");
    this->AddOpcode(1, 0x9A, 0x00, HE_OPEXT_NONE, "call %s", "call %s", false, PT_ABS_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call procedure");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0011, "call far %s", "call far %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call procedure");
    // cbw
    this->AddOpcode(1, 0x98, 0x00, "cbw", "cwde", "Convert byte to word");
    // cwd
    this->AddOpcode(1, 0x99, 0x00, "cwd", "cdq", "Convert word to dword");
    // clc
    this->AddOpcode(1, 0xF8, 0x00, "clc", "clc", "Clear carry flag");
    // cld
    this->AddOpcode(1, 0xFC, 0x00, "cld", "cld", "Clear direction flag");
    // cli
    this->AddOpcode(1, 0xFA, 0x00, "cli", "cli", "Clear interrupt flag");
    // cmc
    this->AddOpcode(1, 0xF5, 0x00, "cmc", "cmc", "Complement carry flag");
    // cmp
    this->AddOpcode(1, 0x3C, 0x00, HE_OPEXT_NONE, "cmp al, %s", "cmp al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x3D, 0x00, HE_OPEXT_NONE, "cmp ax, %s", "cmp eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0111, "cmp %s, %s", "cmp %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0111, "cmp %s, %s", "cmp %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0111, "cmp %s, %s", "cmp %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x38, 0x00, HE_OPEXT_NONE, "cmp %s, %s", "cmp %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x39, 0x00, HE_OPEXT_NONE, "cmp %s, %s", "cmp %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x3A, 0x00, HE_OPEXT_NONE, "cmp %s, %s", "cmp %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Compare");
    this->AddOpcode(1, 0x3B, 0x00, HE_OPEXT_NONE, "cmp %s, %s", "cmp %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Compare");
    // cmps
    this->AddOpcode(1, 0xA6, 0x00, "cmpsb", "cmpsb", "Compare string operands [es:(e)di->ds:(e)si]");
    this->AddOpcode(1, 0xA7, 0x00, "cmpsw", "cmpsd", "Compare string operands [es:(e)di->ds:(e)si]");
    // daa
    this->AddOpcode(1, 0x27, 0x00, "daa", "daa", "Decimal adjust al after addition");
    // daa
    this->AddOpcode(1, 0x2F, 0x00, "das", "das", "Decimal adjust al after subtraction");
    // dec
    this->AddOpcode(1, 0xFE, 0x00, HE_OPEXT_0001, "dec %s", "dec %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Decrement by one");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0001, "dec %s", "dec %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Decrement by one");
    this->AddOpcode(1, 0x48, 0x00, "dec ax", "dec eax", "Decrement by one");
    this->AddOpcode(1, 0x49, 0x00, "dec cx", "dec ecx", "Decrement by one");
    this->AddOpcode(1, 0x4A, 0x00, "dec dx", "dec edx", "Decrement by one");
    this->AddOpcode(1, 0x4B, 0x00, "dec bx", "dec ebx", "Decrement by one");
    this->AddOpcode(1, 0x4C, 0x00, "dec sp", "dec esp", "Decrement by one");
    this->AddOpcode(1, 0x4D, 0x00, "dec bp", "dec ebp", "Decrement by one");
    this->AddOpcode(1, 0x4E, 0x00, "dec si", "dec esi", "Decrement by one");
    this->AddOpcode(1, 0x4F, 0x00, "dec di", "dec edi", "Decrement by one");
    // div
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0110, "div %s", "div %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Unsigned divide");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0110, "div %s", "div %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Unsigned divide");
    // enter
    this->AddOpcode(1, 0xC8, 0x00, HE_OPEXT_NONE, "enter %s, %s", "enter %s, %s", false, PT_IMMVAL, BIT16, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Make stack frame for procedure parameters");
    // hlt
    this->AddOpcode(1, 0xF4, 0x00, "hlt", "hlt", "Halt");
    // idiv
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0111, "idiv %s", "idiv %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Signed divide");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0111, "idiv %s", "idiv %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Signed divide");
    // imul
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0101, "imul %s", "imul %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Signed integer multiply");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0101, "imul %s", "imul %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Signed integer multiply");
    this->AddOpcode(1, 0x6B, 0x00, HE_OPEXT_NONE, "imul %s, %s, %s", "imul %s, %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, "Signed integer multiply");
    this->AddOpcode(1, 0x69, 0x00, HE_OPEXT_NONE, "imul %s, %s, %s", "imul %s, %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, "Signed integer multiply");
    // in
    this->AddOpcode(1, 0xE4, 0x00, HE_OPEXT_NONE, "in al, %s", "in al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Input from port");
    this->AddOpcode(1, 0xE5, 0x00, HE_OPEXT_NONE, "in ax, %s", "in eax, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Input from port");
    this->AddOpcode(1, 0xEC, 0x00, "in al, dx", "in al, dx", "Input from port");
    this->AddOpcode(1, 0xED, 0x00, "in ax, dx", "in eax, dx", "Input from port");
    // inc
    this->AddOpcode(1, 0xFE, 0x00, HE_OPEXT_0000, "inc %s", "inc %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Increment by one");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0000, "inc %s", "inc %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Increment by one");
    this->AddOpcode(1, 0x40, 0x00, "inc ax", "inc eax", "Increment by one");
    this->AddOpcode(1, 0x41, 0x00, "inc cx", "inc ecx", "Increment by one");
    this->AddOpcode(1, 0x42, 0x00, "inc dx", "inc edx", "Increment by one");
    this->AddOpcode(1, 0x43, 0x00, "inc bx", "inc ebx", "Increment by one");
    this->AddOpcode(1, 0x44, 0x00, "inc sp", "inc esp", "Increment by one");
    this->AddOpcode(1, 0x45, 0x00, "inc bp", "inc ebp", "Increment by one");
    this->AddOpcode(1, 0x46, 0x00, "inc si", "inc esi", "Increment by one");
    this->AddOpcode(1, 0x47, 0x00, "inc di", "inc edi", "Increment by one");
    // ins
    this->AddOpcode(1, 0x6C, 0x00, "insb", "insb", "Input string from port");
    this->AddOpcode(1, 0x6D, 0x00, "insw", "insd", "Input string from port");
    // int
    this->AddOpcode(1, 0xCC, 0x00, "int 3", "int 3", "Call interrupt 3 (Debugger trap)");
    this->AddOpcode(1, 0xCD, 0x00, HE_OPEXT_NONE, "int %s", "int %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Call interrupt");
    this->AddOpcode(1, 0xCE, 0x00, "into", "into", "Call interrupt 4 if overflow");
    // iret
    this->AddOpcode(1, 0xCF, 0x00, "iretw", "iretd", "Return from interrupt");
    // Jcc (Conditioned jumps)
    this->AddOpcode(1, 0x70, 0x00, HE_OPEXT_NONE, "jo %s", "jo %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump on overflow ");
    this->AddOpcode(1, 0x71, 0x00, HE_OPEXT_NONE, "jno %s", "jno %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump on not overflow ");
    this->AddOpcode(1, 0x72, 0x00, HE_OPEXT_NONE, "jb %s", "jb %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if below");
    this->AddOpcode(1, 0x73, 0x00, HE_OPEXT_NONE, "jnb %s", "jnb %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not below");
    this->AddOpcode(1, 0x74, 0x00, HE_OPEXT_NONE, "je %s", "je %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if equal");
    this->AddOpcode(1, 0x75, 0x00, HE_OPEXT_NONE, "jne %s", "jne %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not equal");
    this->AddOpcode(1, 0x76, 0x00, HE_OPEXT_NONE, "jbe %s", "jbe %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if below or equal");
    this->AddOpcode(1, 0x77, 0x00, HE_OPEXT_NONE, "ja %s", "ja %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if above");
    this->AddOpcode(1, 0x78, 0x00, HE_OPEXT_NONE, "js %s", "js %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if signed (negative)");
    this->AddOpcode(1, 0x79, 0x00, HE_OPEXT_NONE, "jns %s", "jns %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not signed (positive)");
    this->AddOpcode(1, 0x7A, 0x00, HE_OPEXT_NONE, "jp %s", "jp %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if parity even");
    this->AddOpcode(1, 0x7B, 0x00, HE_OPEXT_NONE, "jnp %s", "jnp %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if parity odd");
    this->AddOpcode(1, 0x7C, 0x00, HE_OPEXT_NONE, "jl %s", "jl %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if less");
    this->AddOpcode(1, 0x7D, 0x00, HE_OPEXT_NONE, "jnl %s", "jnl %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if not less");
    this->AddOpcode(1, 0x7E, 0x00, HE_OPEXT_NONE, "jle %s", "jle %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if less or equal");
    this->AddOpcode(1, 0x7F, 0x00, HE_OPEXT_NONE, "jg %s", "jg %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump if greater");
    this->AddOpcode(1, 0xE3, 0x00, HE_OPEXT_NONE, "jcxz %s", "jecxz %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump short if (e)cx = 0");
    // jmp (Unconditioned jumps)
    this->AddOpcode(1, 0xEB, 0x00, HE_OPEXT_NONE, "jmp %s", "jmp %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump");
    this->AddOpcode(1, 0xE9, 0x00, HE_OPEXT_NONE, "jmp %s", "jmp %s", false, PT_REL_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump");
    this->AddOpcode(1, 0xEA, 0x00, HE_OPEXT_NONE, "jmp %s", "jmp %s", false, PT_ABS_ADDRESS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0100, "jmp %s", "jmp %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump");
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0101, "jmp far %s", "jmp far %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Jump");
    // lahf
    this->AddOpcode(1, 0x9F, 0x00, "lahf", "lahf", "Load flags into ah register");
    // lds/les
    this->AddOpcode(1, 0xC5, 0x00, HE_OPEXT_NONE, "lds %s, %s", "lds %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load full pointer using ds");
    this->AddOpcode(1, 0xC4, 0x00, HE_OPEXT_NONE, "les %s, %s", "les %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load full pointer using es");
    // lea
    this->AddOpcode(1, 0x8D, 0x00, HE_OPEXT_NONE, "lea %s, %s", "lea %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Load effective address");
    // leave
    this->AddOpcode(1, 0xC9, 0x00, "leave", "leave", "High level procedure exit");
    // lods
    this->AddOpcode(1, 0xAC, 0x00, "lodsb", "lodsb", "Load string operand");
    this->AddOpcode(1, 0xAD, 0x00, "lodsw", "lodsd", "Load string operand");
    // loop
    this->AddOpcode(1, 0xE2, 0x00, HE_OPEXT_NONE, "loop %s", "loop %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Loop with cx counter");
    this->AddOpcode(1, 0xE1, 0x00, HE_OPEXT_NONE, "loope %s", "loope %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Loop while equal");
    this->AddOpcode(1, 0xE0, 0x00, HE_OPEXT_NONE, "loopne %s", "loopne %s", false, PT_REL_ADDRESS, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Loop while not equal");
    // mov
    this->AddOpcode(1, 0x88, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0x89, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0x8A, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0x8B, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move data");
    // TODO(Roxxorfreak) Only r/m16 or r32/r16/m16 allowed
    this->AddOpcode(1, 0x8C, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT1632, PT_SREG, BIT16, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0x8E, 0x00, HE_OPEXT_NONE, "mov %s, %s", "mov %s, %s", true, PT_SREG, BIT16, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Move data");

    this->AddOpcode(1, 0xA0, 0x00, HE_OPEXT_NONE, "mov al, %s", "mov al, %s", false, PT_MEMOFFS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xA1, 0x00, HE_OPEXT_NONE, "mov ax, %s", "mov eax, %s", false, PT_MEMOFFS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xA2, 0x00, HE_OPEXT_NONE, "mov %s, al", "mov %s, al", false, PT_MEMOFFS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xA3, 0x00, HE_OPEXT_NONE, "mov %s, ax", "mov %s, eax", false, PT_MEMOFFS, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB0, 0x00, HE_OPEXT_NONE, "mov al, %s", "mov al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB1, 0x00, HE_OPEXT_NONE, "mov cl, %s", "mov cl, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB2, 0x00, HE_OPEXT_NONE, "mov dl, %s", "mov dl, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB3, 0x00, HE_OPEXT_NONE, "mov bl, %s", "mov bl, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB4, 0x00, HE_OPEXT_NONE, "mov ah, %s", "mov ah, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB5, 0x00, HE_OPEXT_NONE, "mov ch, %s", "mov ch, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB6, 0x00, HE_OPEXT_NONE, "mov dh, %s", "mov dh, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB7, 0x00, HE_OPEXT_NONE, "mov bh, %s", "mov bh, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB8, 0x00, HE_OPEXT_NONE, "mov ax, %s", "mov eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xB9, 0x00, HE_OPEXT_NONE, "mov cx, %s", "mov ecx, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBA, 0x00, HE_OPEXT_NONE, "mov dx, %s", "mov edx, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBB, 0x00, HE_OPEXT_NONE, "mov bx, %s", "mov ebx, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBC, 0x00, HE_OPEXT_NONE, "mov sp, %s", "mov esp, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBD, 0x00, HE_OPEXT_NONE, "mov bp, %s", "mov ebp, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBE, 0x00, HE_OPEXT_NONE, "mov si, %s", "mov esi, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xBF, 0x00, HE_OPEXT_NONE, "mov di, %s", "mov edi, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xC6, 0x00, HE_OPEXT_0000, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Move data");
    this->AddOpcode(1, 0xC7, 0x00, HE_OPEXT_0000, "mov %s, %s", "mov %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Move data");
    // movs
    this->AddOpcode(1, 0xA4, 0x00, "movsb", "movsb", "Move data from string to string");
    this->AddOpcode(1, 0xA5, 0x00, "movsw", "movsd", "Move data from string to string");
    // mul
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0100, "mul %s", "mul %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Unsigned integer multiply");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0100, "mul %s", "mul %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Unsigned integer multiply");
    // neg
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0011, "neg %s", "neg %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Negate (Two's Complement)");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0011, "neg %s", "neg %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Negate (Two's Complement)");
    // nop
    this->AddOpcode(1, 0x90, 0x00, "nop", "nop", "No operation");
    // neg
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0010, "not %s", "not %s", true, PT_REG_MEM, BIT8, PT_NONE, BIT8, PT_NONE, DONTCARE, "Negate (One's Complement)");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0010, "not %s", "not %s", true, PT_REG_MEM, BIT1632, PT_NONE, BIT1632, PT_NONE, DONTCARE, "Negate (One's Complement)");
    // or
    this->AddOpcode(1, 0x0C, 0x00, HE_OPEXT_NONE, "or al, %s", "or al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x0D, 0x00, HE_OPEXT_NONE, "or ax, %s", "or eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0001, "or %s, %s", "or %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0001, "or %s, %s", "or %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0001, "or %s, %s", "or %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x08, 0x00, HE_OPEXT_NONE, "or %s, %s", "or %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x09, 0x00, HE_OPEXT_NONE, "or %s, %s", "or %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x0A, 0x00, HE_OPEXT_NONE, "or %s, %s", "or %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Logical OR");
    this->AddOpcode(1, 0x0B, 0x00, HE_OPEXT_NONE, "or %s, %s", "or %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Logical OR");
    // out
    this->AddOpcode(1, 0xE6, 0x00, HE_OPEXT_NONE, "out %s, al", "out %s, al", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Input from port");
    this->AddOpcode(1, 0xE7, 0x00, HE_OPEXT_NONE, "out %s, ax", "out %s, eax", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Input from port");
    this->AddOpcode(1, 0xEE, 0x00, "out dx, al", "out dx, al", "Input from port");
    this->AddOpcode(1, 0xEF, 0x00, "out dx, ax", "out dx, eax", "Input from port");
    // outs
    this->AddOpcode(1, 0x6E, 0x00, "outsb", "outsb", "Output string to port");
    this->AddOpcode(1, 0x6F, 0x00, "outsw", "outsd", "Output string to port");
    // pop
    this->AddOpcode(1, 0x8F, 0x00, HE_OPEXT_0000, "pop %s", "pop %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Pop a word from the stack");
    this->AddOpcode(1, 0x58, 0x00, "pop ax", "pop eax", "Pop a word from the stack");
    this->AddOpcode(1, 0x59, 0x00, "pop cx", "pop ecx", "Pop a word from the stack");
    this->AddOpcode(1, 0x5A, 0x00, "pop dx", "pop edx", "Pop a word from the stack");
    this->AddOpcode(1, 0x5B, 0x00, "pop bx", "pop ebx", "Pop a word from the stack");
    this->AddOpcode(1, 0x5C, 0x00, "pop sp", "pop esp", "Pop a word from the stack");
    this->AddOpcode(1, 0x5D, 0x00, "pop bp", "pop ebp", "Pop a word from the stack");
    this->AddOpcode(1, 0x5E, 0x00, "pop si", "pop esi", "Pop a word from the stack");
    this->AddOpcode(1, 0x5F, 0x00, "pop di", "pop edi", "Pop a word from the stack");
    this->AddOpcode(1, 0x1F, 0x00, "pop ds", "pop ds", "Pop a word from the stack");
    this->AddOpcode(1, 0x07, 0x00, "pop es", "pop es", "Pop a word from the stack");
    this->AddOpcode(1, 0x17, 0x00, "pop ss", "pop ss", "Pop a word from the stack");
    // popa
    this->AddOpcode(1, 0x61, 0x00, "popa", "popad", "Pop all registers");
    // popf
    this->AddOpcode(1, 0x9D, 0x00, "popf", "popfd", "Pop stack into flag register");
    // push
    this->AddOpcode(1, 0xFF, 0x00, HE_OPEXT_0110, "pop %s", "pop %s", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Push operand onto the stack");
    this->AddOpcode(1, 0x50, 0x00, "push ax", "push eax", "Push operand onto the stack");
    this->AddOpcode(1, 0x51, 0x00, "push cx", "push ecx", "Push operand onto the stack");
    this->AddOpcode(1, 0x52, 0x00, "push dx", "push edx", "Push operand onto the stack");
    this->AddOpcode(1, 0x53, 0x00, "push bx", "push ebx", "Push operand onto the stack");
    this->AddOpcode(1, 0x54, 0x00, "push sp", "push esp", "Push operand onto the stack");
    this->AddOpcode(1, 0x55, 0x00, "push bp", "push ebp", "Push operand onto the stack");
    this->AddOpcode(1, 0x56, 0x00, "push si", "push esi", "Push operand onto the stack");
    this->AddOpcode(1, 0x57, 0x00, "push di", "push edi", "Push operand onto the stack");
    this->AddOpcode(1, 0x6A, 0x00, HE_OPEXT_NONE, "push %s", "push %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Push operand onto the stack");
    this->AddOpcode(1, 0x68, 0x00, HE_OPEXT_NONE, "push %s", "push %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Push operand onto the stack");
    this->AddOpcode(1, 0x0E, 0x00, "push cs", "push cs", "Push operand onto the stack");
    this->AddOpcode(1, 0x1E, 0x00, "push ds", "push ds", "Push operand onto the stack");
    this->AddOpcode(1, 0x06, 0x00, "push es", "push es", "Push operand onto the stack");
    this->AddOpcode(1, 0x16, 0x00, "push ss", "push ss", "Push operand onto the stack");
    // pusha
    this->AddOpcode(1, 0x60, 0x00, "pusha", "pushd", "Push all registers");
    // pushf
    this->AddOpcode(1, 0x9C, 0x00, "pushf", "pushd", "Push flags register onto the stack");
    // rcl
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0010, "rcl %s, 1", "rcl %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left through carry");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0010, "rcl %s, cl", "rcl %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left through carry");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0010, "rcl %s, %s", "rcl %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate left through carry");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0010, "rcl %s, 1", "rcl %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left through carry");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0010, "rcl %s, cl", "rcl %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left through carry");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0010, "rcl %s, %s", "rcl %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate left through carry");
    // rcr
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0011, "rcr %s, 1", "rcr %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right through carry");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0011, "rcr %s, cl", "rcr %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right through carry");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0011, "rcr %s, %s", "rcr %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate right through carry");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0011, "rcr %s, 1", "rcr %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right through carry");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0011, "rcr %s, cl", "rcr %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right through carry");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0011, "rcr %s, %s", "rcr %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate right through carry");
    // rol
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0000, "rol %s, 1", "rol %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0000, "rol %s, cl", "rol %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0000, "rol %s, %s", "rol %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate left");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0000, "rol %s, 1", "rol %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0000, "rol %s, cl", "rol %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate left");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0000, "rol %s, %s", "rol %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate left");
    // ror
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0001, "ror %s, 1", "ror %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0001, "ror %s, cl", "ror %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0001, "ror %s, %s", "ror %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate right");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0001, "ror %s, 1", "ror %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0001, "ror %s, cl", "ror %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Rotate right");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0001, "ror %s, %s", "ror %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Rotate right");
    // ret
    this->AddOpcode(1, 0xC3, 0x00, "ret", "ret", "Return from procedure");
    this->AddOpcode(1, 0xCB, 0x00, "retf", "retf", "Return from procedure");
    this->AddOpcode(1, 0xC2, 0x00, HE_OPEXT_NONE, "ret %s", "ret %s", false, PT_IMMVAL, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Return from procedure");
    this->AddOpcode(1, 0xCA, 0x00, HE_OPEXT_NONE, "retf %s", "retf %s", false, PT_IMMVAL, BIT16, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Return from procedure");
    // sahf
    this->AddOpcode(1, 0x9E, 0x00, "sahf", "sahf", "Store ah into flags");
    // sal
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0100, "shl %s, 1", "shl %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift left through carry");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0100, "shl %s, cl", "shl %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift left through carry");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0100, "shl %s, %s", "shl %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift left through carry");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0100, "shl %s, 1", "shl %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift left through carry");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0100, "shl %s, cl", "shl %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift left through carry");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0100, "shl %s, %s", "shl %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift left through carry");
    // sar
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0111, "sar %s, 1", "sar %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right through carry");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0111, "sar %s, cl", "sar %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right through carry");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0111, "sar %s, %s", "sar %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift right through carry");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0111, "sar %s, 1", "sar %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right through carry");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0111, "sar %s, cl", "sar %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right through carry");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0111, "sar %s, %s", "sar %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift right through carry");
    // shr
    this->AddOpcode(1, 0xD0, 0x00, HE_OPEXT_0101, "shr %s, 1", "shr %s, 1", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right");
    this->AddOpcode(1, 0xD2, 0x00, HE_OPEXT_0101, "shr %s, cl", "shr %s, cl", true, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right");
    this->AddOpcode(1, 0xC0, 0x00, HE_OPEXT_0101, "shr %s, %s", "shr %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift right");
    this->AddOpcode(1, 0xD1, 0x00, HE_OPEXT_0101, "shr %s, 1", "shr %s, 1", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right");
    this->AddOpcode(1, 0xD3, 0x00, HE_OPEXT_0101, "shr %s, cl", "shr %s, cl", true, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Shift right");
    this->AddOpcode(1, 0xC1, 0x00, HE_OPEXT_0101, "shr %s, %s", "shr %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Shift right");
    // sbb
    this->AddOpcode(1, 0x1C, 0x00, HE_OPEXT_NONE, "sbb al, %s", "sbb al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x1D, 0x00, HE_OPEXT_NONE, "sbb ax, %s", "sbb eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0011, "sbb %s, %s", "sbb %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0011, "sbb %s, %s", "sbb %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0011, "sbb %s, %s", "sbb %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x18, 0x00, HE_OPEXT_NONE, "sbb %s, %s", "sbb %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x19, 0x00, HE_OPEXT_NONE, "sbb %s, %s", "sbb %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x1A, 0x00, HE_OPEXT_NONE, "sbb %s, %s", "sbb %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Subtract integers with borrow");
    this->AddOpcode(1, 0x1B, 0x00, HE_OPEXT_NONE, "sbb %s, %s", "sbb %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Subtract integers with borrow");
    // scas
    this->AddOpcode(1, 0xAE, 0x00, "scasb", "scasb", "Compare string data");
    this->AddOpcode(1, 0xAF, 0x00, "scasw", "scasd", "Compare string data");
    // stc
    this->AddOpcode(1, 0xF9, 0x00, "stc", "stc", "Set carry flag");
    // std
    this->AddOpcode(1, 0xFD, 0x00, "std", "std", "Set direction flag");
    // sti
    this->AddOpcode(1, 0xFB, 0x00, "sti", "sti", "Set interrupt flag");
    // stos
    this->AddOpcode(1, 0xAA, 0x00, "stosb", "stosb", "Store string data");
    this->AddOpcode(1, 0xAB, 0x00, "stosw", "stosd", "Store string data");
    // sub
    this->AddOpcode(1, 0x2C, 0x00, HE_OPEXT_NONE, "sub al, %s", "sub al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x2D, 0x00, HE_OPEXT_NONE, "sub ax, %s", "sub eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0101, "sub %s, %s", "sub %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0101, "sub %s, %s", "sub %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0101, "sub %s, %s", "sub %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x28, 0x00, HE_OPEXT_NONE, "sub %s, %s", "sub %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x29, 0x00, HE_OPEXT_NONE, "sub %s, %s", "sub %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x2A, 0x00, HE_OPEXT_NONE, "sub %s, %s", "sub %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Subtract integers");
    this->AddOpcode(1, 0x2B, 0x00, HE_OPEXT_NONE, "sub %s, %s", "sub %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Subtract integers");
    // test
    this->AddOpcode(1, 0xA8, 0x00, HE_OPEXT_NONE, "test al, %s", "test al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical compare");
    this->AddOpcode(1, 0xA9, 0x00, HE_OPEXT_NONE, "test ax, %s", "test eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical compare");
    this->AddOpcode(1, 0xF6, 0x00, HE_OPEXT_0000, "test %s, %s", "test %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical compare");
    this->AddOpcode(1, 0xF7, 0x00, HE_OPEXT_0000, "test %s, %s", "test %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Logical compare");
    this->AddOpcode(1, 0x84, 0x00, HE_OPEXT_NONE, "test %s, %s", "test %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Logical compare");
    this->AddOpcode(1, 0x85, 0x00, HE_OPEXT_NONE, "test %s, %s", "test %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Logical compare");
    // wait
    this->AddOpcode(1, 0x9B, 0x00, "wait", "wait", "Wait");
    // xchg
    this->AddOpcode(1, 0x86, 0x00, HE_OPEXT_NONE, "xchg %s, %s", "xchg %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Exchange data");
    this->AddOpcode(1, 0x87, 0x00, HE_OPEXT_NONE, "xchg %s, %s", "xchg %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Exchange data");
    this->AddOpcode(1, 0x91, 0x00, "xchg cx, ax", "xchg ecx, eax", "Exchange data");
    this->AddOpcode(1, 0x92, 0x00, "xchg dx, ax", "xchg edx, eax", "Exchange data");
    this->AddOpcode(1, 0x93, 0x00, "xchg bx, ax", "xchg ebx, eax", "Exchange data");
    this->AddOpcode(1, 0x94, 0x00, "xchg sp, ax", "xchg esp, eax", "Exchange data");
    this->AddOpcode(1, 0x95, 0x00, "xchg bp, ax", "xchg ebp, eax", "Exchange data");
    this->AddOpcode(1, 0x96, 0x00, "xchg si, ax", "xchg esi, eax", "Exchange data");
    this->AddOpcode(1, 0x97, 0x00, "xchg di, ax", "xchg edi, eax", "Exchange data");
    // xlat
    this->AddOpcode(1, 0xD7, 0x00, "xlatb", "xlatb", "Table look-up translation");
    // xor
    this->AddOpcode(1, 0x34, 0x00, HE_OPEXT_NONE, "xor al, %s", "xor al, %s", false, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x35, 0x00, HE_OPEXT_NONE, "xor ax, %s", "xor eax, %s", false, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x80, 0x00, HE_OPEXT_0110, "xor %s, %s", "xor %s, %s", true, PT_REG_MEM, BIT8, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x81, 0x00, HE_OPEXT_0110, "xor %s, %s", "xor %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT1632, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x83, 0x00, HE_OPEXT_0110, "xor %s, %s", "xor %s, %s", true, PT_REG_MEM, BIT1632, PT_IMMVAL, BIT8, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x30, 0x00, HE_OPEXT_NONE, "xor %s, %s", "xor %s, %s", true, PT_REG_MEM, BIT8, PT_REG, BIT8, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x31, 0x00, HE_OPEXT_NONE, "xor %s, %s", "xor %s, %s", true, PT_REG_MEM, BIT1632, PT_REG, BIT1632, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x32, 0x00, HE_OPEXT_NONE, "xor %s, %s", "xor %s, %s", true, PT_REG, BIT8, PT_REG_MEM, BIT8, PT_NONE, DONTCARE, "Logical, exclusive OR");
    this->AddOpcode(1, 0x33, 0x00, HE_OPEXT_NONE, "xor %s, %s", "xor %s, %s", true, PT_REG, BIT1632, PT_REG_MEM, BIT1632, PT_NONE, DONTCARE, "Logical, exclusive OR");

    // Compact vector
    this->opcode_.shrink_to_fit();
}

/**
 * Adds an opcode to the list of loaded opcodes.
 * @param length The length of the opcode in bytes.
 * @param opcode1 The first byte of the opcode.
 * @param opcode2 The second byte of the opcode.
 * @param opcode_ext The third byte of the opcode if length = 3, additional bits if length < 3 or HE_OPEXT_NONE otherwise.
 * @param asm16 The 16bit opcode name.
 * @param asm32 The 32bit opcode name.
 * @param mod_rm_present Flag: true if a Mod R/M byte is present.
 * @param pt1 The type of the first parameter.
 * @param ps1 The size of the first parameter.
 * @param pt2 The type of the second parameter.
 * @param ps2 The size of the second parameter.
 * @param pt3 The type of the third parameter.
 * @param ps3 The size of the third parameter.
 * @param comment The description of the opcode.
 */
void TAsmOpcodeManager::AddOpcode(std::size_t length, unsigned char opcode1, unsigned char opcode2, int16_t opcode_ext, const char* asm16, const char* asm32,
    bool mod_rm_present, int16_t pt1, std::size_t ps1, int16_t pt2, std::size_t ps2, int16_t pt3, std::size_t ps3, const char* comment)
{
    TOpcode opcode;

    // Store the opcode data
    opcode.length = length;
    opcode.opcode_byte[0] = opcode1;
    opcode.opcode_byte[1] = opcode2;
    opcode.opcode_ext = opcode_ext;
    opcode.asm16 = asm16;
    opcode.asm32 = asm32;
    opcode.has_mod_rm = mod_rm_present;
    opcode.param_type[0] = pt1;
    opcode.param_size[0] = ps1;
    opcode.param_type[1] = pt2;
    opcode.param_size[1] = ps2;
    opcode.param_type[2] = pt3;
    opcode.param_size[2] = ps3;
    opcode.comment = comment;

    // Add the opcode
    this->opcode_.push_back(opcode);
}

/**
 * Adds an opcode to the list of loaded opcodes.
 * @param length The length of the opcode in bytes.
 * @param opcode1 The first byte of the opcode.
 * @param opcode2 The second byte of the opcode.
 * @param asm16 The 16bit opcode name.
 * @param asm32 The 32bit opcode name.
 * @param comment The description of the opcode.
 */
void TAsmOpcodeManager::AddOpcode(std::size_t length, unsigned char opcode1, unsigned char opcode2, const char* asm16, const char* asm32, const char* comment)
{
    TOpcode opcode;

    // Store the opcode data
    opcode.length = length;
    opcode.opcode_byte[0] = opcode1;
    opcode.opcode_byte[1] = opcode2;
    opcode.asm16 = asm16;
    opcode.asm32 = asm32;
    opcode.comment = comment;

    // Set the defaults
    opcode.opcode_ext = HE_OPEXT_NONE;
    opcode.has_mod_rm = false;
    opcode.param_type[0] = PT_NONE;
    opcode.param_size[0] = DONTCARE;
    opcode.param_type[1] = PT_NONE;
    opcode.param_size[1] = DONTCARE;
    opcode.param_type[2] = PT_NONE;
    opcode.param_size[2] = DONTCARE;

    // Add the opcode
    this->opcode_.push_back(opcode);
}

/**
 * Checks if the specified buffer contains the specified opcode.
 * @param buffer The buffer to check for the opcode.
 * @param opcode The opcode to check.
 * @return true if the buffer contains the specified opcode, false otherwise.
 */
bool TAsmOpcodeManager::IsOpcode(const TAsmBuffer* buffer, const TOpcode* opcode) noexcept
{
    if (opcode->length == 3)  // For SIMD (or other 3 byte) instructions
    {
        // 3 byte opcode
        if (buffer->GetByte(0) != opcode->opcode_byte[0]) return false;
        if (buffer->GetByte(1) != opcode->opcode_byte[1]) return false;
        if (buffer->GetByte(2) != opcode->opcode_ext) return false;
    }
    else
    {
        // 1 or 2 byte opcode (maybe with extension)
        if (buffer->GetByte(0) != opcode->opcode_byte[0]) return false;
        if (opcode->length == 2)
        {
            if (buffer->GetByte(1) != opcode->opcode_byte[1]) return false;
        }

        // Check for opcode extension
        if (opcode->opcode_ext != HE_OPEXT_NONE)
        {
            auto c = static_cast<int16_t>((buffer->GetByte(opcode->length) >> 3) & 0x07);
            if (c != opcode->opcode_ext) return false;
        }
    }

    // Opcode identified, return success
    return true;
}

/**
 * Tries to identify the opcode at the current instruction pointer in the machine code buffer.
 * @param buffer The machine code buffer to read the opcode bytes from.
 * @return A pointer to the identified opcode or nullptr if the opcode could not be identified.
 */
TOpcode* TAsmOpcodeManager::FindOpcode(TAsmBuffer* buffer)
{

    // Process all opcodes
    for (auto& oc : this->opcode_)
    {
        // Try to identify opcode at the current position of the instruction pointer
        if (this->IsOpcode(buffer, &oc) == true)
        {
            // Adjust the pointer by the number of opcode bytes
            buffer->AdvanceInstructionPointer(oc.length);

            // Return the identified opcode
            return &oc;
        }
    }

    // No opcode identified
    return nullptr;
}
