// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_SCRIPT_COMMAND_MANAGER_HPP_

    // Header included
    #define HEDIT_SRC_SCRIPT_COMMAND_MANAGER_HPP_

    // Maximum constants
    constexpr int32_t HE_SCRIPT_MAX_CASCADED_LOOP_DEPTH = 4;    //!< The maximum number of cascaded loops in the script.
    constexpr int32_t HE_SCRIPT_MAX_VARIABLE_NAME_LENGTH = 32;  //!< The maximum length for a variable name in a script.

    // String lengths
    constexpr int32_t HE_SCRIPT_MAX_TEXT_TOKEN_LENGTH = 80;     //!< The string length of a user defined text (a string literal) in the script (in characters).
    constexpr int32_t HE_SCRIPT_MAX_OUTPUT_STRING_LENGTH = 40;  //!< The string length of the output buffer for a decimal or hex string (via printds/prinths).

    // The target variable for the specified parameter in the call to a script function.
    // This specifies the internal name of the variable this parameter is stored into.
    enum class TScriptParameter : int32_t {
        NONE,       //!< Parameter: None (The parameter is not used).
        X,          //!< Parameter: X coordinate.
        Y,          //!< Parameter: Y coordinate.
        OFFSET,     //!< Parameter: Offset.
        BYTE,       //!< Parameter: Byte.
        WORD,       //!< Parameter: Word.
        DWORD,      //!< Parameter: DWord.
        QWORD,      //!< Parameter: QWord.
        TEXT1,      //!< Parameter: Text 1.
        TEXT2,      //!< Parameter: Text 2.
        VARIABLE1,  //!< Parameter: Variable 1.
        VARIABLE2,  //!< Parameter: Variable 2.
        FLAG        //!< Parameter: Flag.
    };

    // Variable types for the ExtractParameter function
    // This specifies the virtual type of the variable this parameter is stored into.
    enum class TScriptParameterType : int32_t {
        SHORT,   //!< Variable type: Short integer.
        LONG,    //!< Variable type: Long interger.
        STRING,  //!< Variable type: String.
        BYTE,    //!< Variable type: Byte.
        WORD,    //!< Variable type: Word.
        DWORD,   //!< Variable type: Double word.
        QWORD,   //!< Variable type: Quad word.
        BOOL,    //!< Variable type: Boolean.
        VARNAME  //!< Variable type: Variable name.
    };

    // Avoid circular reference
    struct TExecutionContext;

    /**
     * @brief The structure for a script language command.
     */
    struct TScriptCommand
    {
        TString name;                                        //!< The name of the script instruction (command).
        int32_t parameter_count = {};                        //!< The number of parameters of the script instruction.
        TScriptParameter variable[5] = {};                   //!< The array of target variables for the script parameters.
        bool (*execution_proc)(TExecutionContext* ec) = {};  //!< A pointer to the execution procedure for the script instruction.
    };

    /**
     * @brief The stack that is used for the instruction number used by the loops in a script.
     */
    struct TScriptLabelStack
    {
        int32_t used_labels = {};                                       //!< The number of currently used jump labels.
        int32_t label_number[HE_SCRIPT_MAX_CASCADED_LOOP_DEPTH] = {};   //!< The array with the jump label instruction numbers.
    };

    /**
     * @brief The structure that contains a decoded script instruction.
     */
    struct TScriptInstruction
    {
        bool flag = {};                 //!< A boolean flag.
        int32_t line_number = {};       //!< The original one-based line number (in the script file) the instruction was in.
        int32_t x_val = {};             //!< Cursor X pos, specified directly, no variable accepted.
        int32_t y_val = {};             //!< Cursor Y pos, specified directly, no variable accepted.
        int32_t offset = {};            //!< Offset, specified directly, variable accepted.
        uint8_t byte_val = {};          //!< Optional Byte value, specified directly, variable accepted.
        uint16_t word_val = {};         //!< Optional Word value, specified directly, variable accepted.
        uint32_t dword_val = {};        //!< Optional Double Word  value, specified directly, variable accepted.
        uint64_t qword_val = {};        //!< Optional Quad Word  value, specified directly, variable accepted.
        TString x_var;                  //!< Name of the variable containing the cursor X pos.
        TString y_var;                  //!< Name of the variable containing the cursor Y pos.
        TString byte_var;               //!< Name of the variable containing the Byte.
        TString word_var;               //!< Name of the variable containing the Word.
        TString dword_var;              //!< Name of the variable containing the DWord.
        TString qword_var;              //!< Name of the variable containing the QWord.
        TString offset_var;             //!< Name of the variable containing the offset.
        TString text[2];                //!< Two texts, specified directly, no variable accepted.
        TString variable[2];            //!< Two names of variables.
        TScriptCommand* command = {};   //!< The pointer to the script command that belongs to the decoded instruction.
    };

    /**
     * @brief The structure with all data passed to a script instruction execution procedure.
     * This is called "execution context".
     */
    struct TExecutionContext
    {
        int32_t cursor_x = {};                  //!< The horizontal (column) position of the text cursor.
        int32_t cursor_y = {};                  //!< The vertical (row) position of the text cursor.
        int64_t position = {};                  //!< The position (file offset) to which all offsets are relative.
        int32_t instruction_number = {};        //!< The zero-based instruction number (can be modified for jumps and loops).
        TFile* file = {};                       //!< The file to operate on.
        TString* virtual_screen_buffer = {};    //!< The virtual screen buffer.
        TScriptInstruction* si = {};            //!< The decoded instruction to execute.
        TScriptVariable* variable[2] = {};      //!< The pointer to the two variables that can be referenced in an instruction.
        TScriptLabelStack label_stack;          //!< The label stack (for loops).
    };

    /**
     * @brief The script command manager.
     * @details It manages the command data and contains the internal callback functions for the script commands.
     */
    class TScriptCommandManager
    {
    private:
        std::vector <TScriptCommand> script_command_;   //!< The vector that holds all supported script commands. These are loaded (initialized) from the script interpreter.
    private:
        void AddCommand(const char* name, bool (*execution_proc)(TExecutionContext* ec), int32_t parameter_count, TScriptParameter param1 = TScriptParameter::NONE,
            TScriptParameter param2 = TScriptParameter::NONE, TScriptParameter param3 = TScriptParameter::NONE, TScriptParameter param4 = TScriptParameter::NONE);
        static bool Output(TExecutionContext* ec, const char* text);
        static bool returnProc(TExecutionContext* ec) noexcept;
        static bool gotoxyProc(TExecutionContext* ec) noexcept;
        static bool printProc(TExecutionContext* ec);
        static bool cprintbProc(TExecutionContext* ec);
        static bool cprintwProc(TExecutionContext* ec);
        static bool cprintdProc(TExecutionContext* ec);
        static bool printubProc(TExecutionContext* ec);
        static bool printuwProc(TExecutionContext* ec);
        static bool printudProc(TExecutionContext* ec);
        static bool printhbProc(TExecutionContext* ec);
        static bool printhwProc(TExecutionContext* ec);
        static bool printhdProc(TExecutionContext* ec);
        static bool printsbProc(TExecutionContext* ec);
        static bool printswProc(TExecutionContext* ec);
        static bool printsdProc(TExecutionContext* ec);
        static bool printfProc(TExecutionContext* ec);
        static bool printdProc(TExecutionContext* ec);
        static bool printdsProc(TExecutionContext* ec);
        static bool printhsProc(TExecutionContext* ec);
        static bool printxorProc(TExecutionContext* ec);
        static bool loadbProc(TExecutionContext* ec) noexcept;
        static bool loadwProc(TExecutionContext* ec) noexcept;
        static bool loaddProc(TExecutionContext* ec) noexcept;
        static bool loadqProc(TExecutionContext* ec) noexcept;
        static bool setProc(TExecutionContext* ec) noexcept;
        static bool addProc(TExecutionContext* ec) noexcept;
        static bool subProc(TExecutionContext* ec) noexcept;
        static bool incrProc(TExecutionContext* ec) noexcept;
        static bool decrProc(TExecutionContext* ec) noexcept;
        static bool loopProc(TExecutionContext* ec) noexcept;
        static bool untilProc(TExecutionContext* ec) noexcept;
        static bool printvProc(TExecutionContext* ec);
    public:
        void LoadScriptCommands();
        TScriptCommand* GetCommandByName(const char* name) noexcept;
    };

#endif  // HEDIT_SRC_SCRIPT_COMMAND_MANAGER_HPP_
