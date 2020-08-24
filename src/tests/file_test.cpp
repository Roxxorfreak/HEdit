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

TEST(TFile, Read)
{
    unsigned char buffer[64];
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TFile file2(TestDataFactory::GetFilesDir() + "test.zip", false);

    // Read from closed file (with and without cache)
    ASSERT_EQ(0u, file.Read(buffer, 4));
    ASSERT_EQ(0u, file2.Read(buffer, 4));

    // Read from openend file (with and without cache)
    // With cache
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(4u, file.Read(buffer, 4));
    ASSERT_EQ('P', buffer[0]);
    ASSERT_EQ('K', buffer[1]);
    ASSERT_EQ(0x03, buffer[2]);
    ASSERT_EQ(0x04, buffer[3]);
    // No cache
    ASSERT_EQ(true, file2.Open(TFileMode::READ));
    ASSERT_EQ(4u, file2.Read(buffer, 4));
    ASSERT_EQ(4u, file2.Read(buffer, 4));
    ASSERT_EQ(0x0A, buffer[0]);
    ASSERT_EQ(0x00, buffer[1]);
    ASSERT_EQ(0x00, buffer[2]);
    ASSERT_EQ(0x00, buffer[3]);
}
