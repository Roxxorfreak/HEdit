// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TValueProcessor, FormatValue)
{
    TValueProcessor f;

    // Default format: hex
    ASSERT_STREQ("0x17", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0x3617", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("0x00143617", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());
    ASSERT_STREQ("0x0B", f.FormatValue(11, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0x000B", f.FormatValue(11, TValueSize::WORD, false).ToString());

    // Decimal
    f.SetNumberFormat(TNumberFormat::DEC);
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("1324567", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());

    // Binary
    f.SetNumberFormat(TNumberFormat::BIN);
    ASSERT_STREQ("00010111", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0011011000010111", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("00000000000101000011011000010111", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());

    // Hex-decimal
    f.SetNumberFormat(TNumberFormat::BIN);
    ASSERT_STREQ("00010111", f.FormatValue(1324567, TValueSize::BYTE, false).ToString());
    ASSERT_STREQ("0011011000010111", f.FormatValue(1324567, TValueSize::WORD, false).ToString());
    ASSERT_STREQ("00000000000101000011011000010111", f.FormatValue(1324567, TValueSize::DWORD, false).ToString());
}
