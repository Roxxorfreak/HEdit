// Copyright (c) 2021 Roxxorfreak

#include "headers_test.hpp"

TEST(TAsmBuffer, GetSize)
{
    TAsmBuffer buffer(2);

    ASSERT_EQ(2u, buffer.GetSize());
}

TEST(TAsmBuffer, GetSetBaseAddress)
{
    TAsmBuffer buffer(10);

    // Set base address
    buffer.SetBaseAddress(1234567);

    // Base address must match
    ASSERT_EQ(1234567, buffer.GetBaseAddress());

    // Current address must also match
    ASSERT_EQ(1234567, buffer.GetCurrentAddress());
}

TEST(TAsmBuffer, GetSetInstructionPointer)
{
    TAsmBuffer buffer(10);
    buffer.SetBaseAddress(5);

    // Instruction pointer must be 0
    ASSERT_EQ(0u, buffer.GetInstructionPointer());

    // Advance IP
    buffer.AdvanceInstructionPointer(7);

    // Instruction pointer must be 7
    ASSERT_EQ(7u, buffer.GetInstructionPointer());

    // Current address must also match
    ASSERT_EQ(12, buffer.GetCurrentAddress());
}

TEST(TAsmBuffer, GetByte)
{
    TAsmBuffer buffer(10);

    // Set some code
    strcpy_s(reinterpret_cast<char*>(buffer.GetBuffer()), 10, "abcdefghi");

    // Validate second byte (must be 0, because no code length set)
    ASSERT_EQ(0, buffer.GetByte(1));

    // Set code length
    buffer.SetCodeLength(10);

    // Validate second byte
    ASSERT_EQ('b', buffer.GetByte(1));

    // Validate terminating zero
    ASSERT_EQ(0, buffer.GetByte(9));

    // Validate out of bounds
    ASSERT_EQ(0, buffer.GetByte(10));
    ASSERT_EQ(0, buffer.GetByte(123456));

    // Advance 8 bytes
    buffer.AdvanceInstructionPointer(8);

    // Validate "current" byte
    ASSERT_EQ('i', buffer.GetByte(0));

    // Validate out of bounds
    ASSERT_EQ(0, buffer.GetByte(1));

    // Advance 1 byte
    buffer.AdvanceInstructionPointer(1);

    // Instruction pointer must be 10
    ASSERT_EQ(9u, buffer.GetInstructionPointer());

    // Validate out of bounds
    ASSERT_EQ(0, buffer.GetByte(0));
}

TEST(TAsmBuffer, ReadByte)
{
    TAsmBuffer buffer(10);

    // Set some code
    strcpy_s(reinterpret_cast<char*>(buffer.GetBuffer()), 10, "abcdefghi");

    // Validate next byte (must be 0, because no code length set)
    ASSERT_EQ(0, buffer.ReadByte());

    // Set code length
    buffer.SetCodeLength(10);

    // Validate second byte
    ASSERT_EQ('a', buffer.ReadByte());
    ASSERT_EQ('b', buffer.ReadByte());

    // Advance 8 bytes
    buffer.AdvanceInstructionPointer(6);

    // Validate "current" byte
    ASSERT_EQ('i', buffer.ReadByte());

    // Validate out of bounds
    ASSERT_EQ(0, buffer.ReadByte());

    // Instruction pointer must be 10
    ASSERT_EQ(10u, buffer.GetInstructionPointer());
}

TEST(TAsmBuffer, ReadValue)
{
    int64_t value = 1311768467284833366;
    TAsmBuffer buffer(10);

    // Copy the number bytes to the string
    strncpy_s(reinterpret_cast<char*>(buffer.GetBuffer()), 10, reinterpret_cast<char*>(&value), 8);

    // Set code length
    buffer.SetCodeLength(8);

    // Read byte
    ASSERT_EQ(0x56, buffer.ReadValue(TValueSize::BYTE));

    // Read word
    ASSERT_EQ(0x1234, buffer.ReadValue(TValueSize::WORD));

    // Read dword
    ASSERT_EQ(0x34567890, buffer.ReadValue(TValueSize::DWORD));

    // Read qword
    ASSERT_EQ(0x0000000000000012, buffer.ReadValue(TValueSize::QWORD));

    // Read word
    ASSERT_EQ(0, buffer.ReadValue(TValueSize::WORD));

    // Start over
    buffer.ResetInstructionPointer();

    // Read qword
    ASSERT_EQ(0x1234567890123456, buffer.ReadValue(TValueSize::QWORD));
}
