// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_ASM_OPCODE_MANAGER_HPP_

    // Header included
    #define HEDIT_SRC_ASM_OPCODE_MANAGER_HPP_

    // Operand sizes
    constexpr std::size_t DONTCARE  = 0;    //!< Operand size is unused
    constexpr std::size_t BIT8      = 8;    //!< Operand size is 8 bit
    constexpr std::size_t BIT16     = 16;   //!< Operand size is 16 bit
    constexpr std::size_t BIT1632   = 17;   //!< Operand size is 16 or 32 bit
    constexpr std::size_t BIT32     = 32;   //!< Operand size is 32 bit
    constexpr std::size_t BIT32128  = 34;   //!< Operand size is 32 or 128 bit (for some instructions like "punpcklbw", "punpcklwd", "punpckldq")
    constexpr std::size_t BIT48     = 48;   //!< Operand size is 48 bit (for some instructions like "sgdt", "sidt", "lgdt" or "lidt")
    constexpr std::size_t BIT64     = 64;   //!< Operand size is 64 bit
    constexpr std::size_t BIT64128  = 65;   //!< Operand size is 64 or 128 bit
    constexpr std::size_t BIT128    = 128;  //!< Operand size is 128 bit

    // Parameter types
    constexpr int16_t PT_NONE        = 0;   //!< No parameter
    constexpr int16_t PT_REG         = 1;   //!< Register (in reg field of r/m byte)
    constexpr int16_t PT_REG_MEM     = 2;   //!< Register or memory (in mm and r/m fields of r/m byte)
    constexpr int16_t PT_SREG        = 3;   //!< Segment register (in reg field of r/m byte)
    constexpr int16_t PT_IMMVAL      = 4;   //!< Immediate Value
    constexpr int16_t PT_REL_ADDRESS = 5;   //!< Relative address
    constexpr int16_t PT_ABS_ADDRESS = 6;   //!< Absolute address (Segment:Offset)
    constexpr int16_t PT_MEMOFFS     = 7;   //!< Memory offset ([XXXX])
    constexpr int16_t PT_CREG        = 8;   //!< Control Register (Only used for mov)
    constexpr int16_t PT_DREG        = 9;   //!< Debug Register (Only used for mov)
    constexpr int16_t PT_TREG        = 10;  //!< Test Register (Only used for mov)

    // Define the opcode extensions (The value of the "reg" field in the ModR/M byte)
    // (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.3)
    constexpr int16_t HE_OPEXT_NONE = -1;  //!< No opcode extension used.
    constexpr int16_t HE_OPEXT_0000 = 0;   //!< Opcode extension: 0
    constexpr int16_t HE_OPEXT_0001 = 1;   //!< Opcode extension: 1
    constexpr int16_t HE_OPEXT_0010 = 2;   //!< Opcode extension: 2
    constexpr int16_t HE_OPEXT_0011 = 3;   //!< Opcode extension: 3
    constexpr int16_t HE_OPEXT_0100 = 4;   //!< Opcode extension: 4
    constexpr int16_t HE_OPEXT_0101 = 5;   //!< Opcode extension: 5
    constexpr int16_t HE_OPEXT_0110 = 6;   //!< Opcode extension: 6
    constexpr int16_t HE_OPEXT_0111 = 7;   //!< Opcode extension: 7

    /**
     * @brief The structure that holds all data of an opcode definition.
     */
    struct TOpcode
    {
        bool has_mod_rm = {};               //!< Flag: true if a mod r/m byte is present, false otherwise.
        unsigned char opcode_byte[2] = {};  //!< The first (two) opcode bytes.
        int16_t opcode_ext = {};            //!< The bitmask for the opcode extension or the third opcode byte.
        int16_t param_type[3] = {};         //!< The array off (up to) three parameters types for the opcode.
        std::size_t param_size[3] = {};     //!< The array of (up to) three parameters sizes for the opcode.
        std::size_t length = {};            //!< The length of the opcode in bytes (excluding the parameters).
        TString asm16;                      //!< The 16 bit opcode mnemonic.
        TString asm32;                      //!< The 32 bit opcode mnemonic.
        TString comment;                    //!< The description of the opcode.
    };


    /**
     * @brief The class that manages all opcodes.
     */
    class TAsmOpcodeManager
    {
    private:
        std::vector <TOpcode> opcode_;  //!< The array of managed opcodes.
    private:
        void LoadIntelOpcodes();
        void AddOpcode(std::size_t length, unsigned char opcode1, unsigned char opcode2, int16_t opcode_ext, const char* asm16, const char* asm32, bool mod_rm_present,
            int16_t pt1, std::size_t ps1, int16_t pt2, std::size_t ps2, int16_t pt3, std::size_t ps3, const char* comment);
        void AddOpcode(std::size_t length, unsigned char opcode1, unsigned char opcode2, const char* asm16, const char* asm32, const char* comment);
        static bool IsOpcode(TAsmBuffer* buffer, const TOpcode* opcode) noexcept;
    public:
        TAsmOpcodeManager();
        TOpcode* FindOpcode(TAsmBuffer* buffer);
    };


#endif  // HEDIT_SRC_ASM_OPCODE_MANAGER_HPP_
