// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Clears the contents of the assembler instruction.
 */
void TAsmInstruction::Clear()
{
    // Clear all data fields
    this->length_ = 0;
    this->opcode_ = nullptr;
    this->address_ = 0;
    this->opcode_prefix_ = HE_OPCODE_PREFIX_NONE;
    this->assembler_code_ = "";
    this->segment_override_ = TSegmentOverride::NONE;
    this->address_size_prefix_ = false;
    this->operand_size_prefix_ = false;
    this->mod_rm_.mod = 0;
    this->mod_rm_.reg = 0;
    this->mod_rm_.rm = 0;
    this->param_[0].type = PT_NONE;
    this->param_[0].size = DONTCARE;
    this->param_[1].type = PT_NONE;
    this->param_[1].size = DONTCARE;
    this->param_[2].type = PT_NONE;
    this->param_[2].size = DONTCARE;
    memset(this->machine_code_, 0, HE_MAX_INSTRUCTION_LENGTH);
}

/**
 * Sets the absolute address of the instruction.
 * @param address The absolute address of the instruction.
 */
void TAsmInstruction::SetAddress(int64_t address)
{
    this->address_ = address;
}

/**
 * Returns the absolute address of the instruction.
 * @return The absolute address of the instruction.
 */
int64_t TAsmInstruction::GetAddress()
{
    return this->address_;
}

/**
 * Sets the segement override code of the instruction.
 * (see TSegmentOverride)
 * @param segment_override The segement override code of the instruction.
 */
void TAsmInstruction::SetSegmentOverride(TSegmentOverride segment_override)
{
    this->segment_override_ = segment_override;
}

/**
 * Returns the segement override code of the instruction.
 * (see TSegmentOverride)
 * @return The segement override code of the instruction.
 */
TSegmentOverride TAsmInstruction::GetSegmentOverride()
{
    return this->segment_override_;
}

/**
 * Returns the name of the segment override for the instruction.
 * (if one was specified via prefix, see TSegmentOverride)
 * @return The name of the segment override, followed by a colon.
 */
TString TAsmInstruction::GetSegmentOverrideName()
{
    if (this->segment_override_ == TSegmentOverride::CS) return TString("cs:");
    if (this->segment_override_ == TSegmentOverride::SS) return TString("ss:");
    if (this->segment_override_ == TSegmentOverride::DS) return TString("ds:");
    if (this->segment_override_ == TSegmentOverride::ES) return TString("es:");
    if (this->segment_override_ == TSegmentOverride::FS) return TString("fs:");
    if (this->segment_override_ == TSegmentOverride::GS) return TString("gs:");
    return TString("");
}

/**
 * Sets the opcode of the instruction.
 * @param opcode A pointer to the TOpcode that is represented by the instruction.
 */
void TAsmInstruction::SetOpcode(const TOpcode* opcode)
{
    // Store opcode pointer
    this->opcode_ = opcode;

    // Validate opcode pointer
    if (opcode == nullptr) return;

    // Opcode specified, copy parameter data
    this->param_[0].type = opcode->param_type[0];
    this->param_[0].size = static_cast<uint16_t>(opcode->param_size[0]);
    this->param_[1].type = opcode->param_type[1];
    this->param_[1].size = static_cast<uint16_t>(opcode->param_size[1]);
    this->param_[2].type = opcode->param_type[2];
    this->param_[2].size = static_cast<uint16_t>(opcode->param_size[2]);
}

/**
 * Returns the opcode of the instruction.
 * @return A pointer to the TOpcode that is represented by the instruction (or nullptr, if none).
 */
const TOpcode* TAsmInstruction::GetOpcode()
{
    return this->opcode_;
}

/**
 * Sets the machine code of the instruction.
 * The instruction pointer of the specified buffer 
 * is expected to point at the end of the instruction. 
 * @param buffer The machine code buffer to copy the machine code bytes from.
 */
void TAsmInstruction::SetMachineCode(TAsmBuffer* buffer)
{
    // Calculate the length of the machine code for the current instruction
    auto length = static_cast<std::size_t>(buffer->GetCurrentAddress() - this->address_);

    // Calculate start offset
    auto start_offset = buffer->GetInstructionPointer() - length;

    // Validate length
    if (length > HE_MAX_INSTRUCTION_LENGTH) length = HE_MAX_INSTRUCTION_LENGTH;

    // Validate start offset
    if (start_offset > buffer->GetSize()) return;

    // Copy data
    memcpy(this->machine_code_, &buffer->GetBuffer()[start_offset], length);

    // Store length
    this->length_ = length;
}

/**
 * Returns a pointer to the buffer containing the machine code.
 * @return A pointer to the buffer containing the machine code.
 */
const unsigned char* TAsmInstruction::GetMachineCode()
{
    return this->machine_code_;
}

/**
 * Returns the length of the machine code of the instruction (in bytes).
 * @return The length of the machine code of the instruction (in bytes).
 */
std::size_t TAsmInstruction::GetLength()
{
    return this->length_;
}

/**
 * Decodes the ModR/M byte.
 * The byte is decoded and split in the respective values.
 * @param code_byte The byte to add.
 */
void TAsmInstruction::DecodeModRMByte(unsigned char code_byte)
{
    // Decode the ModR/M
    this->mod_rm_.mod = static_cast<uint16_t>(code_byte >> 6);
    this->mod_rm_.reg = static_cast<uint16_t>((code_byte & 0x38) >> 3);
    this->mod_rm_.rm = static_cast<uint16_t>(code_byte & 0x07);
}
