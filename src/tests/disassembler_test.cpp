// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TDisassembler, opcodes16)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/opcodes16"), TArchitecture::x86_16));
}

TEST(TDisassembler, opcodes32)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/opcodes32"), TArchitecture::x86_32));
}

TEST(TDisassembler, invalid)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/invalid"), TArchitecture::x86_32));
}

TEST(TDisassembler, exeheader)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/exeheader"), TArchitecture::x86_16));
}

TEST(TDisassembler, strcpy)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/strcpy"), TArchitecture::x86_32));
}

TEST(TDisassembler, test1)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/test1"), TArchitecture::x86_32));
}

TEST(TDisassembler, test2)
{
    DisassemblerRunner d;

    ASSERT_TRUE(d.Run(TString("asm/test2"), TArchitecture::x86_32));
}
