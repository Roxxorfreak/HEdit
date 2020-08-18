// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Initializes all elements of the script interpreter.
 * @param script_path The path to load script files from.
 */
TScriptInterpreter::TScriptInterpreter(const TString& script_path)
    : script_loaded_(false),
    error_line_(0),
    script_path_(script_path)
{
    // Load the known script commands
    this->script_command_manager_.LoadScriptCommands();
}

/**
 * Loads the specified script into an array of TScriptInstruction.
 * If any error occurs, the script is not loaded and the error string should contain the reason why.
 * @param file_name The file name of the script file to load.
 * @return true, if the script was loaded, false otherwise.
 */
bool TScriptInterpreter::LoadScript(const TString& file_name)
{
    int32_t script_start_line = 0;  // The zero-based line index of the line where the script code is expected to start.
    int32_t script_start_column = 0;  // The zero-based column index of the column where the script code is expected to start.
    TStringList source_code;  // The list with all source code lines of the script

    // Nothing loaded yet
    this->error_line_ = 0;
    this->error_string_ = "";
    this->script_loaded_ = false;
    this->script_code_.clear();
    this->script_variable_.clear();
    this->script_description_ = "n/a";

    // Store the script file name
    this->script_file_ = file_name;

    // Load the source file
    if (this->LoadScriptFile(file_name, source_code) == false) return false;

    // Get number of source code lines
    const auto lines_loaded = source_code.size();

    // Remove all comments (only the comment itself, empty lines are kept to prevail line numbers)
    for (std::size_t i = 0; i < lines_loaded; i++)
    {
        // Remove any comment from the line
        this->RemoveComment(source_code[i].operator char *());

        // Trim the line
        source_code[i].Trim();
    }

    // Check for the main function and extract the script description
    if (this->FindFunction("main", source_code, &script_start_line, &script_start_column) == false) return false;

    // Process all lines from the starting line on and extract the actual code lines of the main function
    if (this->GetFunctionCode(source_code, script_start_line, script_start_column) == false) return false;

    // Script successfully loaded
    this->script_loaded_ = true;

    // Return success
    return true;
}

/**
 * Returns the script descriptions that is found while loading the script.
 * If no script is loaded yet, it returns the default description provided in the class constructor.
 * @return The script description.
 */
const char* TScriptInterpreter::ScriptDescription() const noexcept
{
    return this->script_description_;
}

/**
 * Returns the last error that occured during the execution of the script.
 * @return The last error string.
 */
const char* TScriptInterpreter::GetLastError() const noexcept
{
    return this->error_string_;
}

/**
 * Executes the previously loaded script with all output going into the specified virtual screen buffer.
 * All offsets specified in the script are treated relative to the specified position.
 * @param file The (opened) file object to execute a script on (not the script file, but the source data).
 * @param position The file position to execute the script at.
 * @param virtual_screen_buffer A pointer to array of TStrings that represents the virtual screen buffer (used for the script output).
 * @return The number of lines that were output into the virtual screen buffer.
 */
