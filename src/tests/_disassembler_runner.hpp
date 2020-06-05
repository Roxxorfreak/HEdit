// Copyright(C) 2020 Roxxorfreak

#ifndef HEDIT_SRC_TESTS__DISASSEMBLER_RUNNER_HPP_

// Header included
#define HEDIT_SRC_TESTS__DISASSEMBLER_RUNNER_HPP_

#include "gtest/gtest.h"
#include "_test_data_factory.hpp"

// The size of the buffer for the assembler output
constexpr int32_t HE_DA_RUNNER_OUTPUT_BUFFER_SIZE = 32767;

// Class for toggeling memory failure
class DisassemblerRunner
{
private:
    TString name_;
    TestDataFactory data_factory_;
public:
    ::testing::AssertionResult Run(TString name, TArchitecture bit_mode);
private:
    std::size_t LoadHexCode(const char* file_name, unsigned char* buffer, std::size_t buffer_size);
    std::size_t ConvertHexString(unsigned char* target, std::size_t size, const char* source);
    bool WriteAssemblerOutput(const char* file_name, TAsmInstruction* instruction, std::size_t count);

};

#endif   // HEDIT_SRC_TESTS__DISASSEMBLER_RUNNER_HPP_
