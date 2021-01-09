// Copyright (c) 2021 Roxxorfreak

#include "headers_test.hpp"

TEST(TAsmInstruction, GetSetAddress)
{
    TAsmInstruction inst;

    // Ensure output = input
    inst.SetAddress(42);
    ASSERT_EQ(42, inst.GetAddress());

    // Ensure clearing
    inst.Clear();
    ASSERT_EQ(0, inst.GetAddress());
}

TEST(TAsmInstruction, GetSetSegmentOverride)
{
    TAsmInstruction inst;

    // Ensure output = input
    inst.SetSegmentOverride(TSegmentOverride::CS);
    ASSERT_EQ(TSegmentOverride::CS, inst.GetSegmentOverride());

    // Ensure clearing
    inst.Clear();
    ASSERT_EQ(TSegmentOverride::NONE, inst.GetSegmentOverride());
}

TEST(TAsmInstruction, GetSegmentOverrideName)
{
    TAsmInstruction inst;

    // Should be empty
    ASSERT_STREQ("", inst.GetSegmentOverrideName().ToString());

    // Test segments
    inst.SetSegmentOverride(TSegmentOverride::CS);
    ASSERT_STREQ("cs:", inst.GetSegmentOverrideName().ToString());
    inst.SetSegmentOverride(TSegmentOverride::DS);
    ASSERT_STREQ("ds:", inst.GetSegmentOverrideName().ToString());
    inst.SetSegmentOverride(TSegmentOverride::ES);
    ASSERT_STREQ("es:", inst.GetSegmentOverrideName().ToString());
    inst.SetSegmentOverride(TSegmentOverride::SS);
    ASSERT_STREQ("ss:", inst.GetSegmentOverrideName().ToString());
    inst.SetSegmentOverride(TSegmentOverride::FS);
    ASSERT_STREQ("fs:", inst.GetSegmentOverrideName().ToString());
    inst.SetSegmentOverride(TSegmentOverride::GS);
    ASSERT_STREQ("gs:", inst.GetSegmentOverrideName().ToString());
}

TEST(TAsmInstruction, GetSetOpcode)
{
    TAsmBuffer buffer(2);
    TAsmInstruction inst;
    TAsmOpcodeManager opcodes;

    // Init the buffer for opcode "nop"
    *buffer.GetBuffer() = 0x90;
    buffer.SetCodeLength(1);

    // Find opcode
    auto oc = opcodes.FindOpcode(&buffer);
    ASSERT_NE(nullptr, oc);
    ASSERT_EQ(nullptr, inst.GetOpcode());

    // Set the opcde
    inst.SetOpcode(oc);

    // Validate opcode data
    ASSERT_EQ(PT_NONE, inst.param_[0].type);
    ASSERT_EQ(DONTCARE, inst.param_[0].size);
    ASSERT_EQ(HE_OPCODE_PREFIX_NONE, inst.opcode_prefix_);

    // Validate opcode & data
    ASSERT_NE(nullptr, inst.GetOpcode());
    ASSERT_STREQ("nop", inst.GetOpcode()->asm16.ToString());
    ASSERT_STREQ("nop", inst.GetOpcode()->asm32.ToString());
}

TEST(TAsmInstruction, GetSetMachineCode)
{
    TAsmBuffer buffer(3);
    TAsmInstruction inst;
    TAsmOpcodeManager opcodes;

    // Init the buffer for command "add eax, dword ptr [eax]"
    *buffer.GetBuffer() = 0x03;
    buffer.SetCodeLength(2);

    // Find opcode
    auto oc = opcodes.FindOpcode(&buffer);
    ASSERT_NE(nullptr, oc);

    // Set the opcde
    inst.SetOpcode(oc);

    // Set the machine code (the second byte is not decoded)
    inst.SetMachineCode(&buffer);

    // Length should be 1
    ASSERT_EQ(1u, inst.GetLength());

    // Compare first code byte
    ASSERT_EQ(3u, *inst.GetMachineCode());
}

TEST(TAsmInstruction, DecodeModRMByte)
{
    TAsmInstruction inst;

    // Decode 0xff
    inst.DecodeModRMByte(0xff);

    // Validate
    ASSERT_EQ(3u, inst.mod_rm_.mod);
    ASSERT_EQ(7u, inst.mod_rm_.reg);
    ASSERT_EQ(7u, inst.mod_rm_.rm);

    // Decode 0xaa
    inst.DecodeModRMByte(0xaa);

    // Validate
    ASSERT_EQ(2u, inst.mod_rm_.mod);
    ASSERT_EQ(5u, inst.mod_rm_.reg);
    ASSERT_EQ(2u, inst.mod_rm_.rm);

    // Only mod set
    inst.DecodeModRMByte(0xc0);

    // Validate
    ASSERT_EQ(3u, inst.mod_rm_.mod);
    ASSERT_EQ(0u, inst.mod_rm_.reg);
    ASSERT_EQ(0u, inst.mod_rm_.rm);

    // Only reg set
    inst.DecodeModRMByte(0x38);

    // Validate
    ASSERT_EQ(0u, inst.mod_rm_.mod);
    ASSERT_EQ(7u, inst.mod_rm_.reg);
    ASSERT_EQ(0u, inst.mod_rm_.rm);

    // Only rm set
    inst.DecodeModRMByte(0x07);

    // Validate
    ASSERT_EQ(0u, inst.mod_rm_.mod);
    ASSERT_EQ(0u, inst.mod_rm_.reg);
    ASSERT_EQ(7u, inst.mod_rm_.rm);
}
