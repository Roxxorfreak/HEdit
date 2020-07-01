// Copyright (c) 2020 Roxxorfreak

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