int32_t TScriptInterpreter::ScriptExecuteAt(TFile* file, int64_t position, TString* virtual_screen_buffer)
{
    TExecutionContext ec;

    // The highest (virtual screen) line number that was accessed via any print* instruction
    int32_t max_line = 0;

    // Reset the content of all variables (not the variables itself!)
    for (auto& variable : this->script_variable_) variable.value_ = 0;

    // Check if a script was loaded. If not, exit returning the error generated during loading
    if (this->script_loaded_ == false)
    {
        // No script loaded, display the error message
        return this->OutputErrorMessage(virtual_screen_buffer);
    }

    // Prepare the execution context (start with cursor at upper left corner (1,1))
    ec.cursor_x = 1;
    ec.cursor_y = 1;
    ec.file = file;
    ec.position = position;
    ec.virtual_screen_buffer = virtual_screen_buffer;
    ec.label_stack.used_labels = 0;

    // Get number of instructions
    const auto instructions_loaded = this->script_code_.size();

    // Now execute one instruction after the other
    for (std::size_t instruction_ptr = 0; instruction_ptr < instructions_loaded; instruction_ptr++)
    {
        // Adjust the maximum line number that is used
        if (ec.cursor_y > max_line) max_line = ec.cursor_y;

        // Get current line of code
        auto& current_instruction = this->script_code_[instruction_ptr];

        // Evaluate variable values. If any *_var contains a valid variable name,
        // load the contents of this variable into the corresponding *_val variable
        TScriptVariable* var;

        // Offset
        var = this->GetVariable(current_instruction.offset_var);
        if (var != nullptr) current_instruction.offset = var->AsSignedInt32();
        // X
        var = this->GetVariable(current_instruction.x_var);
        if (var != nullptr) current_instruction.x_val = var->AsUnsignedInt8();
        // Y
        var = this->GetVariable(current_instruction.y_var);
        if (var != nullptr) current_instruction.y_val = var->AsUnsignedInt8();
        // Byte
        var = this->GetVariable(current_instruction.byte_var);
        if (var != nullptr) current_instruction.byte_val = var->AsUnsignedInt8();
        // Word
        var = this->GetVariable(current_instruction.word_var);
        if (var != nullptr) current_instruction.word_val = var->AsUnsignedInt16();
        // DWord
        var = this->GetVariable(current_instruction.dword_var);
        if (var != nullptr) current_instruction.dword_val = var->AsUnsignedInt32();
        // QWord
        var = this->GetVariable(current_instruction.qword_var);
        if (var != nullptr) current_instruction.qword_val = var->AsUnsignedInt64();

        // Validate the cursor position
        if ((ec.cursor_x < 1) || (ec.cursor_x > HE_SCRIPT_MAX_SCREEN_COLS))
        {
            // Create the error message
            this->error_line_ = current_instruction.line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size() , "Invalid cursor x position: Should be between 1 and %i, is %i", HE_SCRIPT_MAX_SCREEN_COLS, ec.cursor_x);

            // Display the error message
            return this->OutputErrorMessage(virtual_screen_buffer);
        }
        if ((ec.cursor_y < 1) || (ec.cursor_y > HE_SCRIPT_MAX_SCREEN_ROWS))
        {
            // Create the error message
            this->error_line_ = current_instruction.line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Invalid cursor y position: Should be between 1 and %i, is %i", HE_SCRIPT_MAX_SCREEN_ROWS, ec.cursor_y);

            // Display the error message
            return this->OutputErrorMessage(virtual_screen_buffer);
        }

        // Check if a script command is assigned to the decoded instruction. If no command is assigned, continue with the next script instruction.
        if (current_instruction.command == nullptr) continue;

        // Check if an instruction procedure is assigned to the script command. If no instruction procedure is assigned, continue with the next script instruction.
        if (current_instruction.command->execution_proc == nullptr) continue;

        // An instruction procedure is assigned to the script command, update execution context and prepare execution
        ec.instruction_number = static_cast<int32_t>(instruction_ptr);
        ec.si = &current_instruction;
        ec.variable[0] = this->GetVariable(current_instruction.variable[0]);
        ec.variable[1] = this->GetVariable(current_instruction.variable[1]);

        // Pass execution context to the execution function
        const auto continue_script = (*current_instruction.command->execution_proc)(&ec);

        // Reassign instruction pointer (it may have been modified by the execution function to make jumps/loops possible)
        instruction_ptr = static_cast<std::size_t>(ec.instruction_number);

        // If the script should not continue, end here
        if (continue_script == false) break;
    }

    // *** TESTING ***
    // Add the variables and their content at the end of the page
    #ifdef __ScriptDebug
    char temp[128];
    int32_t index = 0;
    snprintf(temp, sizeof(temp), "User variables: %z" PRIu32, this->script_variable_.size());
    virtual_screen_buffer[max_line + 2].InsertAt(1, temp);
    for (const auto& var : this->script_variable_)
    {
        if (var.is_signed_)
            snprintf(temp, sizeof(temp), "Variable \"%s\": %" PRIi32 " (signed)", var.name_.ToString(), var.AsSignedInt32());
        else
            snprintf(temp, sizeof(temp), "Variable \"%s\": %" PRIu32 " (unsigned)", var.name_.ToString(), var.AsUnsignedInt32());
        virtual_screen_buffer[max_line + 3 + index].InsertAt(1, temp);
        index++;
    }
    max_line += (this->script_variable_.size() + 3);
    #endif

    // Return the highest used line number
    return max_line;
}

/**
 * Outputs the current error message to the virtual screen buffer.
 * @param virtual_screen_buffer The virtual screen buffer that is used for the output.
 * @return The number of lines that were output into the virtual screen buffer.
 */
int32_t TScriptInterpreter::OutputErrorMessage(TString* virtual_screen_buffer)
{
    char line_number[32];

    // Convert the line number to a string
    memset(line_number, 0, sizeof(line_number));
    snprintf(line_number, sizeof(line_number), "%" PRIi32, this->error_line_);

    // Output the error message
    virtual_screen_buffer[0].Free();
    virtual_screen_buffer[1] = " ERROR: ";
    virtual_screen_buffer[1] += this->error_string_;
    virtual_screen_buffer[2] = " File: ";
    virtual_screen_buffer[2] += this->script_file_;
    virtual_screen_buffer[3] = " Line: ";
    virtual_screen_buffer[3] += line_number;
    virtual_screen_buffer[4].Free();

    // Return the number of used lines
    return 5;
}

/**
 * Loads the contents of the specified file into an array of strings.
 * @param file_name The name of the file to load.
 * @param script_code A pointer to an TStringList that receive the script source code lines.
 * @return true on success, false otherwise.
 */
bool TScriptInterpreter::LoadScriptFile(const TString& file_name, TStringList& script_code)
{
    TString alternate_file_name;

    // Ensure a file name is specified
    if (file_name.IsEmpty())
    {
        // Store the error description
        this->error_string_ = "No script file specified!";

        // Return failure
        return false;
    }

    // Create the file object
    auto file = new TFile(file_name, true);

    // Try to open the file with the specified name directly
    if (file->Open(TFileMode::READ) == false)
    {
        // The file could not be openend:
        // Create an alternate file name using the default script file path (from the settings)
        alternate_file_name = this->script_path_ + file_name;
        file->AssignFileName(alternate_file_name);

        // Try to open the plugin file at the alternate location
        if (file->Open(TFileMode::READ) == false)
        {
            // Store the error description
            this->error_string_ = "Unable to open script file (in ";
            this->error_string_ += this->script_path_;
            this->error_string_ += ")";

            // Delete the file object
            delete file;

            // Return failure
            return false;
        }
    }

    // Load the complete script into memory
    while (!file->IsEOF())
    {
        // Read the next line of text (do not skip empty lines to preserve the line numbers)
        script_code.push_back(file->ReadLine());
    }

    // Close the source file, the script is now loaded into memory
    file->Close();

    // Delete the file object
    delete file;

    // Ensure the script file is not empty
    if (script_code.size() == 0)
    {
        // Store the error description
        this->error_string_ = "Script file is empty";

        // Return failure
        return false;
    }

    // Return success
    return true;
}

