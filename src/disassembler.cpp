// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Disassembles a single instruction.
 * @param buffer The buffer with the machine code bytes.
 * @param ai A pointer to an object that receives for disassembled instruction data (see TAsmInstruction).
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 */
void TDisassembler::DisassembleInstruction(TAsmBuffer* buffer, TAsmInstruction* ai, TArchitecture architecture)
{
    // Prepare instruction
    ai->Clear();
    ai->SetAddress(buffer->GetCurrentAddress());

    // Process all prefixes (and move the instruction pointer to the offset of the actual instruction)
    this->ProcessPrefixes(buffer, ai);

    // Get the opcode
    ai->SetOpcode(this->opcode_manager_.FindOpcode(buffer));

    // Check if the opcode could be identified
    if (ai->GetOpcode() == nullptr)
    {
        // Create a "db" mnemonic with the current byte aka. the unknown opcode
        ai->assembler_code_ = TString("db ") + this->formatter_.FormatValue(buffer->ReadValue(TValueSize::BYTE), TValueSize::BYTE);

        // Opcode could not be identified, store machine code
        ai->SetMachineCode(buffer);
        return;
    }

    // Process ModR/M byte (if present)
    // (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.5, Address-Mode Encoding of ModR/M and SIB Bytes)
    if (ai->GetOpcode()->has_mod_rm) ai->DecodeModRMByte(buffer->ReadByte());

    // Validate fields
    if ((ai->mod_rm_.reg > 7) || (ai->mod_rm_.rm > 7) || (ai->mod_rm_.mod > 3)) {

        // Set error
        ai->assembler_code_ = "<invalid ModR/M byte>";

        // Store machine code
        ai->SetMachineCode(buffer);
        return;
    }

    // Process all possible (up to three) parameters
    for (int32_t index = 0; index < 3; index++)
    {
        // Skip unused parameters
        if (ai->param_[index].type == PT_NONE) continue;

        // Register coded in reg field of ModR/M byte
        if (ai->param_[index].type == PT_REG)
        {
            ai->param_[index].value = this->GetRegisterName(ai, ai->mod_rm_.reg, ai->param_[index].size, architecture);
        }

        // Segment register coded in reg field of ModR/M byte
        if (ai->param_[index].type == PT_SREG)
        {
            ai->param_[index].value = reg_sreg[ai->mod_rm_.reg];
        }

        // Control register coded in reg field of ModR/M byte
        if (ai->param_[index].type == PT_CREG)
        {
            ai->param_[index].value = reg_control[ai->mod_rm_.reg];
        }

        // Debug register coded in reg field of ModR/M byte
        if (ai->param_[index].type == PT_DREG)
        {
            ai->param_[index].value = reg_debug[ai->mod_rm_.reg];
        }

        // Test register coded in reg field of ModR/M byte
        if (ai->param_[index].type == PT_TREG)
        {
            ai->param_[index].value = reg_test[ai->mod_rm_.reg];
        }

        // Register/Memory coded in mod r/m fields of ModR/M byte (may include SIB byte)
        if (ai->param_[index].type == PT_REG_MEM)
        {
            if (this->AddressSize32(architecture, ai->address_size_prefix_))
                ai->param_[index].value = this->DecodeModRmByte32(ai, index, buffer, architecture);
            else
                ai->param_[index].value = this->DecodeModRmByte16(ai, index, buffer, architecture);
        }

        // Immediate Value (no Mod R/M)
        if (ai->param_[index].type == PT_IMMVAL)
        {
            if (ai->param_[index].size == BIT8)
            {
                ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::BYTE), TValueSize::BYTE);
            }
            else if (ai->param_[index].size == BIT16)
            {
                ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD);
            }
            else if (ai->param_[index].size == BIT32)
            {
                ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD);
            }
            else if (ai->param_[index].size == BIT1632)
            {
                if (this->OperandSize32(architecture, ai->operand_size_prefix_))  // 32 bit
                {
                    ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD);
                }
                else
                {
                    ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD);
                }
            }
            else if (ai->param_[index].size == BIT64)
            {
                ai->param_[index].value = this->formatter_.FormatValue(buffer->ReadValue(TValueSize::QWORD), TValueSize::QWORD);
            }
        }

        // Relative address (no ModR/M)
        if (ai->param_[index].type == PT_REL_ADDRESS)
        {
            int64_t val = 0;

            if (this->OperandSize32(architecture, ai->operand_size_prefix_))  // 32 bit
            {
                if (ai->param_[index].size != BIT8)  // 8bit remain 8bit even in 32bit mode, only 16bit may be changed to 32bit
                {
                    if (ai->param_[index].size == BIT16)  // parameters with 16bit operands even in 32bit mode
                    {
                        val = buffer->ReadValue(TValueSize::WORD);
                    }
                    else  // parameters are changed from 16bit to 32bit (BIT32, BIT1632)
                    {
                        val = buffer->ReadValue(TValueSize::DWORD);
                    }
                }
                else
                {
                    val = buffer->ReadValue(TValueSize::BYTE);
                }
            }
            else  // 16bit processing
            {
                if (ai->param_[index].size != BIT8)
                {
                    val = buffer->ReadValue(TValueSize::WORD);
                }
                else
                {
                    val = buffer->ReadValue(TValueSize::BYTE);
                }
            }

            // Calculate relative address
            const int64_t address = buffer->GetCurrentAddress() + val;

            // Always display address in 64bit
            ai->param_[index].value = this->formatter_.FormatValue(address, TValueSize::QWORD);
        }

        // Memory offset (only for some mov instructions)
        // See Intel Instruction Set Reference, Vol. 2A, Chapter 3.1.1.3, moffs8, moffs16, moffs32.
        if (ai->param_[index].type == PT_MEMOFFS)
        {
            // Add segment override, if specified
            ai->param_[index].value = ai->GetSegmentOverrideName();

            // Add offset
            ai->param_[index].value += "[";
            if (this->AddressSize32(architecture, ai->address_size_prefix_))  // 32 bit
            {
                if (ai->param_[index].size == BIT16)  // parameters with 16bit operands even in 32bit mode
                {
                    ai->param_[index].value += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD);
                }
                else  // parameters are changed from 16bit to 32bit (BIT32, BIT1632)
                {
                    ai->param_[index].value += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD);
                }
            }
            else  // 16bit processing
            {
                ai->param_[index].value += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD);
            }
            ai->param_[index].value += "]";
        }  // EO Memory offset

        // Absolute address (only for some far jmp & far call instructions)
        // See Intel Instruction Set Reference, Vol. 2A, Chapter 3.1.1.3, m16:16, m16:32.
        if (ai->param_[index].type == PT_ABS_ADDRESS)
        {
            int64_t val = 0;
            int64_t val2 = 0;

            if (this->OperandSize32(architecture, ai->operand_size_prefix_))  // 32 bit
            {
                val = buffer->ReadValue(TValueSize::DWORD);
                val2 = buffer->ReadValue(TValueSize::WORD);
                ai->param_[index].value = this->formatter_.FormatValue(val2, TValueSize::WORD);
                ai->param_[index].value += ":";
                ai->param_[index].value += this->formatter_.FormatValue(val, TValueSize::DWORD);
            }
            else
            {
                val = buffer->ReadValue(TValueSize::WORD);
                val2 = buffer->ReadValue(TValueSize::WORD);
                ai->param_[index].value = this->formatter_.FormatValue(val2, TValueSize::WORD);
                ai->param_[index].value += ":";
                ai->param_[index].value += this->formatter_.FormatValue(val, TValueSize::WORD);
            }
        }  // EO Absolute address
    }  /* End of parameter processing */

    // Add opcode prefix to disassembled code
    // TODO(Roxxorfreak): Evaluate prefix depending on operation (branch hint, xacquire)
    if (ai->opcode_prefix_ & HE_OPCODE_PREFIX_REP)   ai->assembler_code_ += "rep ";
    if (ai->opcode_prefix_ & HE_OPCODE_PREFIX_REPNE) ai->assembler_code_ += "repne ";
    if (ai->opcode_prefix_ & HE_OPCODE_PREFIX_LOCK)  ai->assembler_code_ += "lock ";

    // Create the disassembled command as string
    char stuff[128] = {};
    if (this->OperandSize32(architecture, ai->operand_size_prefix_))
    {
        snprintf(stuff, sizeof(stuff), ai->GetOpcode()->asm32, ai->param_[0].value.ToString(), ai->param_[1].value.ToString(), ai->param_[2].value.ToString());
    }
    else
    {
        snprintf(stuff, sizeof(stuff), ai->GetOpcode()->asm16, ai->param_[0].value.ToString(), ai->param_[1].value.ToString(), ai->param_[2].value.ToString());
    }
    ai->assembler_code_ += stuff;

    // Instruction decoded successfully, store machine code
    ai->SetMachineCode(buffer);
}

