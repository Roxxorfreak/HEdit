// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Prepares the buffer by allocating the requested number of bytes.
 * @param size The number of bytes for the buffer.
 */
TAsmBuffer::TAsmBuffer(std::size_t size)
{
    // Allocate the buffer
    this->buffer_ = std::unique_ptr<unsigned char[]>(new unsigned char[size]);

    // Store the buffer size
    this->buffer_size_ = size;

    // Clear the memory
    memset(this->buffer_.get(), 0, this->buffer_size_);
}

/**
 * Returns the size of the underlying memory buffer.
 * @return The size of the underlying memory buffer.
 */
std::size_t TAsmBuffer::GetSize() const noexcept
{
    return this->buffer_size_;
}

/**
 * Returns a pointer to the underlying memory.
 * @return A pointer to the underlying memory.
 */
unsigned char* TAsmBuffer::GetBuffer() const noexcept
{
    return this->buffer_.get();
}

/**
 * Sets the length of the machine code data (in bytes) actually stored in the buffer.
 * ! This does not affect the size or content of the underlying memory !
 * @param code_length The length of the machine code (in bytes).
 */
void TAsmBuffer::SetCodeLength(std::size_t code_length) noexcept
{
    this->code_length_ = code_length;
}

/**
 * Returns the length of the machine code data (in bytes) actually stored in the buffer.
 * @return The length of the machine code data (in bytes) actually stored in the buffer.
 */
std::size_t TAsmBuffer::GetCodeLength() const noexcept
{
    return this->code_length_;
}

/**
 * Sets the absolute base address of the machine code in the buffer.
 * @param base_address The absolute base address of the machine code in the buffer.
 */
void TAsmBuffer::SetBaseAddress(int64_t base_address) noexcept
{
    this->base_address_ = base_address;
}

/**
 * Returns the absolute base address of the machine code in the buffer (the address of the first byte in the buffer).
 * @return The absolute base address of the machine code in the buffer.
 */
int64_t TAsmBuffer::GetBaseAddress() const noexcept
{
    return this->base_address_;
}

/**
 * Returns the current absolute address of the instruction pointer in the buffer.
 * This is the value of "base address" + "instruction pointer".
 * @return The current absolute address of the instruction pointer in the buffer.
 */
int64_t TAsmBuffer::GetCurrentAddress() const noexcept
{
    return this->base_address_ + this->instruction_pointer_;
}

/**
 * Loads machine code from the specified file. 
 * CodeLength() can be used to get the number of machine code bytes read from the file.
 * This base address is stored for later use.
 * @param file The file to load the machine code from.
 * @param base_address The base address (file offset) to load the machine code from.
 */
void TAsmBuffer::LoadFromFile(TFile* file, int64_t base_address) noexcept
{
    // Store the base address
    this->SetBaseAddress(base_address);

    // Try to load the machine code from the file
    this->SetCodeLength(file->ReadAt(this->buffer_.get(), this->buffer_size_, base_address));
}

/**
 * Resets the instruction pointer.
 * (Sets it to zero)
 */
void TAsmBuffer::ResetInstructionPointer() noexcept
{
    this->instruction_pointer_ = 0;
}

/**
 * Adds the specified offset to the instruction pointer.
 * @param offset The offset to add.
 */
void TAsmBuffer::AdvanceInstructionPointer(std::size_t offset) noexcept
{
    // Advance instruction pointer
    this->instruction_pointer_ += offset;
}

/**
 * Returns the instruction pointer (the offset to the start of the buffer)
 * @return The instruction pointer.
 */
std::size_t TAsmBuffer::GetInstructionPointer() const noexcept
{
    return this->instruction_pointer_;
}

/**
 * Returns the byte at the current position (of the instruction pointer) + the specified offset.
 * The instruction pointer is NOT moved!
 * @param offset The offset that is added to the instruction pointer.
 * @return The byte at the current position (of the instruction pointer).
 */
unsigned char TAsmBuffer::GetByte(std::size_t offset) const noexcept
{
    // If the instruction pointer is beyond the end of the code, return a null byte.
    if ((this->instruction_pointer_ + offset) >= this->code_length_) return 0u;

    // Return the byte
    return this->buffer_[this->instruction_pointer_ + offset];
}

/**
 * Returns the byte at the current position (of the instruction pointer).
 * The instruction pointer IS moved afterwards!
 * @return The byte at the current position (of the instruction pointer).
 */
unsigned char TAsmBuffer::ReadByte() noexcept
{
    // If the instruction pointer is beyond the end of the code, return a null byte.
    if (this->instruction_pointer_ >= this->code_length_) return 0u;

    // Return the byte and advance instruction pointer
    return this->buffer_[this->instruction_pointer_++];
}

/**
 * Extracts a value from the machine code into a 64bit integer variable.
 * The instruction pointer IS moved afterwards!
 * @param value_size The size of the value (number of bytes) to read.
 * @return The value (as 64bit signed integer).
 */
int64_t TAsmBuffer::ReadValue(TValueSize value_size) noexcept
{
    // Create a conversion buffer with defined alignment for the numeric casts
    alignas(1) unsigned char value_buffer[16];

    // Clear conversion buffer
    memset(value_buffer, 0, sizeof(value_buffer));

    // Copy the specified number of bytes from the source buffer into the target buffer
    for (std::size_t i = 0; i < EV(value_size); i++)
    {
        value_buffer[i] = this->ReadByte();
    }

    // Convert the value, depending on the size
    switch (value_size)
    {
    case TValueSize::BYTE:
        return *(reinterpret_cast<int8_t*>(value_buffer));
    case TValueSize::WORD:
        return *(reinterpret_cast<int16_t*>(value_buffer));
    case TValueSize::DWORD:
        return *(reinterpret_cast<int32_t*>(value_buffer));
    case TValueSize::QWORD:
        return *(reinterpret_cast<int64_t*>(value_buffer));
    }

    // This should never happen
    return 0;
}
