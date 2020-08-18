// Copyright (c) 2020 Roxxorfreak

#include "_test_data_factory.hpp"

/**
 * Loads the specified text file into a string list.
 * @param file_name The file to load (relative to the test data directory).
 * @param list The string list to fill.
 * @return true on success, false on failure.
 */
bool TestDataFactory::ReadTextFile(const char* file_name, TStringList& list)
{
    // Add path
    TString full_file = TString(HE_TEST_DATA_DIR) + file_name;

    // Create file object
    TFile file(full_file, true);

    // Try to open the file
    if (file.Open(TFileMode::READ) == false) return false;

    // Read the binary data
    while (!file.IsEOF())
    {
        list.push_back(file.ReadLine());
    }

    // Close file
    file.Close();

    // Return success
    return true;
}

/**
 * Loads the specified binary file into the specified buffer.
 * @param file_name The file to load (relative to the test data directory).
 * @param buffer The buffer to load the data into.
 * @param buffer_size The size of the buffer (in bytes).
 * @return The number of bytes read (0 on error).
 */
std::size_t TestDataFactory::ReadBinaryFile(const char* file_name, unsigned char* buffer, std::size_t buffer_size)
{
    // Add path
    TString full_file = TString(HE_TEST_DATA_DIR) + file_name;

    // Create file object
    TFile file(full_file, true);

    // Clear the target buffer
    memset(buffer, 0, buffer_size);

    // Try to open the file
    if (file.Open(TFileMode::READ) == false)
    {
        return 0;
    }

    // Read the binary data
    auto length = file.Read(buffer, buffer_size);

    // Close file
    file.Close();

    // Return data length
    return length;
}

/**
 * Writes the specified data from the buffer into the file. The file is created or overwritten.
 * @param file_name The file to write (relative to the test data directory).
 * @param buffer The buffer to write to the file.
 * @param buffer_size The size of the buffer (in bytes).
 * @return The number of bytes written to the file (or 0 on error).
 */
std::size_t TestDataFactory::WriteBinaryFile(const char* file_name, unsigned char* buffer, std::size_t buffer_size)
{
    // Add path
    TString full_file = TString(HE_TEST_DATA_DIR) + file_name;

    // Create file object
    TFile file(full_file, true);

    // Try to create output file
    if (file.Open(TFileMode::CREATE) == false)
    {
        return 0;
    }

    // Write data
    auto bytes_written = file.Write(buffer, buffer_size);

    // Close file
    file.Close();

    // Report success
    return bytes_written;
}

/**
 * Deletes the specified file.
 * @param file_name The file to delete (relative to the test data directory).
 */
void TestDataFactory::DeleteFile(const char* file_name)
{
    // Add path
    TString full_file = TString(HE_TEST_DATA_DIR) + file_name;

    // Delete file
    _unlink(full_file);
}

/**
 * Returns a string with the full path name of a config file for testing.
 * @return The full path name of a config file for testing.
 */
TString TestDataFactory::GetConfigName()
{
    return TString(HE_TEST_DATA_DIR) + "files/test.config";
}

/**
 * Returns a string with the directory name of the folder with the scripts for testing.
 * @return The full directory name of the script folder.
 */
TString TestDataFactory::GetScriptDir()
{
    return TString(HE_TEST_DATA_DIR) + "scripts/";
}