/**
 * Searches the specified string from the specified position for the specified character token.
 * Whitespaces (space, tab) are ignored, any other character that is encountered results in a "not found" situation, if ignore_unknown_tokens is false.
 * @param string The string to search the token in.
 * @param start_index The zero-based index in the string to start from.
 * @param token The token character to find.
 * @param ignore_unknown_tokens true to ignore unknown tokens, false to return an error if an unknown (non-whitespace) token is encountered.
 * @return The zero-based index of the specified token or < 0 if the token was not found.
 */
int32_t TScriptInterpreter::FindToken(const char* string, int32_t start_index, const char token, bool ignore_unknown_tokens) noexcept
{
    // Ensure a valid string
    if (string == nullptr) return HE_SCRIPT_TOKEN_ERROR_INVALID_STRING;

    // Get the length of the source string
    const auto length = static_cast<int32_t>(strlen(string));

    // Ensure the specified index is in the string
    if (start_index >= length) return HE_SCRIPT_TOKEN_ERROR_EOL;

    // Process all characters
    for (int32_t i = start_index; i < length; i++)
    {
        // Check for the desired token
        if (string[i] == token)
        {
            // The token was found, return the index
            return i;
        }

        // Skip everything, if ignore flag is set
        if (ignore_unknown_tokens == true) continue;

        // Skip valid whitespaces
        if (string[i] == ' ') continue;
        if (string[i] == '\t') continue;

        // Any other token was encountered and "ignore_unknown_tokens" is false, thus return an error
        return HE_SCRIPT_TOKEN_ERROR_UNKNOWN_TOKEN;
    }

    // The token was not found in the string
    return HE_SCRIPT_TOKEN_ERROR_EOL;
}

/**
 * Processes all script lines, searches for the main function and extracts the script name.
 * The start of the script code is also determined.
 * @param function_name The name of the function to find.
 * @param script_code A reference to a TStringList that contains the script code.
 * @param start_line A pointer to an integer that receives the line index (zero-based) of the line to start looking for the opening bracket ({).
 * @param start_column A pointer to an integer that receives the column index (zero-based) of the column to start looking for the opening bracket ({).
 * @return true on success (the main function was found and the description extracted), false otherwise.
 */
bool TScriptInterpreter::FindFunction(const char* function_name, const TStringList& script_code, int32_t* start_line, int32_t* start_column)
{
    // Count the line numbers (one-based)
    int32_t line_number = 0;

    // Clear the code start information
    *start_line = 0;
    *start_column = 0;

    // Get the length of the requested function name
    const auto name_length = strlen(function_name);

    // Process all script lines
    for (auto line : script_code)
    {
        // Increment line number counter
        line_number++;

        // Skip all empty lines, the first non-empty line must contain the main function
        if (line.IsEmpty()) continue;

        // Check if the line starts with the function definition, thus the function name.
        if (line.Equals(function_name, name_length) == false)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Function name <%s> missing", function_name);

            // Return failure
            return false;
        }

        // Main function found, try to extract script description

        // Find the opening bracket
        auto bracket_index = this->FindToken(line, static_cast<int32_t>(name_length), '(', false);
        if (bracket_index < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Missing opening bracket";

            // Return failure
            return false;
        }

        // Find the opening double quote
        const auto start_quote_index = this->FindToken(line, bracket_index + 1, '"', false);
        if (start_quote_index < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Missing opening double quote";

            // Return failure
            return false;
        }

        // Find the closing double quote
        const auto end_quote_index = this->FindToken(line, start_quote_index + 1, '"', true);
        if (end_quote_index < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Missing closing double quote";

            // Return failure
            return false;
        }

        // Find the closing bracket
        bracket_index = this->FindToken(line, end_quote_index + 1, ')', false);
        if (bracket_index < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Missing closing bracket";

            // Return failure
            return false;
        }

        // Ensure there is no more data except an opening bracket {
        auto token_index = this->FindToken(line, bracket_index + 1, '{', false);
        if (token_index < 0)
        {
            // Check for end of line
            if (token_index != HE_SCRIPT_TOKEN_ERROR_EOL)
            {
                // Create the error message
                this->error_line_ = line_number;
                this->error_string_ = "{ or EOL expected";

                // Return failure
                return false;
            }
            else
            {
                // No opening bracket, we need to start interpreting the script from the next line
                *start_line = line_number;
                *start_column = 0;
            }
        }
        else
        {
            // There was an opening bracket, we need to start interpreting the script from this character
            *start_line = line_number - 1;
            *start_column = token_index;
        }

        // Ensure the script description is not empty
        if ((end_quote_index - start_quote_index - 1) <= 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Script description empty";

            // Return failure
            return false;
        }

        // Success: Extract script description
        this->script_description_ = line.SubString(static_cast<std::size_t>(start_quote_index) + 1, end_quote_index - start_quote_index - 1);

        // Return success
        return true;
    }

    // Create the error message
    this->error_string_ = "Main function missing";

    // Return failure
    return false;
}

/**
 * Processes all script lines from the specified line on, searches for an opening bracket and extracts all code (commands) up to the closing bracket.
 * @param script_code A reference to a TStringList that contains the script source code.
 * @param start_line The line index (zero-based) of the line to start looking for the opening bracket ({).
 * @param start_column The column index (zero-based) of the column to start looking for the opening bracket ({).
 * @return true on success, false otherwise.
 */