/**
 * Process all prefixes (Group 1-4) in the buffer.
 * (see Intel Instruction Set Reference, Vol. 2A, Chapter 2.1.1: Instruction Prefixes)
 * @param buffer The buffer with the machine code bytes.
 * @param ai A pointer to an object that receives for disassembled instruction data (see TAsmInstruction).
 */
void TDisassembler::ProcessPrefixes(TAsmBuffer* buffer, TAsmInstruction* ai)
{
    // No prefixes yet
    bool prefix_found = false;

    // Process the machine code byte by byte.
    do
    {
        // No prefix byte found yet
        prefix_found = false;

        switch (buffer->GetByte())
        {
        // Check for Group 1 (lock, rep) prefixes
        case 0xF3:
            ai->opcode_prefix_ |= HE_OPCODE_PREFIX_REP;
            prefix_found = true;
            break;
        case 0xF2:
            ai->opcode_prefix_ |= HE_OPCODE_PREFIX_REPNE;
            prefix_found = true;
            break;
        case 0xF0:
            ai->opcode_prefix_ |= HE_OPCODE_PREFIX_LOCK;
            prefix_found = true;
            break;
        // Check for Group 2 (segment-override) prefixes
        case 0x2E:
            ai->SetSegmentOverride(TSegmentOverride::CS);
            prefix_found = true;
            break;
        case 0x36:
            ai->SetSegmentOverride(TSegmentOverride::SS);
            prefix_found = true;
            break;
        case 0x3E:
            ai->SetSegmentOverride(TSegmentOverride::DS);
            prefix_found = true;
            break;
        case 0x26:
            ai->SetSegmentOverride(TSegmentOverride::ES);
            prefix_found = true;
            break;
        case 0x64:
            ai->SetSegmentOverride(TSegmentOverride::FS);
            prefix_found = true;
            break;
        case 0x65:
            ai->SetSegmentOverride(TSegmentOverride::GS);
            prefix_found = true;
            break;
        // Check for Group 3 (operand-size override) prefixes
        case 0x66:
            ai->operand_size_prefix_ = true;
            prefix_found = true;
            break;
        // Check for Group 4 (address-size override) prefixes
        case 0x67:
            ai->address_size_prefix_ = true;
            prefix_found = true;
            break;
        }

        // Check if a prefix was found
        if (prefix_found == true)
        {
            // A prefix was found, advance instruction pointer
            buffer->AdvanceInstructionPointer(1);
        }

    } while (prefix_found);
}

