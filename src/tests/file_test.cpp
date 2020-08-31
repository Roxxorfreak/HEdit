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

TEST(TFile, ReadAt)
{
    unsigned char buffer[64];
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);
    TFile file2(TestDataFactory::GetFilesDir() + "test.zip", false);

    // Read from closed file (with and without cache)
    ASSERT_EQ(0u, file.ReadAt(buffer, 4, 0x00004567));
    ASSERT_EQ(0u, file2.ReadAt(buffer, 4, 0x00004567));

    // Read from openend file (with and without cache)
    // With cache
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(4u, file.ReadAt(buffer, 4, 0x00004567));
    ASSERT_EQ(0x99, buffer[0]);
    ASSERT_EQ(0xA6, buffer[1]);
    ASSERT_EQ(0x7B, buffer[2]);
    ASSERT_EQ(0xF9, buffer[3]);
    ASSERT_EQ(4u, file.ReadAt(buffer, 4, 0x00004568));
    ASSERT_EQ(0xA6, buffer[0]);
    ASSERT_EQ(0x7B, buffer[1]);
    ASSERT_EQ(0xF9, buffer[2]);
    ASSERT_EQ(0x24, buffer[3]);
    // No cache
    ASSERT_EQ(true, file2.Open(TFileMode::READ));
    ASSERT_EQ(4u, file2.ReadAt(buffer, 4, 0x00004567));
    ASSERT_EQ(0x99, buffer[0]);
    ASSERT_EQ(0xA6, buffer[1]);
    ASSERT_EQ(0x7B, buffer[2]);
    ASSERT_EQ(0xF9, buffer[3]);

    // Out of range
    ASSERT_EQ(0u, file.ReadAt(buffer, 1, 0x00045678));
    ASSERT_EQ(0u, file2.ReadAt(buffer, 1, 0x00045678));
}

TEST(TFile, Write)
{
    TString file_name = TestDataFactory::GetFilesDir() + "test.dat";
    TFile file(file_name, true);
    const unsigned char* text = reinterpret_cast<unsigned char*>("Hello");

    // Write to closed file
    ASSERT_EQ(0u, file.Write(text, 5));

    // Write to read-only file
    ASSERT_EQ(false, file.Open(TFileMode::READ)) << "<" << file_name.ToString() << "> must be deleted prior to test run";
    ASSERT_EQ(0u, file.Write(text, 5));

    // Write to r/w file
    ASSERT_EQ(true, file.Open(TFileMode::CREATE));
    ASSERT_EQ(5u, file.Write(text, 5));
    ASSERT_EQ(5, file.FileSize());
    file.Close();

    // Delete test file
    ASSERT_EQ(0, _unlink(file_name.ToString())) << "Delete failed for <" << file_name.ToString() << ">";
}

TEST(TFile, WriteAt)
{
    TString file_name = TestDataFactory::GetFilesDir() + "test.dat";
    TFile file(file_name, true);
    const unsigned char* text = reinterpret_cast<unsigned char*>("Hello");

    // Write to closed file
    ASSERT_EQ(0u, file.WriteAt(text, 5, 42));

    // Write to read-only file
    ASSERT_EQ(false, file.Open(TFileMode::READ)) << "<" << file_name.ToString() << "> must be deleted prior to test run";
    ASSERT_EQ(0u, file.WriteAt(text, 5, 42));

    // Write to r/w file
    ASSERT_EQ(true, file.Open(TFileMode::CREATE));
    ASSERT_EQ(5u, file.WriteAt(text, 5, 42));
    ASSERT_EQ(47, file.FileSize());
    file.Close();

    // Delete test file
    ASSERT_EQ(0, _unlink(file_name.ToString())) << "Delete failed for <" << file_name.ToString() << ">";
}

TEST(TFile, Seek)
{
    TFile file(TestDataFactory::GetFilesDir() + "test.zip", true);

    // Seek in closed file
    ASSERT_EQ(false, file.Seek(10));

    // Seek in open file
    ASSERT_EQ(true, file.Open(TFileMode::READ));
    ASSERT_EQ(false, file.Seek(-10));
    ASSERT_EQ(true, file.Seek(0));
    ASSERT_EQ(true, file.Seek(129006));
    ASSERT_EQ(true, file.Seek(0));
    ASSERT_EQ(true, file.Seek(229006));
}
