// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TFile, Open)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);

    // Open the test zip file for reading
    ASSERT_EQ(true, file.Open(TFileMode::READ));

    // Open non-existing file for reading
    TString file_name = TestDataFactory::GetFilesDir() + "test2.zip";
    file.AssignFileName(file_name);
    ASSERT_EQ(false, file.Open(TFileMode::READ)) << "<" << file_name.ToString() << "> must be deleted prior to test run";

    // Create file
    ASSERT_EQ(true, file.Open(TFileMode::CREATE));
    file.Close();

    // Open created file
    ASSERT_EQ(true, file.Open(TFileMode::READWRITE));
    file.Close();

    // Delete test file
    ASSERT_EQ(0, _unlink(file_name.ToString())) << "Delete failed for <" << file_name.ToString() << ">";
}
