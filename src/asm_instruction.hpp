// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_ASM_INSTRUCTION_HPP_

    // Header included
    #define HEDIT_SRC_ASM_INSTRUCTION_HPP_

    // The maximum number of machine code bytes in an assembler instruction.
    // A single instruction should not exceed 15 bytes without redundant prefixes.
    // (see Intel Instruction Set Reference, Vol. 3B, Chapter 22.25)
    constexpr std::size_t HE_MAX_INSTRUCTION_LENGTH = 20;

    // Define the Opcode Prefix codes (bit field, created from prefix bytes of Group 1)
    // (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.1)
    constexpr uint16_t HE_OPCODE_PREFIX_NONE    = 0x00;     //!< No opcode prefix.
    constexpr uint16_t HE_OPCODE_PREFIX_REP     = 0x01;     //!< Opcode prefix: Repeat.
    constexpr uint16_t HE_OPCODE_PREFIX_REPNE   = 0x02;     //!< Opcode prefix: Repeat while not equal.
    constexpr uint16_t HE_OPCODE_PREFIX_LOCK    = 0x04;     //!< Opcode prefix: Lock.

    // Define the Segment Override Codes (created from prefix bytes of Group 2)
    // (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.1)
    enum class TSegmentOverride : int32_t {
        NONE,   //!< No segment override
        CS,     //!< Segment override: Code Segment
        SS,     //!< Segment override: Stack Segment
        DS,     //!< Segment override: Data Segment
        ES,     //!< Segment override: Extra Segment
        FS,     //!< Segment override: FS
        GS      //!< Segment override: GS
    };

    /**
     * @brief The structure that holds the REG, MOD and R/M values of the ModR/M byte.
     * (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.5)
     */
    struct TModRMByte
    {
        uint16_t mod = { 0 };   //!< The MOD value, bits 6-7 in the ModR/M byte.
        uint16_t reg = { 0 };   //!< The REG value, bits 3-5 in the ModR/M byte.
        uint16_t rm = { 0 };    //!< The R/M value, bits 0-2 in the ModR/M byte.
    };

    /**
     * @brief The structure that holds the opcode data and the instruction data of a single instruction parameter.
     */
    struct TInstructionParameter
    {
        int16_t type = { 0 };   //!< The parameter type (constant, see PT_... constants)
        uint16_t size = { 0 };  //!< The allowed size(s) of the parameter (constant, see Operand sizes)
        TString value;          //!< The (decoded) value of the parameter for a specific instruction.
    };

    /**
     * @brief The class that holds all data of an assembler instruction (that was or is being disassembled).
     */
    class TAsmInstruction
    {
    private:
        int64_t address_ = {};                                          //!< The absolute address of the instruction.
        std::size_t length_ = {};                                       //!< The length of the machine code of the instruction (in bytes).
        const TOpcode* opcode_ = {};                                    //!< The pointer to the opcode (of the loaded instruction set), or nullptr, if none (e.g. for data bytes).
        TSegmentOverride segment_override_ = {};                        //!< The segment override code, if specified via prefix (Group 2).
        unsigned char machine_code_[HE_MAX_INSTRUCTION_LENGTH] = {};    //!< The raw machine code bytes of the instruction (not NULL terminated).
    public:
        TString assembler_code_;                                        //!< The disassembled code (assembler mnemonics).
        int32_t opcode_prefix_ = { HE_OPCODE_PREFIX_NONE };             //!< The lock and repeat prefixes (Group 1, 0xF0/0xF2/0xF3) as bit mask (see HE_OPCODE_PREFIX_).
        bool address_size_prefix_ = {};                                 //!< The flag that states, if (at least one) address size prefix (Group 4, 0x67) was specified.
        bool operand_size_prefix_ = {};                                 //!< The flag that states, if (at least one) operand size prefix (Group 3, 0x66) was specified.
        TModRMByte mod_rm_ = {};                                        //!< The decoded ModR/M byte, see TModRMByte.
        TInstructionParameter param_[3] = {};                           //!< The array for the (up to three) parameters of the instruction.
    public:
        void Clear();
        void SetAddress(int64_t address);
        int64_t GetAddress();
        void SetSegmentOverride(TSegmentOverride segment_override);
        TSegmentOverride GetSegmentOverride();
        TString GetSegmentOverrideName();
        void SetOpcode(const TOpcode* opcode);
        const TOpcode* GetOpcode();
        void SetMachineCode(TAsmBuffer* buffer);
        const unsigned char* GetMachineCode();
        std::size_t GetLength();
        void DecodeModRMByte(unsigned char code_byte);
    };

#endif  // HEDIT_SRC_ASM_INSTRUCTION_HPP_
