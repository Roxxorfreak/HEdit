// Copyright (c) 2020 Roxxorfreak

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