/**
 * Decodes a ModR/M byte (16bit architecture).
 * @param ai A pointer to the assembler instruction currently processing.
 * @param index The zero-based index of the parameter that is being processed.
 * @param buffer The machine code buffer to read additional data from.
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 * @return The decoded value of the ModR/M byte.
 */
TString TDisassembler::DecodeModRmByte16(TAsmInstruction* ai, int32_t index, TAsmBuffer* buffer, TArchitecture architecture)
{
    TString val;

    // MOD = 3: The parameter is a register
    if (ai->mod_rm_.mod == 3)
    {
        return this->GetRegisterName(ai, ai->mod_rm_.rm, ai->param_[index].size, architecture);
    }

    // Clear the data
    val.New(20);

    // Add operand (pointer) size
    val += this->GetOperandPointerName(ai, ai->param_[index].size, architecture);

    // Add segment override prefix, if present
    val += ai->GetSegmentOverrideName();

    if (ai->mod_rm_.mod == 0)
    {
        switch (ai->mod_rm_.rm)
        {
            case 0:
                val += "[bx+si]";
                break;
            case 1:
                val += "[bx+di]";
                break;
            case 2:
                val += "[bp+si]";
                break;
            case 3:
                val += "[bp+di]";
                break;
            case 4:
                val += "[si]";
                break;
            case 5:
                val += "[di]";
                break;
            case 6:
                val += "[";
                val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD);
                val += "]";
                break;
            case 7:
                val += "[bx]";
                break;
        }
    }
    else if (ai->mod_rm_.mod == 1)
    {
        if (ai->mod_rm_.rm == 0) val += "[bx+si";
        if (ai->mod_rm_.rm == 1) val += "[bx+di";
        if (ai->mod_rm_.rm == 2) val += "[bp+si";
        if (ai->mod_rm_.rm == 3) val += "[bp+di";
        if (ai->mod_rm_.rm == 4) val += "[si";
        if (ai->mod_rm_.rm == 5) val += "[di";
        if (ai->mod_rm_.rm == 6) val += "[bp";
        if (ai->mod_rm_.rm == 7) val += "[bx";

        val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::BYTE), TValueSize::BYTE, true);
        val += "]";
    }
    else if (ai->mod_rm_.mod == 2)
    {
        if (ai->mod_rm_.rm == 0) val += "[bx+si";
        if (ai->mod_rm_.rm == 1) val += "[bx+di";
        if (ai->mod_rm_.rm == 2) val += "[bp+si";
        if (ai->mod_rm_.rm == 3) val += "[bp+di";
        if (ai->mod_rm_.rm == 4) val += "[si";
        if (ai->mod_rm_.rm == 5) val += "[di";
        if (ai->mod_rm_.rm == 6) val += "[bp";
        if (ai->mod_rm_.rm == 7) val += "[bx";

        val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::WORD), TValueSize::WORD, true);
        val += "]";
    }

    // Return the decoded data
    return val;
}

