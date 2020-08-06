// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TComparator, Add)
{
    TComparator comparator;

    // Add one editor
    ASSERT_EQ(0, comparator.Add(nullptr));

    // Add all possible editors and check result
    for (int32_t i = 1; i < HE_MAX_EDITORS; i++)
    {
        ASSERT_EQ(i, comparator.Add(nullptr));
    }

    // Add one too many
    ASSERT_EQ(-1, comparator.Add(nullptr));

    // Validate number of editors
    ASSERT_EQ(HE_MAX_EDITORS, comparator.EditorCount());
}

TEST(TComparator, CompareOffset)
{
    TComparator comparator;

    // Add 2 editors
    ASSERT_EQ(0, comparator.Add(nullptr));
    ASSERT_EQ(1, comparator.Add(nullptr));

    // All offsets are invalid, resulting in 0x00 data to be assumed 
    ASSERT_EQ(true, comparator.CompareOffset(0));
    ASSERT_EQ(true, comparator.CompareOffset(42));
    ASSERT_EQ(true, comparator.CompareOffset(12345678));
}
