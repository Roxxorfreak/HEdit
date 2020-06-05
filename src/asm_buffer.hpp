// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_ASM_BUFFER_HPP_

    // Header included
    #define HEDIT_SRC_ASM_BUFFER_HPP_

    // Define the immediate value / displacement sizes (in bytes)
    // These values are used to calculate the number of bytes during disassembling.
    enum class TValueSize : std::size_t {
        BYTE = 1,   //!< The value size: byte
        WORD = 2,   //!< The value size: word
        DWORD = 4,  //!< The value size: double word
        QWORD = 8   //!< The value size: quad word
    };

    // The macro for accessing the value of a TValueSize
    #define EV(x) static_cast<std::size_t>(x)

    /**
     * @brief The buffer class that manages a byte buffer for machine code to be used with the Disassembler.
     */
    class TAsmBuffer
    {
    private:
        int64_t base_address_ = {};                     //!< The absolute base address of the machine code in the buffer.
        std::size_t buffer_size_ = {};                  //!< The size of the buffer for the data to disassemble.
        std::size_t code_length_ = {};                  //!< The length of the machine code actually stored in the buffer.
        std::size_t instruction_pointer_ = {};          //!< The offset of the current instruction, relative to the buffer start.
        std::unique_ptr<unsigned char[]> buffer_ = {};  //!< The buffer for the data to disassemble.
    public:
        explicit TAsmBuffer(std::size_t size);
        TAsmBuffer(const TAsmBuffer& source) = delete;
        TAsmBuffer& operator=(const TAsmBuffer& source) = delete;
        TAsmBuffer(TAsmBuffer&&) = delete;
        TAsmBuffer& operator=(TAsmBuffer&&) = delete;
        std::size_t GetSize();
        unsigned char* GetBuffer();
        void SetCodeLength(std::size_t code_length);
        std::size_t GetCodeLength();
        void SetBaseAddress(int64_t base_address);
        int64_t GetBaseAddress();
        int64_t GetCurrentAddress();
        void LoadFromFile(TFile* file, int64_t base_address);
        void ResetInstructionPointer();
        void AdvanceInstructionPointer(std::size_t offset);
        std::size_t GetInstructionPointer();
        unsigned char GetByte(std::size_t offset = 0);
        unsigned char ReadByte();
        int64_t ReadValue(TValueSize value_size) noexcept;
    };

#endif  // HEDIT_SRC_ASM_BUFFER_HPP_
