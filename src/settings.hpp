// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_SETTINGS_HPP_

    // Header included
    #define HEDIT_SRC_SETTINGS_HPP_

    // The name of the config file (within the user's home directory).
    constexpr const char* HE_CONFIG_FILE = ".hedit";

    /**
     * @brief The class that manages all editor settings.
     */
    class TSettings
    {
    private:
        TString config_dir_;                //!< The directory to store the config file (usually the user's home directory).
        TString config_file_;               //!< The full path and name of the config file (directory + file name).
    public:
        // Settings
        bool use_caching_;                  //!< The flag that specifies if file caching is used.
        bool temp_file_persistent_;         //!< The flag that specifies if the temporary file is persistent.
        int32_t undo_steps_;                //!< The maximum number of changes that can be undone (0 - 200).
        int32_t probable_word_length_;      //!< The minimum length of joined characters that are needed to make up a word.
        TString temp_file_name_;            //!< The file name of the temporary file that is used for copy/paste operations.
        TString probable_word_char_set_;    //!< The collection of characters that is used to detect if a character belongs to a word.
        TNumberFormat number_format_;       //!< The number format for number output in the disassembler.
        // Colors
        TColor text_color_;                 //!< The color for normal text.
        TColor text_back_color_;            //!< The background color for normal text.
        TColor inactive_text_color_;        //!< The color for inactive texts.
        TColor active_file_color_;          //!< The color for the active file name.
        TColor active_file_back_color_;     //!< The background color for the active file name.
        TColor marked_text_color_;          //!< The color for marked text.
        TColor marked_text_back_color_;     //!< The background color for marked text.
        TColor dialog_color_;               //!< The color for text in dialogs.
        TColor dialog_back_color_;          //!< The background color for text in dialogs.
        TColor difference_color_[5];        //!< The array of colors to use for colored differences.
        TColor difference_back_color_[5];   //!< The array of background colors to use for colored differences.
        // Plugins
        TString plugin_path_;               //!< The path to look for plugins (the same directory as for the config file).
        TString plugin_file_;               //!< The default plugin to start by hotkey.
        // Internal settings
        bool marker_mode_;                  //!< The flag that specifies if the "marker mode" is active, i.e. moving the cursor modifies the start or end of the selection.
        bool compare_mode_;                 //!< The flag that specifies if the "compare mode" is active, i.e. all differences are painted in different colors.
        bool cursor_lock_mode_;             //!< The flag that specifies if the "cursor lock mode" is active, i.e. moving the cursor in one file moves the cursor in all files.
    private:
        static TColor GetColorId(const char* color_name) noexcept;
        static const char* GetColorName(TColor color_id) noexcept;
        void LoadDefaultSettings();
        void GetColor(TString& line, const char* color_name, TColor* color_value) noexcept;
        void WriteNewline(TConfigFile* file) noexcept;
        void WriteConfigLine(TConfigFile* file, const char* format, ...) noexcept;
    public:
        explicit TSettings(const char* config_file);
        TSettings(const TSettings&) = delete;
        TSettings& operator=(const TSettings&) = delete;
        TSettings(TSettings&&) = delete;
        TSettings& operator=(TSettings&&) = delete;
        ~TSettings() noexcept(false);
        bool LoadSettings();
        bool SaveSettings();
        void ToggleCursorLockMode() noexcept;
        void ToggleCompareMode() noexcept;
        void ToggleMarkerMode() noexcept;
    };

#endif  // HEDIT_SRC_SETTINGS_HPP_