bool TScriptInterpreter::GetFunctionCode(TStringList& script_code, int32_t start_line, int32_t start_column)
{
    auto token_found = false;

    // Get number of script lines
    const auto script_lines = static_cast<int32_t>(script_code.size());

    // Process all script lines from the designated start line to find the script start
    for (int32_t i = start_line; i < script_lines; i++)
    {
        // Get the script line
        auto& line = script_code[static_cast<std::size_t>(i)];

        // Skip all empty lines
        if (line.IsEmpty()) continue;

        // Find the opening bracket ({) on the first non-empty script line (thus the start token)
        const auto token_index = this->FindToken(line, start_column, '{', false);
        if (token_index < 0)
        {
            // The token was not found, but every function has to start with the opening bracket
            this->error_line_ = i + 1;
            this->error_string_ = "Missing {";

            // Return failure
            return false;
        }

        // The opening bracket was found, the code starts here, adjust the line and column index
        token_found = true;
        start_line = i;
        start_column = token_index + 1;

        // End here
        break;
    }

    // Ensure the opening bracket was found
    if (token_found == false)
    {
        // There was no opening bracket in the script
        this->error_line_ = start_line + 1;
        this->error_string_ = "Missing {";

        // Return failure
        return false;
    }

    // No end token found yet
    token_found = false;

    // Process all script lines from the designated start line and process the commands
    for (int32_t i = start_line; i < script_lines; i++)
    {
        // Get the script line
        auto& line = script_code[static_cast<std::size_t>(i)];

        // Process all commands
        while (start_column < static_cast<int32_t>(line.Length()))
        {
            // If the function end was found, do not allow data on the same line
            if (token_found == true)
            {
                // Create the error message
                this->error_line_ = i + 1;
                this->error_string_ = "Code out of function";

                // Return failure
                return false;
            }

            // Check for closing bracket (})
            const auto token_index = this->FindToken(line, start_column, '}', false);
            if (token_index >= 0)
            {
                // The closing bracket that ends the function was found: Remember the token
                token_found = true;

                // Set the column index behind the bracket
                start_column = token_index + 1;
            }
            else
            {
                int32_t column_after_instruction;

                // Create empty instruction
                TScriptInstruction instruction;

                // No closing bracket, check for regular command
                if (this->DecodeInstruction(line, start_column, &instruction, &column_after_instruction, i + 1) == false) return false;

                // Add instruction
                this->script_code_.push_back(instruction);

                // Ensure the column index increases (to avoid deadlocks)
                if (column_after_instruction <= start_column)
                {
                    start_column++;
                }
                else
                {
                    start_column = column_after_instruction;
                }
            }
        }

        // If the end token (closing bracket) was found, end here
        if (token_found == true) break;

        // Start the new line in the first column
        start_column = 0;
    }

    // If no closing bracket was found, create an error
    if (token_found == false)
    {
        // Create the error message
        this->error_line_ = script_lines;
        this->error_string_ = "Missing }";

        // Return failure
        return false;
    }

    // Return success
    return true;
}

/**
 * Extracts a token from the specified string.
 * A token in this case is any data (like a string, variable, function name, number, etc.) that is limited by a whitespace or delimiting character.
 * @param source_string The source string to get the first token from.
 * @param target_string The targe buffer that receives the token data.
 * @param target_size The size of the target buffer in characters.
 * @param full_token_length A pointer to an integer that receives the total token length (token + whitespaces).
 * @return true on success, false if the token is too long.
 */
bool TScriptInterpreter::ExtractToken(const char* source_string, char* target_string, std::size_t target_size, int32_t* full_token_length) noexcept
{
    // Reset the total token length
    *full_token_length = 0;

    // Get the length of the source string (that may contain more than 1 instruction)
    const auto source_length = strlen(source_string);

    // Skip leading whitespaces
    std::size_t token_start = 0;
    for (std::size_t i = 0; i < source_length; i++)
    {
        // If any character is encountered that is not a whitespace, leave the loop
        if ((source_string[i] != ' ') && (source_string[i] != '\t')) break;

        // Count the character
        token_start++;
    }

    // Extract the first token
    std::size_t token_length = 0;
    for (std::size_t i = token_start; i < source_length; i++)
    {
        // If any character is encountered that marks the end of a token, leave the loop
        if (source_string[i] == 0) break;
        if (source_string[i] == '(') break;
        if (source_string[i] == ')') break;
        if (source_string[i] == ' ') break;
        if (source_string[i] == ';') break;
        if (source_string[i] == ',') break;
        if (source_string[i] == '\t') break;

        // Count the character
        token_length++;
    }

    // Ensure a valid token length
    if (token_length >= target_size) return false;

    // Copy the token to the target buffer
    strncpy_s(target_string, target_size, &source_string[token_start], token_length);

    // Store the full token length (token length + whitespace count)
    *full_token_length = static_cast<int32_t>(token_start + token_length);

    // Return success
    return true;
}

/**
 * Looks for comment delimiters // in the specified string and truncates the line there.
 * If there are no comments found, the line remains unchanged.
 * @param line The line to process.
 */
void TScriptInterpreter::RemoveComment(char* line) noexcept
{
    // Ensure a valid string
    if (line == nullptr) return;

    // Line with less than 2 characters can't have comments, since comments are 2 bytes long (//)
    if (strlen(line) < 2) return;

    // Process the line char by char
    for (std::size_t i = 0; i < strlen(line) - 1; i++)
    {
        if ((line[i] == '/') && (line[i + 1] == '/') && (!this->CharIsInText(line, i)))
        {
            // A comment was found, just cut the rest of the line
            line[i] = 0;
            return;
        }
    }
    return;
}

