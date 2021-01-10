// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Initialize all script commands.
 */
void TScriptCommandManager::LoadScriptCommands()
{
    this->script_command_.clear();
    this->AddCommand("",  nullptr,  0);  // The empty command
    this->AddCommand("return",   returnProc,   0);
    this->AddCommand("gotoxy",   gotoxyProc,   2, TScriptParameter::X, TScriptParameter::Y);
    this->AddCommand("print",    printProc,    1, TScriptParameter::TEXT1);
    this->AddCommand("cprintb",  cprintbProc,  3, TScriptParameter::TEXT1, TScriptParameter::OFFSET, TScriptParameter::BYTE);
    this->AddCommand("cprintw",  cprintwProc,  3, TScriptParameter::TEXT1, TScriptParameter::OFFSET, TScriptParameter::WORD);
    this->AddCommand("cprintd",  cprintdProc,  3, TScriptParameter::TEXT1, TScriptParameter::OFFSET, TScriptParameter::DWORD);
    this->AddCommand("printub",  printubProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printuw",  printuwProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printud",  printudProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printhb",  printhbProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printhw",  printhwProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printhd",  printhdProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printsb",  printsbProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printsw",  printswProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printsd",  printsdProc,  3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printf",   printfProc,   3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printd",   printdProc,   3, TScriptParameter::TEXT1, TScriptParameter::TEXT2, TScriptParameter::OFFSET);
    this->AddCommand("printv",   printvProc,   2, TScriptParameter::TEXT1, TScriptParameter::VARIABLE2);
    this->AddCommand("printds",  printdsProc,  2, TScriptParameter::OFFSET, TScriptParameter::WORD);
    this->AddCommand("prinths",  printhsProc,  2, TScriptParameter::OFFSET, TScriptParameter::WORD);
    this->AddCommand("printxor", printxorProc, 2, TScriptParameter::OFFSET, TScriptParameter::WORD);
    this->AddCommand("declare",  nullptr,      2, TScriptParameter::VARIABLE1, TScriptParameter::FLAG);
    this->AddCommand("loadb",    loadbProc,    2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("loadw",    loadwProc,    2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("loadd",    loaddProc,    2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("loadq",    loadqProc,    2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("set",      setProc,      2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("add",      addProc,      2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("sub",      subProc,      2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);
    this->AddCommand("incr",     incrProc,     1, TScriptParameter::VARIABLE2);
    this->AddCommand("decr",     decrProc,     1, TScriptParameter::VARIABLE2);
    this->AddCommand("loop",     loopProc,     0);
    this->AddCommand("until",    untilProc,    2, TScriptParameter::VARIABLE2, TScriptParameter::OFFSET);

    // Compact vector
    this->script_command_.shrink_to_fit();
}

/**
 * Adds a new script command.
 * @param name The case-sensitive name of the command.
 * @param execution_proc The function that executes the command. May be nullptr, if no explicit execution is needed.
 * @param parameter_count The number of parameters used by this command.
 * @param param1 The first parameter.
 * @param param2 The second parameter.
 * @param param3 The third parameter.
 * @param param4 The fourth parameter.
 */
void TScriptCommandManager::AddCommand(const char* name, bool (*execution_proc)(TExecutionContext *ec), int32_t parameter_count, TScriptParameter param1, TScriptParameter param2,
    TScriptParameter param3, TScriptParameter param4)
{
    TScriptCommand command;

    // Store the instruction name (the command itself)
    command.name = name;

    // Store the parameters
    command.parameter_count = parameter_count;
    command.variable[0] = param1;
    command.variable[1] = param2;
    command.variable[2] = param3;
    command.variable[3] = param4;
    command.variable[4] = TScriptParameter::NONE;
    command.execution_proc = execution_proc;

    this->script_command_.push_back(command);
}

/**
 * Gets the command with the specified name.
 * @param name The name of the script command to find.
 * @return A pointer to the script command or nullptr if none found.
 */
TScriptCommand* TScriptCommandManager::GetCommandByName(const char* name) noexcept
{
    // Process all instructions
    for (auto &command : this->script_command_)
    {
        // If the name matches a script command, return failure
        if (command.name.Equals(name) == true) return &command;
    }

    // Command not found
    return nullptr;
}

/**
 * Writes the specified text at the current position into the output buffer.
 * @param ec The execution context of the current instruction.
 * @param text The text to output.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::Output(TExecutionContext* ec, const char* text)
{
    // Print the text onto the virtual screen
    ec->virtual_screen_buffer[ec->cursor_y - 1].InsertAt(ec->cursor_x - 1, text);

    // Advance the horizontal cursor position by the length of the text
    ec->cursor_x += strlen(text);

    // Return success
    return true;
}

/**
 * Ends the execution of a script.
 * @param ec The execution context of the current instruction.
 * @return false to stop the script execution.
 */
bool TScriptCommandManager::returnProc(TExecutionContext* ec) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(ec);
    #endif
    // Stop the script
    return false;
}

/**
 * Positions the text cursor on the virtual screen.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::gotoxyProc(TExecutionContext* ec) noexcept
{
    // Set the virtual cursor
    ec->cursor_x = ec->si->x_val;
    ec->cursor_y = ec->si->y_val;

    // Allow the script to continue
    return true;
}

/**
 * Prints a text.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printProc(TExecutionContext* ec)
{
    // Print the text onto the virtual screen
    return Output(ec, ec->si->text[0]);
}

/**
 * Conditional print byte.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::cprintbProc(TExecutionContext* ec)
{
    unsigned char buffer[4];

    // Read one byte at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, 1, ec->position + ec->si->offset);

    // Ensure a byte was read
    if (bytes != 1) return false;

    // If the comparison was successfull, print the text
    if (buffer[0] != ec->si->byte_val) return true;

    // Print the text
    return Output(ec, ec->si->text[0]);
}

/**
 * Conditional print word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::cprintwProc(TExecutionContext* ec)
{
    uint16_t word_value = 0;

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&word_value), sizeof(word_value), ec->position + ec->si->offset);

    // Ensure a word was read
    if (bytes != sizeof(word_value)) return false;

    // If the comparison was successfull, print the text
    if (ec->si->word_val != word_value) return true;

    // Print the text
    return Output(ec, ec->si->text[0]);
}

/**
 * Conditional print double word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::cprintdProc(TExecutionContext* ec)
{
    uint32_t dword_value = 0;

    // Read the double word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&dword_value), sizeof(dword_value), ec->position + ec->si->offset);

    // Ensure a double word was read
    if (bytes != sizeof(dword_value)) return false;

    // If the comparison was successfull, print the text
    if (ec->si->dword_val != dword_value) return true;

    // Print the text
    return Output(ec, ec->si->text[0]);
}

/**
 * Print unsigned byte.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printubProc(TExecutionContext* ec)
{
    unsigned char buffer[4] = {};
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the byte at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, 1, ec->position + ec->si->offset);

    // Ensure a byte was read
    if (bytes != 1) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIu8 "%s", ec->si->text[0].ToString(), static_cast<uint8_t>(buffer[0]), ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print unsigned word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printuwProc(TExecutionContext* ec)
{
    uint16_t word_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&word_value), sizeof(word_value), ec->position + ec->si->offset);

    // Ensure a word was read
    if (bytes != sizeof(word_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIu16 "%s", ec->si->text[0].ToString(), word_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print unsigned double word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printudProc(TExecutionContext* ec)
{
    uint32_t dword_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the double word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&dword_value), sizeof(dword_value), ec->position + ec->si->offset);

    // Ensure a double word was read
    if (bytes != sizeof(dword_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIu32 "%s", ec->si->text[0].ToString(), dword_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print hexdecimal byte.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printhbProc(TExecutionContext* ec)
{
    unsigned char buffer[4] = {};
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the byte at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, 1, ec->position + ec->si->offset);

    // Ensure a byte was read
    if (bytes != 1) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%02" PRIX8 "%s", ec->si->text[0].ToString(), static_cast<int8_t>(buffer[0]), ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print hexdecimal word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printhwProc(TExecutionContext* ec)
{
    uint16_t word_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&word_value), sizeof(word_value), ec->position + ec->si->offset);

    // Ensure a word was read
    if (bytes != sizeof(word_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%04" PRIX16 "%s", ec->si->text[0].ToString(), word_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print hexdecimal double word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printhdProc(TExecutionContext* ec)
{
    uint32_t dword_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the double word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&dword_value), sizeof(dword_value), ec->position + ec->si->offset);

    // Ensure a double word was read
    if (bytes != sizeof(dword_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%08" PRIX32 "%s", ec->si->text[0].ToString(), dword_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print signed byte.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printsbProc(TExecutionContext* ec)
{
    unsigned char buffer[4] = {};
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the byte at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, 1, ec->position + ec->si->offset);

    // Ensure a byte was read
    if (bytes != 1) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIi8 "%s", ec->si->text[0].ToString(), static_cast<int8_t>(buffer[0]), ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print signed word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printswProc(TExecutionContext* ec)
{
    int16_t word_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&word_value), sizeof(word_value), ec->position + ec->si->offset);

    // Ensure a word was read
    if (bytes != sizeof(word_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIi16 "%s", ec->si->text[0].ToString(), word_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print signed double word.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printsdProc(TExecutionContext* ec)
{
    int32_t dword_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&dword_value), sizeof(dword_value), ec->position + ec->si->offset);

    // Ensure a word was read
    if (bytes != sizeof(dword_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%" PRIi32 "%s", ec->si->text[0].ToString(), dword_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print float.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printfProc(TExecutionContext* ec)
{
    float float_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32] = {};

    // Read the float at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&float_value), sizeof(float_value), ec->position + ec->si->offset);

    // Ensure a float was read
    if (bytes != sizeof(float_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%f%s", ec->si->text[0].ToString(), static_cast<double>(float_value), ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print double.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printdProc(TExecutionContext* ec)
{
    double double_value = 0;
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 64] = {};

    // Read the float at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&double_value), sizeof(double_value), ec->position + ec->si->offset);

    // Ensure a float was read
    if (bytes != sizeof(double_value)) return false;

    // Create the string
    snprintf(temp_string, sizeof(temp_string), "%s%e%s", ec->si->text[0].ToString(), double_value, ec->si->text[1].ToString());

    // Print the string
    return Output(ec, temp_string);
}

/**
 * Print decimal string.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printdsProc(TExecutionContext* ec)
{
    unsigned char buffer[HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH + 1] = {};

    // Reset the buffer
    memset(buffer, 0, sizeof(buffer));

    // Check string length against bounds
    if (ec->si->word_val > HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH) ec->si->word_val = HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH;

    // Read the string at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, ec->si->word_val, ec->position + ec->si->offset);

    // Ensure the string was read
    if (bytes != ec->si->word_val) return false;

    // Print the string
    return Output(ec, reinterpret_cast<char*>(buffer));
}

/**
 * Print hexdecimal string.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printhsProc(TExecutionContext* ec)
{
    char temp_string[4] = {};
    unsigned char buffer[HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH] = {};

    // Reset the buffer
    memset(buffer, 0, sizeof(buffer));

    // Check string length against bounds
    if (ec->si->word_val > HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH) ec->si->word_val = HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH;

    // Read the string at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, ec->si->word_val, ec->position + ec->si->offset);

    // Ensure the string was read
    if (bytes != ec->si->word_val) return false;

    // Output HEX String
    for (uint16_t j = 0; j < ec->si->word_val; j++)
    {
        snprintf(temp_string, sizeof(temp_string), "%02" PRIX8, buffer[j]);
        ec->virtual_screen_buffer[ec->cursor_y - 1].InsertAt(ec->cursor_x - 1 + (j * 2), temp_string);
    }

    // Advance the cursor
    ec->cursor_x += ec->si->word_val;

    // Allow the script to continue
    return true;
}

/**
 * Prints the xor'ed value of all characters at the specified offset for the specified length.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printxorProc(TExecutionContext* ec)
{
    char temp_string[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH] = {};
    unsigned char buffer[HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH] = {};

    // Reset the buffer
    memset(buffer, 0, sizeof(buffer));

    // Check string length against bounds
    if (ec->si->word_val > HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH) ec->si->word_val = HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH;

    // Read the string at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, ec->si->word_val, ec->position + ec->si->offset);

    // Ensure the string was read
    if (bytes != ec->si->word_val) return false;

    // Create and output xor'ed value
    auto checksum = buffer[0];
    for (uint16_t j = 1; j < ec->si->word_val; j++)
    {
        checksum ^= buffer[j];
    }
    snprintf(temp_string, sizeof(temp_string), "%02" PRIX8, checksum);

    // Print the result
    return Output(ec, temp_string);
}

/**
 * Load byte from offset.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::loadbProc(TExecutionContext* ec) noexcept
{
    unsigned char buffer[4];

    // Read the byte at the specified offset
    const auto bytes = ec->file->ReadAt(buffer, 1, ec->position + ec->si->offset);

    // Ensure the byte was read
    if (bytes != 1) return false;

    // Load the byte into the variable
    if (ec->variable[1] != nullptr) ec->variable[1]->value_ = static_cast<uint64_t>(buffer[0]);

    // Allow the script to continue
    return true;
}

/**
 * Load word from offset.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::loadwProc(TExecutionContext* ec) noexcept
{
    uint16_t word_val;

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&word_val), sizeof(word_val), ec->position + ec->si->offset);

    // Ensure the word was read
    if (bytes != sizeof(word_val)) return false;

    // Load the word into the variable
    if (ec->variable[1] != nullptr) ec->variable[1]->value_ = static_cast<uint64_t>(word_val);

    // Allow the script to continue
    return true;
}

/**
 * Load double word from offset.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::loaddProc(TExecutionContext* ec) noexcept
{
    uint32_t dword_val;

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&dword_val), sizeof(dword_val), ec->position + ec->si->offset);

    // Ensure the double word was read
    if (bytes != sizeof(dword_val)) return false;

    // Load the double word into the variable
    if (ec->variable[1] != nullptr) ec->variable[1]->value_ = static_cast<uint64_t>(dword_val);

    // Allow the script to continue
    return true;
}

/**
 * Load quad word from offset.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::loadqProc(TExecutionContext* ec) noexcept
{
    uint64_t qword_val;

    // Read the word at the specified offset
    const auto bytes = ec->file->ReadAt(reinterpret_cast<unsigned char*>(&qword_val), sizeof(qword_val), ec->position + ec->si->offset);

    // Ensure the double word was read
    if (bytes != sizeof(qword_val)) return false;

    // Load the double word into the variable
    if (ec->variable[1] != nullptr) ec->variable[1]->value_ = qword_val;

    // Allow the script to continue
    return true;
}

/**
 * Set variable value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::setProc(TExecutionContext* ec) noexcept
{
    // Set the variable value
    if (ec->variable[1] != nullptr) ec->variable[1]->value_ = static_cast<uint64_t>(ec->si->offset);

    // Allow the script to continue
    return true;
}

/**
 * Add to the variable value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it
 */
bool TScriptCommandManager::addProc(TExecutionContext* ec) noexcept
{
    // Add the value
    if (ec->variable[1] != nullptr) ec->variable[1]->Add(static_cast<int64_t>(ec->si->offset));

    // Allow the script to continue
    return true;
}

/**
 * Subtract from the variable value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::subProc(TExecutionContext* ec) noexcept
{
    // Subtract the value
    if (ec->variable[1] != nullptr) ec->variable[1]->Sub(static_cast<int64_t>(ec->si->offset));

    // Allow the script to continue
    return true;
}

/**
 * Increase the variable value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::incrProc(TExecutionContext* ec) noexcept
{
    // Increase the value
    if (ec->variable[1] != nullptr) ec->variable[1]->Add(1);

    // Allow the script to continue
    return true;
}

/**
 * Decrease the variable value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::decrProc(TExecutionContext* ec) noexcept
{
    // Decrease the value
    if (ec->variable[1] != nullptr) ec->variable[1]->Sub(1);

    // Allow the script to continue
    return true;
}

/**
 * Start loop.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::loopProc(TExecutionContext* ec) noexcept
{
    // Check the loop depth
    if (ec->label_stack.used_labels < HE_SCRIPT_MAX_CASCADED_LOOP_DEPTH)
    {
        // Remember the instruction number
        ec->label_stack.label_number[ec->label_stack.used_labels] = ec->instruction_number;

        // Increment the label counter
        ec->label_stack.used_labels++;
    }

    // Allow the script to continue
    return true;
}

/**
 * Jump to loop start until the variable has the specified value.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::untilProc(TExecutionContext* ec) noexcept
{
    // Check there are labels
    if (ec->label_stack.used_labels <= 0) return true;

    // Decrement the number of used labels
    ec->label_stack.used_labels--;

    // Check, if the variables match. If they match, dont jump to the label
    if (ec->variable[1]->AsSignedInt32() == (static_cast<int32_t>(ec->si->offset))) return true;

    // Jump to the loop (-1 because i is incremented for this loop pass)
    ec->instruction_number = ec->label_stack.label_number[ec->label_stack.used_labels] - 1;

    // Allow the script to continue
    return true;
}

/**
 * Print variable contents.
 * @param ec The execution context of the current instruction.
 * @return true to continue the script execution, false to stop it.
 */
bool TScriptCommandManager::printvProc(TExecutionContext* ec)
{
    char temp[HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH + 32];

    // Create the string with the variable value
    snprintf(temp, sizeof(temp), "%s%08" PRIX32, ec->si->text[0].ToString(), ec->variable[1]->AsUnsignedInt32());

    // Print the string
    return Output(ec, temp);
}
