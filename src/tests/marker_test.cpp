// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TMarker, Clear)
{
    TMarker marker;

    // No range specified
    marker.start_ = 42;
    marker.end_ = 43;
    marker.Clear();
    ASSERT_EQ(-1, marker.start_);
    ASSERT_EQ(-1, marker.end_);
    ASSERT_EQ(0, marker.Length());
}

TEST(TMarker, Length)
{
    TMarker marker;

    // No range specified
    ASSERT_EQ(0, marker.Length());

    // Start specified, but no end
    marker.Clear();
    marker.start_ = 42;
    ASSERT_EQ(0, marker.Length());

    // End specified, but no start
    marker.Clear();
    marker.end_ = 42;
    ASSERT_EQ(0, marker.Length());

    // Valid range 1
    marker.Clear();
    marker.start_ = 42;
    marker.end_ = 42;
    ASSERT_EQ(1, marker.Length());

    // Valid range 2
    marker.Clear();
    marker.start_ = 55;
    marker.end_ = 42;
    ASSERT_EQ(14, marker.Length());
}

TEST(TMarker, Start)
{
    TMarker marker;

    // No range
    ASSERT_EQ(-1, marker.Start());

    // Valid range 1
    marker.Clear();
    marker.start_ = 42;
    marker.end_ = 43;
    ASSERT_EQ(42, marker.Start());

    // Valid range 2
    marker.Clear();
    marker.start_ = 55;
    marker.end_ = 43;
    ASSERT_EQ(43, marker.Start());
}

TEST(TMarker, IsSelected)
{
    TMarker marker;

    // No range
    ASSERT_EQ(false, marker.IsSelected(42));

    // Start specified, but no end
    marker.Clear();
    marker.start_ = 42;
    ASSERT_EQ(false, marker.IsSelected(42));

    // End specified, but no start
    marker.Clear();
    marker.end_ = 42;
    ASSERT_EQ(false, marker.IsSelected(42));

    // Valid range 1
    marker.Clear();
    marker.start_ = 42;
    marker.end_ = 43;
    ASSERT_EQ(false, marker.IsSelected(41));
    ASSERT_EQ(true, marker.IsSelected(42));
    ASSERT_EQ(true, marker.IsSelected(43));
    ASSERT_EQ(false, marker.IsSelected(44));

    // Valid range 2
    marker.Clear();
    marker.start_ = 53;
    marker.end_ = 52;
    ASSERT_EQ(false, marker.IsSelected(51));
    ASSERT_EQ(true, marker.IsSelected(52));
    ASSERT_EQ(true, marker.IsSelected(53));
    ASSERT_EQ(false, marker.IsSelected(54));
}