// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_SCRIPT_INTERPRETER_HPP_

    // Header included
    #define HEDIT_SRC_SCRIPT_INTERPRETER_HPP_

    // The string length of a parser token (in characters).
    // A parser token can be a variable name, command name, string, user variable or number.
    constexpr int32_t HE_SCRIPT_MAX_PARSER_TOKEN_LENGTH     = 128;

    // The error codes for the FindToken function
    constexpr int32_t HE_SCRIPT_TOKEN_ERROR_EOL             = -1;    //!< FindToken return value: End-of-Line reached.
    constexpr int32_t HE_SCRIPT_TOKEN_ERROR_UNKNOWN_TOKEN   = -2;    //!< FindToken return value: Encountered an unknown token.
    constexpr int32_t HE_SCRIPT_TOKEN_ERROR_INVALID_STRING  = -3;    //!< FindToken return value: String variable invalid.

    /**
     * @brief The script language interpreter.
     */
    class TScriptInterpreter
    {
    private:
        bool script_loaded_;                            //!< The flag that specifies if a script was loaded successfully.
        int32_t error_line_;                            //!< The (one-based) line number of the last script error that occured. A value of zero means: line unknown.
        TString script_file_;                           //!< The file name of the script file (used for error output).
        TString script_path_;                           //!< The path to load script files from, if no full path is specified.
        TString error_string_;                          //!< The string variable for the error description.
        TString script_description_;                    //!< The string variable for the script description (taken from the main function).
        std::vector<TScriptVariable>script_variable_;   //!< The storage for the variables in the script.
        std::vector<TScriptInstruction> script_code_;   //!< The storage for the decoded script instructions.
        TScriptCommandManager script_command_manager_;  //!< The script instruction manager.
    private:
        int32_t OutputErrorMessage(TString* virtual_screen_buffer);
        bool LoadScriptFile(const TString& file_name, TStringList& script_code);
        static int32_t FindToken(const char* string, int32_t start_index, const char token, bool ignore_unknown_tokens) noexcept;
        bool FindFunction(const char* function_name, const TStringList& script_code, int32_t* start_line, int32_t* start_column);
        bool GetFunctionCode(TStringList& script_code, int32_t start_line, int32_t start_column);
        static bool ExtractToken(const char* source_string, char* target_string, std::size_t target_size, int32_t* full_token_length) noexcept;
        void RemoveComment(char* line) noexcept;
        static bool CharIsInText(const char* string, std::size_t start_index) noexcept;
        bool DecodeInstruction(const TString& line, int32_t start_column_index, TScriptInstruction* si, int32_t* new_column_index, int32_t line_number);
        bool ExtractParameter(const char* line, int32_t start_column_index, int32_t* new_column_index, char* token, int32_t token_size, int32_t line_number);
        bool ProcessParameter(const char* token, int32_t param_index, void* var_pointer, TString* var_name, TScriptParameterType type, bool accept_variable, int32_t line_number);
        static bool ExtractNumber(const char* number_string, TScriptParameterType type, void* variable);
        bool IsValidVariableName(const char* name) noexcept;
        bool VariableExists(const char* name);
        TScriptVariable* GetVariable(const char* name) noexcept;
    public:
        explicit TScriptInterpreter(const TString& script_path);
        bool LoadScript(const TString& file_name);
        const char* ScriptDescription() const noexcept;
        int32_t ScriptExecuteAt(TFile* file, int64_t position, TString* virtual_screen_buffer);
        const char* GetLastError() const noexcept;
    };

#endif  // HEDIT_SRC_SCRIPT_INTERPRETER_HPP_
