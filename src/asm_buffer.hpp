// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_ASM_BUFFER_HPP_

    // Header included
    #define HEDIT_SRC_ASM_BUFFER_HPP_

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
        std::size_t GetSize() const noexcept;
        unsigned char* GetBuffer() const noexcept;
        void SetCodeLength(std::size_t code_length) noexcept;
        std::size_t GetCodeLength() const noexcept;
        void SetBaseAddress(int64_t base_address) noexcept;
        int64_t GetBaseAddress() const noexcept;
        int64_t GetCurrentAddress() const noexcept;
        void LoadFromFile(TFile* file, int64_t base_address) noexcept;
        void ResetInstructionPointer() noexcept;
        void AdvanceInstructionPointer(std::size_t offset) noexcept;
        std::size_t GetInstructionPointer() const noexcept;
        unsigned char GetByte(std::size_t offset = 0) const noexcept;
        unsigned char ReadByte() noexcept;
        int64_t ReadValue(TValueSize value_size) noexcept;
    };

#endif  // HEDIT_SRC_ASM_BUFFER_HPP_
