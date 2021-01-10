// Copyright (c) 2021 Roxxorfreak

#include "headers_test.hpp"

TEST(TConsole, ListDirectory)
{
    int32_t file_count = 0;
    TStringList files;
    TConsole console;

    // The "test-data" directory is expected to contain 5 files (".", "..", "asm", "files", "scripts")
    ASSERT_EQ(5, console.ListDirectory(HE_TEST_DATA_DIR, "*", &files));

    // Process files
    for (TString file : files)
    {
        if (file_count == 0) ASSERT_STREQ(".", file.ToString());
        if (file_count == 1) ASSERT_STREQ("..", file.ToString());
        if (file_count == 2) ASSERT_STREQ("asm", file.ToString());
        if (file_count == 3) ASSERT_STREQ("files", file.ToString());
        if (file_count == 4) ASSERT_STREQ("scripts", file.ToString());
        file_count++;
    }
}