/**
 * Decodes a ModR/M byte (32bit architecture).
 * @param ai A pointer to the assembler instruction currently processing.
 * @param index The zero-based index of the parameter that is being processed.
 * @param buffer The machine code buffer to read additional data from.
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 * @return The decoded value of the ModR/M byte.
 */
TString TDisassembler::DecodeModRmByte32(TAsmInstruction* ai, int32_t index, TAsmBuffer* buffer, TArchitecture architecture)
{
    TString val;

    // MOD = 3: The parameter is a register
    if (ai->mod_rm_.mod == 3)
    {
        return this->GetRegisterName(ai, ai->mod_rm_.rm, ai->param_[index].size, architecture);
    }

    // Assume no additional data is present
    bool imm8 = false;
    bool imm32 = false;
    bool sib_present = false;

    // Clear the data
    val.New(20);

    // Add operand (pointer) size
    val += this->GetOperandPointerName(ai, ai->param_[index].size, architecture);

    // Add segment override prefix, if present
    val += ai->GetSegmentOverrideName();

    // Evaluate "mod" value
    if (ai->mod_rm_.mod == 0)
    {
        switch (ai->mod_rm_.rm)
        {
            case 0:
                val += "[eax]";
                break;
            case 1:
                val += "[ecx]";
                break;
            case 2:
                val += "[edx]";
                break;
            case 3:
                val += "[ebx]";
                break;
            case 4:
                sib_present = true;
                break;
            case 5:
                val += "[";
                val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD);
                val += "]";
                break;
            case 6:
                val += "[esi]";
                break;
            case 7:
                val += "[edi]";
                break;
        }
    }
    else if (ai->mod_rm_.mod == 1)
    {
        switch (ai->mod_rm_.rm)
        {
            case 0:
                val += "[eax";
                break;
            case 1:
                val += "[ecx";
                break;
            case 2:
                val += "[edx";
                break;
            case 3:
                val += "[ebx";
                break;
            case 4:
                sib_present = true;
                imm8 = true;
                break;
            case 5:
                val += "[ebp";
                break;
            case 6:
                val += "[esi";
                break;
            case 7:
                val += "[edi";
                break;
        }
        if (!sib_present)  // Add 8bit displacement if SIB byte is not present, otherwise remember 8bit displacement and continue to SIP processing
        {
            val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::BYTE), TValueSize::BYTE, true);
            val += "]";
        }
    }
    else if (ai->mod_rm_.mod == 2)
    {
        switch (ai->mod_rm_.rm)
        {
            case 0:
                val += "[eax";
                break;
            case 1:
                val += "[ecx";
                break;
            case 2:
                val += "[edx";
                break;
            case 3:
                val += "[ebx";
                break;
            case 4:
                sib_present = true;
                imm32 = true;
                break;
            case 5:
                val += "[ebp";
                break;
            case 6:
                val += "[esi";
                break;
            case 7:
                val += "[edi";
                break;
        }
        if (!sib_present)  // Add 32bit displacement if SIB byte is not present, otherwise remember 32bit displacement and continue to SIP processing
        {
            val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD, true);
            val += "]";
        }
    }

    if (sib_present)  // SIB (Scale Index Base) byte present
    {
        // Decode SIB byte
        const auto ss = static_cast<int16_t>(buffer->GetByte() >> 6);
        const auto ind = static_cast<int16_t>((buffer->GetByte() & 0x38) >> 3);
        const auto base = static_cast<int16_t>(buffer->GetByte() & 0x07);

        // Advance instruction pointer
        buffer->AdvanceInstructionPointer(1);

        val += "[";
        switch (base)
        {
        case 0:
            val += "eax";
            break;
        case 1:
            val += "ecx";
            break;
        case 2:
            val += "edx";
            break;
        case 3:
            val += "ebx";
            break;
        case 4:
            val += "esp";
            break;
        case 5:
            if (ai->mod_rm_.mod == 0)
            {
                imm32 = true;
            }
            else
            {
                // Append "ebp" if base = 5 and mod != 0
                // See: Intel Instruction Set Reference, Table 2-3. (32-Bit Addressing Forms with the SIB byte)
                val += "ebp";
            }
            break;
        case 6:
            val += "esi";
            break;
        case 7:
            val += "edi";
            break;
        }

        if ((ss == 0) && (ind == 0)) val += "+eax*1";
        if ((ss == 0) && (ind == 1)) val += "+ecx*1";
        if ((ss == 0) && (ind == 2)) val += "+edx*1";
        if ((ss == 0) && (ind == 3)) val += "+ebx*1";
        if ((ss == 0) && (ind == 4)) val += "";
        if ((ss == 0) && (ind == 5)) val += "+ebp*1";
        if ((ss == 0) && (ind == 6)) val += "+esi*1";
        if ((ss == 0) && (ind == 7)) val += "+edi*1";
        if ((ss == 1) && (ind == 0)) val += "+eax*2";
        if ((ss == 1) && (ind == 1)) val += "+ecx*2";
        if ((ss == 1) && (ind == 2)) val += "+edx*2";
        if ((ss == 1) && (ind == 3)) val += "+ebx*2";
        if ((ss == 1) && (ind == 4)) val += "";
        if ((ss == 1) && (ind == 5)) val += "+ebp*2";
        if ((ss == 1) && (ind == 6)) val += "+esi*2";
        if ((ss == 1) && (ind == 7)) val += "+edi*2";
        if ((ss == 2) && (ind == 0)) val += "+eax*4";
        if ((ss == 2) && (ind == 1)) val += "+ecx*4";
        if ((ss == 2) && (ind == 2)) val += "+edx*4";
        if ((ss == 2) && (ind == 3)) val += "+ebx*4";
        if ((ss == 2) && (ind == 4)) val += "";
        if ((ss == 2) && (ind == 5)) val += "+ebp*4";
        if ((ss == 2) && (ind == 6)) val += "+esi*4";
        if ((ss == 2) && (ind == 7)) val += "+edi*4";
        if ((ss == 3) && (ind == 0)) val += "+eax*8";
        if ((ss == 3) && (ind == 1)) val += "+ecx*8";
        if ((ss == 3) && (ind == 2)) val += "+edx*8";
        if ((ss == 3) && (ind == 3)) val += "+ebx*8";
        if ((ss == 3) && (ind == 4)) val += "";
        if ((ss == 3) && (ind == 5)) val += "+ebp*8";
        if ((ss == 3) && (ind == 6)) val += "+esi*8";
        if ((ss == 3) && (ind == 7)) val += "+edi*8";

        if (imm8)
        {
            val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::BYTE), TValueSize::BYTE, true);
        }
        if (imm32)
        {
            val += this->formatter_.FormatValue(buffer->ReadValue(TValueSize::DWORD), TValueSize::DWORD, true);
        }

        val += "]";
    }

    // Return the decoded data
    return val;
}

