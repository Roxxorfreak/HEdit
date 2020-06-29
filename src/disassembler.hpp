// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_DISASSEMBLER_HPP_

    // Header included
    #define HEDIT_SRC_DISASSEMBLER_HPP_

    // The processor architecture (bit mode)
    enum class TArchitecture : int32_t {
        x86_16,    //!< 16bit (x86-16, i8086)
        x86_32,    //!< 32bit (x86-32, IA-32)
        x86_64     //!< 64bit (x86-64)
    };

    // The register names
    constexpr const char* reg_mm[] = { "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7" };
    constexpr const char* reg_xmm[] = { "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7" };
    constexpr const char* reg_sreg[] = { "es", "cs", "ss", "ds", "fs", "gs", "?", "?" };
    constexpr const char* reg_reg8[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
    constexpr const char* reg_reg16[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
    constexpr const char* reg_reg32[] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
    constexpr const char* reg_test[] = { "tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7" };
    constexpr const char* reg_debug[] = { "dr0", "dr1", "dr2", "dr3", "?", "?", "dr6", "dr7" };
    constexpr const char* reg_control[] = { "cr0", "?", "cr2", "cr3", "cr4", "?", "?", "?" };

    // Validate register count
    static_assert((sizeof(reg_mm) / sizeof(reg_mm[0])) == 8, "Invalid register count");

    /**
     * @brief The disassembler engine that can be used to disassemble byte code into readable assembler code.
     */
    class TDisassembler
    {
    private:
        TValueProcessor formatter_;
        TAsmOpcodeManager opcode_manager_;
    public:
        int32_t Disassemble(TAsmBuffer* buffer, TAsmInstruction* instruction, int32_t instruction_size, TArchitecture architecture, TNumberFormat number_format);
    private:
        void DisassembleInstruction(TAsmBuffer* buffer, TAsmInstruction* ai, TArchitecture architecture);
        void ProcessPrefixes(TAsmBuffer* buffer, TAsmInstruction* ai);
        TString DecodeModRmByte16(TAsmInstruction* ai, int32_t index, TAsmBuffer* buffer, TArchitecture architecture);
        TString DecodeModRmByte32(TAsmInstruction* ai, int32_t index, TAsmBuffer* buffer, TArchitecture architecture);
        TString GetRegisterName(TAsmInstruction* ai, uint16_t register_code, uint16_t size, TArchitecture architecture);
        TString GetOperandPointerName(TAsmInstruction* ai, uint16_t size, TArchitecture architecture);
        static bool OperandSize32(TArchitecture bit_mode, bool operand_size_prefix) noexcept;
        static bool AddressSize32(TArchitecture bit_mode, bool address_size_prefix) noexcept;
    };

#endif  // HEDIT_SRC_DISASSEMBLER_HPP_
