// Copyright (c) 2021 Roxxorfreak

#include "_disassembler_runner.hpp"

/**
 * Runs a test on a specified test file.
 * Three files are involved: 
 * 1. <Test>.hex: The machine code to disassemble -> must exist.
 * 2. <Test>.out: The reference assembler code -> must exist.
 * 3. <Test>.temp: The disassembled code -> created during the test and compared to <Test>.out.
 * The test is successfull, if the output matches the code, i.e. if <Test>.temp equals <Test>.out.
 * @param name The name of the Test (aka. the name of the test data file, without extension).
 * @param bit_mode The bit mode (see TArchitecture)
 */
::testing::AssertionResult DisassemblerRunner::Run(TString name, TArchitecture bit_mode)
{
    TAsmBuffer buffer(HE_DA_RUNNER_OUTPUT_BUFFER_SIZE);
    TStringList reference_code, test_code;
    TDisassembler da;
    std::unique_ptr<TAsmInstruction[]> instruction(new TAsmInstruction[400]);

    // Create the file names
    TString code_name = name + ".out";
    TString input_name = name + ".hex";
    TString output_name = name + ".temp";

    // Load the binary data
    buffer.SetCodeLength(this->LoadHexCode(input_name, buffer.GetBuffer(), buffer.GetSize()));
    buffer.SetBaseAddress(0);

    // Validate data
    if (buffer.GetCodeLength() == 0)
    {
        return ::testing::AssertionFailure() << "Failed to open input file <" << HE_TEST_DATA_DIR << input_name << ">";
    }

    // Write binary output to file (for debugging)
    //this->data_factory_.WriteBinaryFile(name + ".bin", buffer.GetBuffer(), buffer.GetCodeLength());

    // Disassemble
    auto code_length = static_cast<std::size_t>(da.Disassemble(&buffer, instruction.get(), 400, bit_mode, TNumberFormat::HEX));

    // Validate data
    if (code_length == 0)
    {
        return ::testing::AssertionFailure() << "Nothing disassembled";
    }

    // Write assembler output to file
    if (!WriteAssemblerOutput(output_name, instruction.get(), code_length))
    {
        return ::testing::AssertionFailure() << "Failed to create assembler output file <" << HE_TEST_DATA_DIR << output_name << ">";
    }

    // Load files
    this->data_factory_.ReadTextFile(code_name, reference_code);
    this->data_factory_.ReadTextFile(output_name, test_code);

    // Validate file sizes
    if (reference_code.size() == 0)
    {
        return ::testing::AssertionFailure() << "Reference code (" << HE_TEST_DATA_DIR << code_name << ") is empty";
    }
    if (test_code.size() == 0)
    {
        return ::testing::AssertionFailure() << "Generated code (" << HE_TEST_DATA_DIR << output_name << ") is empty";
    }
    if (test_code.size() != reference_code.size())
    {
        return ::testing::AssertionFailure() << "Test code and reference code are different in size";
    }

    // Compare files
    for (std::size_t i = 0; i < reference_code.size(); i++)
    {
        if (test_code[i].Equals(reference_code[i]) == false)
        {
            return ::testing::AssertionFailure() << "Output differs in line " << (i+1) << ": <" << test_code[i] << "> != <" << reference_code[i] << ">";
        }
    }

    // Remove output file
    this->data_factory_.DeleteFile(output_name);

    // Return success
    return ::testing::AssertionSuccess();
}

/**
 * Loads hex codes from a text file.
 * These hex codes represent machine code.
 * The file may contain comments (//).
 * @param file_name The file to load (relative to the test data directory).
 * @param buffer The buffer to load the data into.
 * @param buffer_size The size of the buffer (in bytes).
 */
std::size_t DisassemblerRunner::LoadHexCode(const char* file_name, unsigned char* buffer, std::size_t buffer_size)
{
    std::size_t length = 0;

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
    while (!file.IsEOF())
    {
        // Read next line
        TString line = file.ReadLine();

        // Trim
        line.Trim();

        // Check for comment or empty line
        if (line.Length() == 0) continue;
        if (line[0u] == '/') continue;

        // Remove spaces
        line.StripChar(' ');

        // Line should be pure hex code, convert to binary
        length += this->ConvertHexString(&buffer[length], buffer_size - length, line);
    }

    // Close file
    file.Close();

    // Return data length
    return length;
}

/**
 * Converts a specified source string with hex chars (e.g. "FF023D") into it's binary representation.
 * @param target The target buffer that receives the converted string.
 * @param size The size of the target buffer in bytes.
 * @param source The null-terminated source string.
 * @return The number of converted (created) bytes, thus the length of the binary string.
 */
std::size_t DisassemblerRunner::ConvertHexString(unsigned char* target, std::size_t size, const char* source)
{
    TString buffer;

    // Clear the target buffer
    memset(target, 0, size);

    // Calculate the (expected) size of the target buffer in bytes.
    const auto length = strlen(source) / 2;

    // Initialize the buffer for the target string
    buffer.New(5);

    // Find the minimum of the length of the source string and the size of the input buffer
    const std::size_t byte_count = hedit_min(size, length);

    // Process the source string
    for (std::size_t i = 0; i < byte_count; i++)
    {
        // Copy one hex byte (two characters) to the buffer string
        strncpy_s(buffer, 3, &source[i << 1], 2);
        buffer[2u] = 0;
        target[i] = static_cast<unsigned char>(buffer.ParseHex());
    }

    // Return the length (in bytes) of the hex string that was stored
    return byte_count;
}

/**
 * Write the list of disassembled instruction to an assembler output file.
 * @param file_name The name of the file (relative to the test data directory).
 * @param instruction The array with the disassembled instructions.
 * @param count The number on disassembled instructions.
 */
bool DisassemblerRunner::WriteAssemblerOutput(const char* file_name, TAsmInstruction* instruction, std::size_t count)
{
    char adr[64];
    TAsmBuffer buffer(HE_DA_RUNNER_OUTPUT_BUFFER_SIZE);
    std::size_t max_length = 0;

    // Get maximum instruction length
    for (std::size_t i = 0; i < count; i++)
    {
        if (instruction[i].GetLength() > max_length) max_length = instruction[i].GetLength();
    }
    max_length++;

    // Create a string with lines of code
    for (std::size_t i = 0; i < count; i++)
    {
        // Add instruction address
        snprintf(adr, sizeof(adr), "%016" PRIX64 "  ", instruction[i].GetAddress());
        strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), adr);

        // Add hex codes
        for (std::size_t pos = 0; pos < instruction[i].GetLength(); pos++)
        {
            snprintf(adr, sizeof(adr), "%02" PRIX8 " ", static_cast<uint8_t>(instruction[i].GetMachineCode()[pos]));
            strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), adr);
        }

        // Fill with spaces to ensure alignment
        for (std::size_t pos = instruction[i].GetLength(); pos < max_length; pos++)
        {
            strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), "   ");
        }
        strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), "  ");

        // Add instruction code
        strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), instruction[i].assembler_code_.ToString());

        // Add line break
        strcat_s(reinterpret_cast<char*>(buffer.GetBuffer()), buffer.GetSize(), "\n");
    }

    // Set length
    auto length = strlen(reinterpret_cast<char*>(buffer.GetBuffer()));

    // Write to file
    auto success = (this->data_factory_.WriteBinaryFile(file_name, buffer.GetBuffer(), length) == length);

    // Return status
    return success;
}