/**
 * Check, if the char at the specified position is in a text string delimited by "s
 * @param string The string to search.
 * @param start_index The position to start at.
 * @return true, if the character is in a string, false otherwise.
 */
bool TScriptInterpreter::CharIsInText(const char* string, std::size_t start_index) noexcept
{
    int32_t delimiter_count = 0;

    // Process the line backwards
    for (std::size_t i = 0; i < start_index; i++)
    {
        if (i > 0)
        {
            if ((string[i] == '"') && (string[i - 1] != '\\')) delimiter_count++;
            if ((string[i] == '\'') && (string[i - 1] != '\\')) delimiter_count++;
        }
        else
        {
            if (string[i] == '"') delimiter_count++;
            if (string[i] == '\'') delimiter_count++;
        }
    }
    return (delimiter_count % 2 == 0) ? false : true;
}

/**
 * Decodes a single instruction in the specified line into a TScriptInstruction structure.
 * There may be more instruction in the specified line, but only the first is decoded.
 * @param line The line with the instruction(s) to decode.
 * @param start_column_index The index within the column where to start parsing.
 * @param si The structure for the decoded script instuction.
 * @param new_column_index A pointer to an integer that receives the new position of the parser in the current line.
 * @param line_number The one-based line number (for error output).
 * @return true, if the instruction was decoded successfully, false otherwise.
 */
bool TScriptInterpreter::DecodeInstruction(const TString& line, int32_t start_column_index, TScriptInstruction* si, int32_t* new_column_index, int32_t line_number)
{
    int32_t new_token_index = 0;
    char token[HE_SCRIPT_MAX_PARSER_TOKEN_LENGTH];

    // Clear the token string
    memset(token, 0, sizeof(token));

    // No instruction found yet
    *new_column_index = 0;

    // Default cursor position
    si->x_val = 1;
    si->y_val = 1;

    // Store the original line number
    si->line_number = line_number;

    // Extract the first token, this should be the instruction name
    if (this->ExtractToken(line.SubString(static_cast<std::size_t>(start_column_index)), token, HE_SCRIPT_MAX_PARSER_TOKEN_LENGTH, &new_token_index) == false)
    {
        // Create the error message
        this->error_line_ = line_number;
        this->error_string_ = "Invalid command";

        // Return failure
        return false;
    }

    // Process all known instructions to check if the found token is a valid instruction
    auto command = this->script_command_manager_.GetCommandByName(token);

    // Instruction unknown, generate an error
    if (command == nullptr)
    {
        // Create the error message
        this->error_line_ = line_number;
        this->error_string_.New(128);
        snprintf(this->error_string_, this->error_string_.Size(), "Invalid instruction <%.100s>", token);

        // Return failure
        return false;
    }

    // Instruction found, store the instruction id
    si->command = command;

    // Set the parser position (column index) to the next character after the command name
    int32_t column_index = start_column_index + new_token_index;

    // Check if the command has parameters.
    if (command->parameter_count > 0)
    {
        // Find the opening bracket of the command (start of the parameters)
        column_index = this->FindToken(line, column_index, '(', false);

        // Ensure the bracket was found
        if (column_index < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Missing ( after <%.100s>", token);

            // Return failure
            return false;
        }
        else
        {
            // Advance to the first character after the opening bracket
            column_index++;
        }

        //
        auto new_variable = false;
        auto use_variable = false;

        // Process all expected parameters
        for (int32_t parameter_index = 0; parameter_index < command->parameter_count; parameter_index++)
        {
            int32_t token_index = 0;

            // Clear the token string
            memset(token, 0, sizeof(token));

            // Extract the next parameter
            if (this->ExtractParameter(line, column_index, &new_token_index, token, HE_SCRIPT_MAX_PARSER_TOKEN_LENGTH, line_number) == false) return false;

            // Check if the parameter was the last one
            if (parameter_index < command->parameter_count - 1)
            {
                // The parameter is not the last one, search for a comma
                token_index = this->FindToken(line, new_token_index, ',', false);

                // Ensure the delimiter was found
                if (token_index < 0)
                {
                    // Create the error message
                    this->error_line_ = line_number;
                    this->error_string_.New(128);
                    snprintf(this->error_string_, this->error_string_.Size(), "Missing comma after <%.100s>", token);

                    // Return failure
                    return false;
                }
            }
            else
            {
                // The parameter is the last one, search for a closing bracket
                token_index = this->FindToken(line, new_token_index, ')', false);

                // Ensure the delimiter was found
                if (token_index < 0)
                {
                    // Create the error message
                    this->error_line_ = line_number;
                    this->error_string_.New(128);
                    snprintf(this->error_string_, this->error_string_.Size(), "Missing ) after <%.100s>", token);

                    // Return failure
                    return false;
                }
            }

            // Advance onto the next token
            column_index = token_index + 1;

            // Evaluate the target variable for the token
            switch (command->variable[parameter_index])
            {
                case TScriptParameter::X:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->x_val, &si->x_var, TScriptParameterType::SHORT, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::Y:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->y_val, &si->y_var, TScriptParameterType::SHORT, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::OFFSET:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->offset, &si->offset_var, TScriptParameterType::LONG, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::TEXT1:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->text[0], nullptr, TScriptParameterType::STRING, false, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::TEXT2:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->text[1], nullptr, TScriptParameterType::STRING, false, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::BYTE:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->byte_val, &si->byte_var, TScriptParameterType::BYTE, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::WORD:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->word_val, &si->word_var, TScriptParameterType::WORD, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::DWORD:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->dword_val, &si->dword_var, TScriptParameterType::DWORD, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::QWORD:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->qword_val, &si->qword_var, TScriptParameterType::QWORD, true, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::VARIABLE1:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->variable[0], nullptr, TScriptParameterType::VARNAME, false, line_number) == false) return false;
                    new_variable = true;
                    break;
                }
                case TScriptParameter::VARIABLE2:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->variable[1], nullptr, TScriptParameterType::VARNAME, false, line_number) == false) return false;
                    use_variable = true;
                    break;
                }
                case TScriptParameter::FLAG:
                {
                    if (this->ProcessParameter(token, parameter_index, &si->flag, nullptr, TScriptParameterType::BOOL, false, line_number) == false) return false;
                    break;
                }
                case TScriptParameter::NONE:
                    break;
            }
        }

        // A new variable should be created
        if (new_variable)
        {
            // Check if the variable already exists
            if (this->VariableExists(si->variable[0]) == true)
            {
                // Create the error message
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Variable <%s> is already declared", si->variable[0].ToString());

                // Return failure
                return false;
            }

            // Create the new variable
            TScriptVariable variable;

            // Store the variable data
            variable.name_ = si->variable[0];
            variable.value_ = 0;
            variable.is_signed_ = si->flag;

            // Add the variable
            this->script_variable_.push_back(variable);
        }

        // A variable is to used
        if (use_variable)
        {
            // Check the variable for validity. The variable MUST exist.
            if (this->VariableExists(si->variable[1]) == false)
            {
                // Create the error message
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Undeclared variable <%s>", si->variable[1].ToString());

                // Return failure
                return false;
            }
        }

        // Check for valid cursor positions
        if ((si->x_val < 1) || (si->x_val > HE_SCRIPT_MAX_SCREEN_COLS))
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Invalid cursor x position: Should be between 1 and %i, is %i", HE_SCRIPT_MAX_SCREEN_COLS, si->x_val);

            // Return failure
            return false;
        }
        if ((si->y_val < 1) || (si->y_val > HE_SCRIPT_MAX_SCREEN_ROWS))
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Invalid cursor y position: Should be between 1 and %i, is %i", HE_SCRIPT_MAX_SCREEN_ROWS, si->y_val);

            // Return failure
            return false;
        }
    }

    // Find the colon that closes the instruction
    new_token_index = this->FindToken(line, column_index, ';', false);

    // Ensure the colon was found
    if (new_token_index < 0)
    {
        // Create the error message
        this->error_line_ = line_number;
        this->error_string_ = "Missing ;";

        // Return failure
        return false;
    }

    // Store the new column index
    *new_column_index = new_token_index;

    // Return success
    return true;
}

