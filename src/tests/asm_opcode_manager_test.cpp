// Copyright (c) 2021 Roxxorfreak

#include "headers_test.hpp"

TEST(TAsmOpcodeManager, FindOpcode)
{
    unsigned char* cp;
    TOpcode* oc = nullptr;
    TAsmBuffer buffer(5);
    TAsmOpcodeManager opcodes;

    // Get buffer
    cp = buffer.GetBuffer();

    // 1 byte opcode (iretw)
    *cp = 0xCF;
    buffer.SetCodeLength(1);
    buffer.ResetInstructionPointer();
    ASSERT_NE(nullptr, oc = opcodes.FindOpcode(&buffer));
    ASSERT_STREQ("iretw", oc->asm16.ToString());
    ASSERT_STREQ("iretd", oc->asm32.ToString());

    // 2 byte opcode without extension (wbinvd)
    *cp = 0x0F;
    *(cp + 1) = 0x09;
    *(cp + 2) = 0x01;
    buffer.SetCodeLength(3);
    buffer.ResetInstructionPointer();
    ASSERT_NE(nullptr, oc = opcodes.FindOpcode(&buffer));
    ASSERT_STREQ("wbinvd", oc->asm16.ToString());
    ASSERT_STREQ("wbinvd", oc->asm32.ToString());

    // 2 byte opcode with extension (lldt)
    *cp = 0x0F;
    *(cp + 1) = 0x00;
    *(cp + 2) = 0x10;
    buffer.SetCodeLength(3);
    buffer.ResetInstructionPointer();
    ASSERT_NE(nullptr, oc = opcodes.FindOpcode(&buffer));
    ASSERT_STREQ("lldt %s", oc->asm16.ToString());
    ASSERT_STREQ("lldt %s", oc->asm32.ToString());

    // 2 byte opcode with extension (ltr)
    *cp = 0x0F;
    *(cp + 1) = 0x00;
    *(cp + 2) = 0x18;
    buffer.SetCodeLength(3);
    buffer.ResetInstructionPointer();
    ASSERT_NE(nullptr, oc = opcodes.FindOpcode(&buffer));
    ASSERT_STREQ("ltr %s", oc->asm16.ToString());
    ASSERT_STREQ("ltr %s", oc->asm32.ToString());

    // 3 byte opcode (monitor)
    *cp = 0x0F;
    *(cp + 1) = 0x01;
    *(cp + 2) = 0xC8;
    buffer.SetCodeLength(3);
    buffer.ResetInstructionPointer();
    ASSERT_NE(nullptr, oc = opcodes.FindOpcode(&buffer));
    ASSERT_STREQ("monitor", oc->asm16.ToString());
    ASSERT_STREQ("monitor", oc->asm32.ToString());
}
