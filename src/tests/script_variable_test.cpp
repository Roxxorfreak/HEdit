// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TScriptVariable, AsSignedInt32)
{
    TScriptVariable var;

    // Assign 0x0100
    var.value_ = 0x0100;
    ASSERT_EQ(256, var.AsSignedInt32());

    // Assign 0xffffffff
    var.value_ = 4294967295;
    ASSERT_EQ(-1, var.AsSignedInt32());

    // Assign 0xffffffffff
    var.value_ = 1099511627775;
    ASSERT_EQ(-1, var.AsSignedInt32());

    // Assign 0xffffffffffff
    var.value_ = 0xffff000000ff;
    ASSERT_EQ(255, var.AsSignedInt32());

    // Assign 0xffffffff00000100
    var.value_ = 0xffffffff00000100;
    ASSERT_EQ(256, var.AsSignedInt32());
}

TEST(TScriptVariable, AsUnsignedInt8)
{
    TScriptVariable var;

    // Assign 0x0100
    var.value_ = 0x0100;
    ASSERT_EQ(0u, var.AsUnsignedInt8());

    // Assign 0xffffffff
    var.value_ = 4294967295;
    ASSERT_EQ(255u, var.AsUnsignedInt8());

    // Assign 0xffffffffff
    var.value_ = 1099511627775;
    ASSERT_EQ(255u, var.AsUnsignedInt8());

    // Assign 0xffffffffffff
    var.value_ = 0xffff000000ff;
    ASSERT_EQ(255u, var.AsUnsignedInt8());

    // Assign 0xffffffff00000100
    var.value_ = 0xffffffff00000100;
    ASSERT_EQ(0u, var.AsUnsignedInt8());
}

TEST(TScriptVariable, AsUnsignedInt16)
{
    TScriptVariable var;

    // Assign 0x0100
    var.value_ = 0x0100;
    ASSERT_EQ(256u, var.AsUnsignedInt16());

    // Assign 0xffffffff
    var.value_ = 4294967295;
    ASSERT_EQ(65535u, var.AsUnsignedInt16());

    // Assign 0xffffffffff
    var.value_ = 1099511627775;
    ASSERT_EQ(65535u, var.AsUnsignedInt16());

    // Assign 0xffffffffffff
    var.value_ = 0xffff000000ff;
    ASSERT_EQ(255u, var.AsUnsignedInt16());

    // Assign 0xffffffff00000100
    var.value_ = 0xffffffff00000100;
    ASSERT_EQ(256u, var.AsUnsignedInt16());
}

TEST(TScriptVariable, AsUnsignedInt32)
{
    TScriptVariable var;

    // Assign 0x0100
    var.value_ = 0x0100;
    ASSERT_EQ(256u, var.AsUnsignedInt32());

    // Assign 0xffffffff
    var.value_ = 4294967295;
    ASSERT_EQ(4294967295u, var.AsUnsignedInt32());

    // Assign 0xffffffffff
    var.value_ = 1099511627775;
    ASSERT_EQ(4294967295u, var.AsUnsignedInt32());

    // Assign 0xffffffffffff
    var.value_ = 0xffff000000ff;
    ASSERT_EQ(255u, var.AsUnsignedInt32());

    // Assign 0xffffffff00000100
    var.value_ = 0xffffffff00000100;
    ASSERT_EQ(256u, var.AsUnsignedInt32());
}

TEST(TScriptVariable, AsUnsignedInt64)
{
    TScriptVariable var;

    // Assign 0x0100
    var.value_ = 0x0100;
    ASSERT_EQ(0x0100u, var.AsUnsignedInt64());

    // Assign 0xffffffff
    var.value_ = 4294967295;
    ASSERT_EQ(4294967295u, var.AsUnsignedInt64());

    // Assign 0xffffffffff
    var.value_ = 1099511627775;
    ASSERT_EQ(1099511627775u, var.AsUnsignedInt64());

    // Assign 0xffffffffffff
    var.value_ = 0xffff000000ff;
    ASSERT_EQ(0xffff000000ffu, var.AsUnsignedInt64());

    // Assign 0xffffffff00000100
    var.value_ = 0xffffffff00000100;
    ASSERT_EQ(0xffffffff00000100u, var.AsUnsignedInt64());
}