/**
 * Extracts the next token that is expected to be a command parameter from the line.
 * @param line The line with the instruction(s) to decode.
 * @param start_column_index The index within the column where to start parsing.
 * @param new_column_index A pointer to an integer that receives the new position of the parser in the current line.
 * @param token A pointer to a string that receives the token data.
 * @param token_size The size of the buffer for the token data in characters.
 * @param line_number The one-based line number (for error output).
 * @return true on success, false otherwise.
 */
bool TScriptInterpreter::ExtractParameter(const char* line, int32_t start_column_index, int32_t* new_column_index, char* token, int32_t token_size, int32_t line_number)
{
    int32_t token_end = 0;
    int32_t token_length = 0;

    // Get the length of the instruction string (that may contain more than 1 instruction)
    const auto length = static_cast<int32_t>(strlen(line));

    // Find the start of the the token (skip leading whitespaces)
    int32_t token_start = 0;
    for (int32_t i = start_column_index; i < length; i++)
    {
        // If there character is a whitespace, count it and continue
        if ((line[i] == ' ') || (line[i] == '\t'))
        {
            token_start++;
            continue;
        }

        // Character is not a whitespace, end here
        break;
    }

    // Advance to the actual token start (the first character of the token)
    token_start += start_column_index;

    // Check, if the token is string, delimited by " or '
    if ((line[token_start] == '"') || (line[token_start] == '\''))
    {
        // The token is a string, find the ending delimiter
        token_end = this->FindToken(line, token_start + 1, line[token_start], true);

        // If the string is not ended, this is an error
        if (token_end < 0)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "String not terminated";

            // Return failure
            return false;
        }

        // The string is terminated, calculate the string length
        token_length = token_end - token_start + 1;

        // Check if the string is too long
        if (token_length >= token_size)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "String constant too long";

            // Return failure
            return false;
        }

        // Return the string token
        strncpy_s(token, static_cast<std::size_t>(token_size), &line[token_start], static_cast<std::size_t>(token_length));

        // The new column index for the parser is behind the closing string delimiter
        *new_column_index = token_end + 1;
    }
    else
    {
        // The token is not a string, find the next whitespace, comma or closing bracket.
        token_end = token_start;
        for (int32_t i = token_start; i < length; i++)
        {
            // If there character is a whitespace, comma or closing bracket this marks the end of the token.
            if ((line[i] == ' ') || (line[i] == '\t') || (line[i] == ',') || (line[i] == ')')) break;

            // Count the character
            token_end++;
        }

        // Calculate the token length
        token_length = token_end - token_start;

        // Check if the string is too long
        if (token_length >= token_size)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Parameter too long";

            // Return failure
            return false;
        }

        // Return the string token
        strncpy_s(token, static_cast<std::size_t>(token_size), &line[token_start], static_cast<std::size_t>(token_length));

        // The new column index for the parser is directly on the next character after the token
        *new_column_index = token_end;
    }

    // Return success
    return true;
}