/**
 * Evaluates a register name from a register code (3bit value, 0-7).
 * @param ai A pointer to the assembler instruction currently processing.
 * @param register_code The register code (0-7).
 * @param size The size of the register (bit code, see Operand sizes)
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 * @return The string with the register name.
 */
TString TDisassembler::GetRegisterName(TAsmInstruction* ai, uint16_t register_code, uint16_t size, TArchitecture architecture)
{
    // Fixed size
    if (size == BIT8) return TString(reg_reg8[register_code]);
    if (size == BIT16) return TString(reg_reg16[register_code]);
    if (size == BIT32) return TString(reg_reg32[register_code]);
    if (size == BIT64) return TString(reg_mm[register_code]);
    if (size == BIT128) return TString(reg_xmm[register_code]);

    // Variable size
    if (size == BIT1632)
    {
        if (this->OperandSize32(architecture, ai->operand_size_prefix_))
        {
            return TString(reg_reg32[register_code]);
        }
        else
        {
            return TString(reg_reg16[register_code]);
        }
    }
    if (size == BIT32128)
    {
        // Evaluate operand size prefix
        if (ai->operand_size_prefix_ == true)
        {
            return TString(reg_xmm[register_code]);
        }
        else
        {
            return TString(reg_mm[register_code]);
        }
    }
    if (size == BIT64128)
    {
        // Evaluate operand size prefix
        if (ai->operand_size_prefix_ == true)
        {
            return TString(reg_xmm[register_code]);
        }
        else
        {
            return TString(reg_mm[register_code]);
        }
    }

    // Register code could not be evaluated
    return TString("?");
}

