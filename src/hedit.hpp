// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_HEDIT_HPP_

    // Header included
    #define HEDIT_SRC_HEDIT_HPP_

    // Version and copyright
    constexpr const char* HE_PROGRAM_TITLE      = "HEdit v4.2.4";                       //!< The program title including version number.
    constexpr const char* HE_PROGRAM_COPYRIGHT  = "Copyright (c) 2021 Roxxorfreak";     //!< The copyright notice (for the about box).
    constexpr const char* HE_PROGRAM_BUILD_DATE = __DATE__;                             //!< The build date of the application.

    // Modes for the "Goto Address" function
    enum class TPositioningMode : int32_t {
        PM_RELATIVE,    //!< The positioning mode for "Goto Address": Relative to current file pointer
        PM_ABSOLUTE     //!< The positioning mode for "Goto Address": Absolute (relative to file start)
    };

    // Search directions
    enum class TSearchDirection : int32_t {
        FORWARD,    //!< The direction for the search operations: Forward
        BACKWARD    //!< The direction for the search operations: Backward
    };

    // Search modes
    enum class TSearchMode : int32_t {
        NONE,              //!< Search mode: None (no active search)
        TEXT_CS,           //!< Search mode: ASCII Text (case sensitive)
        TEXT_CI,           //!< Search mode: ASCII Text (case insensitive)
        UNICODE_TEXT,      //!< Search mode: Unicode (UCS-2) Text (case sensitive)
        HEX_STRING,        //!< Search mode: String specified as hex characters.
        HEX_RANGE,         //!< Search mode: A single hex character in the specified range.
        HEX_COMPARING,     //!< Search mode: Comparing Hex: A hex character or string with the specified value at the same location in all open files.
        ANY_DIFFERENCE,    //!< Search mode: Any difference between the open files.
        PROBABLE_WORD,     //!< Search mode: A word (of specified length) that contains only the characters specified via config file.
        COUNT,             //!< Search mode: Counts the number of ocurrences of the search character or string.
    };

    /**
     * @brief The HEdit application class.
     * This class provides the HEdit application and the Edit() function that is used to create the editor(s).
     */
    class THEdit
    {
    private:
        int32_t files_ = {};                                //!< The number of files that are to be edited, including files that cannot be opened.
        int32_t window_height_ = {};                        //!< The height of the HEdit main window (used to draw the Help window).
        TString formula_string_;                            //!< The formula string.
        TFormula formula_;                                  //!< The formula evaluation class.
        TConsole* console_;                                 //!< The console object. It is used by all classes to output data.
        std::unique_ptr<TSettings> settings_;               //!< The program settings.
        TEditor* editor_[HE_MAX_EDITORS] = { nullptr };     //!< The editors.
        TSearchMode search_mode_;                           //!< The current search mode.
        TComparator comparator_;                            //!< The comparator engine.
    private:
        void MainLoop();
        void MessageBox(const char* title, const char* text1, const char* text2 = "");
        void About();
        void Help(TViewMode view_mode);
        bool GotoAddress(int32_t active_editor, TPositioningMode positioning_mode);
        bool SwitchViewer(int32_t active_editor);
        void Calculator();
        bool FileMenu(int32_t active_editor);
        bool FindMenu(int32_t active_editor, TSearchDirection search_direction);
        static std::size_t ConvertHexString(unsigned char* target, std::size_t size, const char* source);
        bool Search(TSearchMode search_mode, TSearchDirection search_direction, int32_t active_editor);
        bool CheckCondition(int32_t active_editor, TSearchMode search_mode, int64_t position, std::size_t search_string_length, int32_t& search_counter) noexcept;
        static bool strequal(const unsigned char* string1, const unsigned char* string2, std::size_t length) noexcept;
    public:
        THEdit();
        THEdit(const THEdit& source) = delete;
        THEdit& operator=(const THEdit& source) = delete;
        THEdit(THEdit&&) = delete;
        THEdit& operator=(THEdit&&) = delete;
        ~THEdit();
        void Edit(int32_t file_count, char* file[]);
    };

#endif  // HEDIT_SRC_HEDIT_HPP_