/**
 * This function processes a previously extracted parameter token (which can be a string literal, a variable name, a number, etc.) and
 * stores the parameter value in the appropriate data variable of the instruction.
 * @param token The parameter token to process.
 * @param param_index The parameter index (0 - n) in the function call. Used when creating error messages.
 * @param var_pointer A pointer to the variable that receives the parameter data.
 * @param var_name A pointer to a string that receives variable names, if used.
 * @param type The type of parameter that is expected.
 * @param accept_variable true if a variable name is accepted as this parameter, false otherwise.
 * @param line_number The one-based line number (for error output).
 * @return true on success, false otherwise.
 */
bool TScriptInterpreter::ProcessParameter(const char* token, int32_t param_index, void* var_pointer, TString* var_name, TScriptParameterType type, bool accept_variable, int32_t line_number)
{
    // If the specified parameter may be a variable, check if it IS one,
    // in this case remember the name, if not: read the immediate value
    if (accept_variable == true)
    {
        // Ensure a target buffer for the variable name is present
        if (var_name == nullptr)
        {
            // Create the error message
            this->error_line_ = line_number;
            this->error_string_ = "Internal error (0x00001: No variable buffer specified)";

            // Return failure
            return false;
        }

        // Ensure the specified parameter may be a valid variable name
        if (this->IsValidVariableName(token) == true)
        {
            // Check if a variable with the specified name already exists
            if (this->VariableExists(token))
            {
                // The variable seems valid, store the variable name
                (*var_name) = token;

                // Return success
                return true;
            }
            else
            {
                // The specified variable name is not known, create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Unknown variable <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }
        }

        // The parameter is not a variable, evaluate it
    }

    switch (type)
    {
        case TScriptParameterType::SHORT:
        case TScriptParameterType::LONG:
        case TScriptParameterType::BYTE:
        case TScriptParameterType::WORD:
        case TScriptParameterType::DWORD:
        case TScriptParameterType::QWORD:
        {
            // Extract the number
            if (this->ExtractNumber(token, type, var_pointer) == false)
            {
                // The number is too big to be read as the specified datatype (overflow), create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Invalid number <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Return success
            return true;
        }
        case TScriptParameterType::STRING:
        {
            // Ensure the parameter is a string constant
            if ((token[0] != '"') && (token[0] != '\''))
            {
                // The string does not start with a delimiter, create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Invalid string constant <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Get the length of the string including the delimiters
            const auto token_length = static_cast<int32_t>(strlen(token));

            // Ensure the string is terminated (by the same character used to start the token, like quote etc.)
            if (token[token_length - 1] != token[0])
            {
                // The string does not end with a delimiter, create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Unterminated string constant <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Check line length
            if ((token_length - 2) >= HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH)
            {
                // The string is too long create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Too long string constant <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Copy the string data (and remove trailing string delimiter)
            (*static_cast<TString*>(var_pointer)) = &token[1];
            (*static_cast<TString*>(var_pointer)).RemoveRight(1);

            // Return success
            return true;
        }
        case TScriptParameterType::BOOL:
        {
            // Evaluate the boolean value
            // Check for "true"
            if (strcmp(token, "true") == 0)
            {
                // Store the boolean value
                (*(static_cast<bool*>(var_pointer))) = true;

                // Return success
                return true;
            }
            // Check for "false"
            if (strcmp(token, "false") == 0)
            {
                // Store the boolean value
                (*(static_cast<bool*>(var_pointer))) = false;

                // Return success
                return true;
            }

            // The boolean value is invalid (neither true nor false), create an error
            this->error_line_ = line_number;
            this->error_string_.New(128);
            snprintf(this->error_string_, this->error_string_.Size(), "Invalid boolean value <%s> passed as parameter %i", token, param_index + 1);

            // Return failure
            return false;
        }
        case TScriptParameterType::VARNAME:
        {
            // Parameter is variable name (used with declare() and loadb/w/d())
            // Check length restrictions
            if (strlen(token) > HE_SCRIPT_MAX_VARIABLE_NAME_LENGTH)
            {
                // The specified variable name is too long, create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Too long variable name <%.64s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Check for valid variable name
            if (this->IsValidVariableName(token) == false)
            {
                // The specified variable name is not known, create an error
                this->error_line_ = line_number;
                this->error_string_.New(128);
                snprintf(this->error_string_, this->error_string_.Size(), "Invalid variable name <%s> passed as parameter %i", token, param_index + 1);

                // Return failure
                return false;
            }

            // Copy the variable name
            (*static_cast<TString*>(var_pointer)) = token;

            // Return success
            return true;
        }
    }

    // The datatype is unknown (not handled), create an error
    this->error_line_ = line_number;
    this->error_string_ = "Internal error (0x00002: Unhandled datatype)";

    // Return failure
    return false;
}

/**
 * Extracts the number from the specified string with valid type conversion
 * according to the specified type into the specified variable.
 * @param number_string The string containing the number.
 * @param type The type of value to extract.
 * @param variable The target variable for the data.
 * @return true on success, false otherwise.
 */
bool TScriptInterpreter::ExtractNumber(const char* number_string, TScriptParameterType type, void* variable)
{
    bool valid = false;
    uint64_t value = 0;

    // Check for integer that is specified hex-decimal "0xYYYYY"
    if (_strnicmp(number_string, "0x", 2) == 0)
    {
        // Create string
        TString hex_string = TString(&number_string[2]);

        // Validity checks
        // Short: 4 Byte 0xXXXX
        if ((type == TScriptParameterType::SHORT) && (hex_string.Length() > 4)) return false;
        // Long: 8 Byte 0xXXXXXXXX
        if ((type == TScriptParameterType::LONG) && (hex_string.Length() > 8)) return false;
        // Byte: 2 Byte 0xXX
        if ((type == TScriptParameterType::BYTE) && (hex_string.Length() > 2)) return false;
        // Word: 4 Byte 0xXXXX
        if ((type == TScriptParameterType::WORD) && (hex_string.Length() > 4)) return false;
        // DWord: 8 Byte 0xXXXXXXXX
        if ((type == TScriptParameterType::DWORD) && (hex_string.Length() > 8)) return false;
        // QWord: 16 Byte 0xXXXXXXXXXXXXXXXX
        if ((type == TScriptParameterType::QWORD) && (hex_string.Length() > 16)) return false;

        // Parse hex number
        value = static_cast<uint64_t>(hex_string.ParseHex(&valid));
    }
    else  // Integer is specified decimal "+/-YYYYY"
    {
        // Create string
        TString dec_string = TString(number_string);

        // Parse decimal number
        value = static_cast<uint64_t>(dec_string.ParseDec(&valid));

        // Validity checks
        // Short: -32768<=x<=32768, but casted to unsigned
        if ((type == TScriptParameterType::SHORT) && (value > 65535)) return false;
        // Long: -2147483648<=x<=2147483647, but casted to unsigned
        if ((type == TScriptParameterType::LONG) && (value > 4294967295UL)) return false;
        // Byte: x<=255
        if ((type == TScriptParameterType::BYTE) && (value > 255)) return false;
        // Word: x<=65535
        if ((type == TScriptParameterType::WORD) && (value > 65535)) return false;
        // Double Word: x<=4294967295
        if ((type == TScriptParameterType::DWORD) && (value > 4294967295UL)) return false;
    }

    // Validate parsing
    if (valid == false) return false;

    // Now that the value is validated, assign it to the variable
    if (type == TScriptParameterType::SHORT) (*(static_cast<int16_t*>(variable))) = static_cast<int16_t>(value);
    if (type == TScriptParameterType::LONG)  (*(static_cast<int32_t*>(variable))) = static_cast<int32_t>(value);
    if (type == TScriptParameterType::BYTE)  (*(static_cast<uint8_t*>(variable))) = static_cast<uint8_t>(value);
    if (type == TScriptParameterType::WORD)  (*(static_cast<uint16_t*>(variable))) = static_cast<uint16_t>(value);
    if (type == TScriptParameterType::DWORD) (*(static_cast<uint32_t*>(variable))) = static_cast<uint32_t>(value);
    if (type == TScriptParameterType::QWORD) (*(static_cast<uint64_t*>(variable))) = static_cast<uint64_t>(value);

    // Report success
    return true;
}

/**
 * Ensures that the specified token is a valid variable name.
 * Variable names must start with a character between a-z or A-Z or an underscore.
 * They may not contain spaces. No reserved words may be used.
 * @param name The variable name to check.
 * @return true, if the variable name is valid, false otherwise.
 */
bool TScriptInterpreter::IsValidVariableName(const char* name) noexcept
{
    bool valid = false;

    // Get the length of the string
    const auto length = static_cast<int32_t>(strlen(name));

    // Process the string
    for (int32_t i = 0; i < length; i++)
    {
        // Check for underscore, always valid
        if (name[i] == '_') continue;

        // Check for number (0-9), only valid if not first character
        valid = false;
        for (int32_t c = 48; c < 58; c++)
        {
            if ((name[i] == static_cast<char>(c)) && (i > 0)) valid = true;
        }
        if (valid) continue;

        // Check if the first character is an uppercase letter (A - Z), always valid
        valid = false;
        for (int32_t c = 65; c <= 90; c++)
        {
            if (name[i] == static_cast<char>(c)) valid = true;
        }
        if (valid) continue;

        // Check if the first character is a lowercase letter (a - z), always valid
        valid = false;
        for (int32_t c = 97; c <= 122; c++)
        {
            if (name[i] == static_cast<char>(c)) valid = true;
        }
        if (valid) continue;

        // None of the accepted characters, return failure
        return false;
    }

    // Check for reserved words = commands
    if (this->script_command_manager_.GetCommandByName(name) != nullptr) return false;

    // Return success
    return true;
}

/**
 * Checks all known script variables, if the specified variable exists.
 * @param name The name of the variable to find.
 * @return true if the variable exists, false otherwise.
 */
bool TScriptInterpreter::VariableExists(const char* name)
{
    // Validate name
    if (name == nullptr) return false;

    // Check all variables for the variable with the specified name (case sensitive!)
    for (const auto& variable : this->script_variable_)
    {
        // If the name is found, return the variable
        if (variable.name_.Equals(name) == true) return true; // cppcheck-suppress useStlAlgorithm
    }

    // Return failure
    return false;
}

/**
 * Returns a pointer to the specified variable.
 * @param name The name of the variable to return.
 * @return A pointer to the user variable.
 */
TScriptVariable* TScriptInterpreter::GetVariable(const char* name) noexcept
{
    // If no name is specified, return a nullptr
    if (name == nullptr) return nullptr;
    if (strlen(name) == 0) return nullptr;

    // Process all variables
    for (auto& variable : this->script_variable_)
    {
        if (variable.name_.Equals(name) == true) return &variable;
    }

    // No variable found
    return nullptr;
}