/**
 * Creates the operand pointer name from the operand size.
 * @param ai A pointer to the assembler instruction currently processing.
 * @param size The size of the operand (bit code, see Operand sizes)
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 * @return The string with the operand pointer name.
 */
TString TDisassembler::GetOperandPointerName(TAsmInstruction* ai, uint16_t size, TArchitecture architecture)
{
    if (size == BIT8) return TString("byte ptr ");
    if (size == BIT16) return TString("word ptr ");
    if (size == BIT32) return TString("dword ptr ");
    if (size == BIT64) return TString("qword ptr ");
    if (size == BIT128) return TString("xmmword ptr ");
    if (size == BIT1632)
    {
        if (this->OperandSize32(architecture, ai->operand_size_prefix_))
        {
            return TString("dword ptr ");
        }
        else
        {
            return TString("word ptr ");
        }
    }
    if (size == BIT32128)
    {
        if (ai->operand_size_prefix_ == true)
        {
            return TString("xmmword ptr ");
        }
        else
        {
            return TString("dword ptr ");
        }
    }
    if (size == BIT64128)
    {
        if (ai->operand_size_prefix_ == true)
        {
            return TString("xmmword ptr ");
        }
        else
        {
            return TString("qword ptr ");
        }
    }

    // No pointer name
    return TString("");
}

/**
 * Checks if the current default operand size is 32 bit.
 * @param bit_mode The processor architecture (bit mode) (see TArchitecture).
 * @param operand_size_prefix true if the operand has a size prefix, false otherwise.
 * @return true if the current default operand size is 32 bit, false otherwise.
 */
bool TDisassembler::OperandSize32(TArchitecture bit_mode, bool operand_size_prefix) noexcept
{
    if ((bit_mode == TArchitecture::x86_32) && (operand_size_prefix)) return false;
    if ((bit_mode == TArchitecture::x86_16) && (!operand_size_prefix)) return false;
    return true;
}

/**
 * Checks if the current default address size is 32 bit.
 * @param bit_mode The processor architecture (bit mode) (see TArchitecture).
 * @param address_size_prefix true if the address has a size prefix, false otherwise.
 * @return true if the current default address size is 32 bit, false otherwise.
 */
bool TDisassembler::AddressSize32(TArchitecture bit_mode, bool address_size_prefix) noexcept
{
    if ((bit_mode == TArchitecture::x86_32) && (address_size_prefix)) return false;
    if ((bit_mode == TArchitecture::x86_16) && (!address_size_prefix)) return false;
    return true;
}

/**
 * Disassembles the specified buffer into an array of assembler instructions.
 * @param buffer The buffer that contains the machine code to disassemble.
 * @param instruction A pointer to an array that receives the disassembled instructions.
 * @param instruction_size The number of elements in the buffer for the disassembled instructions (thus the maximum number of instructions to disassemble).
 * @param architecture The processor architecture to use for disassembling (see TArchitecture).
 * @param number_format The number format for the output of numbers (see TNumberFormat).
 * @return The number of instructions that were disassembled.
 */
int32_t TDisassembler::Disassemble(TAsmBuffer* buffer, TAsmInstruction* instruction, int32_t instruction_size, TArchitecture architecture, TNumberFormat number_format)
{
    int32_t disassembled_instructions = 0;

    // Set the parameters for the value processor
    this->formatter_.SetNumberFormat(number_format);

    // Start over
    buffer->ResetInstructionPointer();

    // Process as many instructions as specified
    for (int32_t i = 0; i < instruction_size; i++)
    {
        // Ensure the instruction pointer is within the specified code buffer
        if (buffer->GetInstructionPointer() >= buffer->GetCodeLength()) break;

        // Disassemble next instruction
        this->DisassembleInstruction(buffer, &instruction[i], architecture);

        // Count the disassembled instruction
        disassembled_instructions++;
    }

    // Return the number of instructions that were disassembled
    return disassembled_instructions;
}
