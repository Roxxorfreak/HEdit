// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TValueProcessor, FormatValue_Default)
{
    TValueProcessor f;

    // Default format: hex
    ASSERT_STREQ("0x17", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0x3617", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("0x00143617", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());
    ASSERT_STREQ("0x0000100000143617", f.FormatValue(17592187368983, TValueSize::QWORD, false).ToString());
}

TEST(TValueProcessor, FormatValue_Decimal)
{
    TValueProcessor f;

    // Decimal
    f.SetNumberFormat(TNumberFormat::DEC);
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::QWORD, false).ToString());
    ASSERT_STREQ("-1324567", f.FormatValue(-1324567, TValueSize::QWORD, true).ToString());
}

TEST(TValueProcessor, FormatValue_Binary)
{
    TValueProcessor f;

    // Binary
    f.SetNumberFormat(TNumberFormat::BIN);
    ASSERT_STREQ("00010111", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0011011000010111", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("00000000000101000011011000010111", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());
    ASSERT_STREQ("0000000000000000000100000000000000000000000101000011011000010111", f.FormatValue(17592187368983, TValueSize::QWORD, false).ToString());
    ASSERT_STREQ("-0011011000010111", f.FormatValue(-1324567, TValueSize::WORD, true).ToString());
    ASSERT_STREQ("-00000001", f.FormatValue(-1, TValueSize::BYTE, true).ToString());
    ASSERT_STREQ("11111111", f.FormatValue(-1, TValueSize::BYTE, false).ToString());
}

TEST(TValueProcessor, FormatValue_Hex)
{
    TValueProcessor f;

    // Hex-decimal
    f.SetNumberFormat(TNumberFormat::HEX);
    ASSERT_STREQ("0x0B", f.FormatValue(11, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0x000B", f.FormatValue(11, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("-0x000B", f.FormatValue(-11, TValueSize::WORD, true).ToString());
    ASSERT_STREQ("0xFF", f.FormatValue(255, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("-0x01", f.FormatValue(-1, TValueSize::BYTE, true).ToString());
    ASSERT_STREQ("0xFF", f.FormatValue(-1, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0xFFFF", f.FormatValue(65535, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("-0x0001", f.FormatValue(-1, TValueSize::WORD, true).ToString());
    ASSERT_STREQ("0xFFFF", f.FormatValue(-1, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("0xFFFFFFFF", f.FormatValue(4294967295, TValueSize::DWORD, false).ToString());
    ASSERT_STREQ("-0x00000001", f.FormatValue(-1, TValueSize::DWORD, true).ToString());
    ASSERT_STREQ("0xFFFFFFFF", f.FormatValue(-1, TValueSize::DWORD, false).ToString());
}
