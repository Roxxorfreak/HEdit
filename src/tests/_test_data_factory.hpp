// Copyright(C) 2020 Roxxorfreak

#ifndef HEDIT_SRC_TESTS__TEST_DATA_FACTORY_HPP_

// Header included
#define HEDIT_SRC_TESTS__TEST_DATA_FACTORY_HPP_

// Include HEdit headers
#include "headers.hpp"

// The directory where the test data is stored
constexpr char* HE_TEST_DATA_DIR = "../test-data/";

// Class for toggeling memory failure
class TestDataFactory
{
public:
    bool ReadTextFile(const char* file_name, TStringList& list);
    std::size_t ReadBinaryFile(const char* file_name, unsigned char* buffer, std::size_t buffer_size);
    std::size_t WriteBinaryFile(const char* file_name, unsigned char* buffer, std::size_t buffer_size);
    void DeleteFile(const char* file_name);
};

#endif   // HEDIT_SRC_TESTS__TEST_DATA_FACTORY_HPP_
