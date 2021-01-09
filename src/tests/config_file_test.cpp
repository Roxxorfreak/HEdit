// Copyright (c) 2021 Roxxorfreak

#include "headers_test.hpp"

TEST(TConfigFile, TConfigFile)
{
    TConfigFile config(TestDataFactory::GetConfigName());

    // Try to open the file
    ASSERT_EQ(true, config.Open(TFileMode::READ));
}

TEST(TConfigFile, ReadNextEntry)
{
    TConfigEntry entry;
    TConfigFile config(TestDataFactory::GetConfigName());

    // Try to open the file
    ASSERT_EQ(true, config.Open(TFileMode::READ));

    // Invalid pointer
    ASSERT_EQ(false, config.ReadNextEntry(nullptr));

    // Read all file content
    // 1st entry
    ASSERT_EQ(true, config.ReadNextEntry(&entry));
    ASSERT_STREQ("this", entry.key.ToString());
    ASSERT_STREQ("is a test", entry.value.ToString());
    // 2nd entry
    ASSERT_EQ(true, config.ReadNextEntry(&entry));
    ASSERT_STREQ("another", entry.key.ToString());
    ASSERT_STREQ("test", entry.value.ToString());
    // 3rd entry
    ASSERT_EQ(true, config.ReadNextEntry(&entry));
    ASSERT_STREQ("yet", entry.key.ToString());
    ASSERT_STREQ("\"another test\"", entry.value.ToString());
    // 4th entry
    ASSERT_EQ(true, config.ReadNextEntry(&entry));
    ASSERT_STREQ("valid", entry.key.ToString());
    ASSERT_STREQ("entry", entry.value.ToString());

    // No more entries
    ASSERT_EQ(false, config.ReadNextEntry(&entry));
}
