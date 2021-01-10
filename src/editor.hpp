// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_EDITOR_HPP_

    // Header included
    #define HEDIT_SRC_EDITOR_HPP_

    // String lengths
    constexpr int32_t HE_EDITOR_MAX_SEARCH_STRING_LENGTH = 64;  //!< The string length of the search string that is stored per editor.

    // View modes
    enum class TViewMode : int32_t {
        HEXDEC,         //!< View mode: Hexdecimal file view
        TEXT,           //!< View mode: Text file view
        DISASSEMBLER,   //!< View mode: Disassembler
        SCRIPT          //!< View mode: Script interpreter
    };

    /**
     * @brief The class for a single editor.
     * @details Each editor window can be populated by different viewers.
     */
    class TEditor
    {
    private:
        int64_t file_pos_;              //!< The current position within the file.
        TFile* file_;                   //!< The file object of the file to edit.
        TMarker* marker_;               //!< The marker for the currently marked area in the editor.
        TString file_name_;             //!< The name of the file in this editor.
        TConsole* console_;             //!< The console for any screen output.
        TSettings* settings_;           //!< The editor settings object.
        TAsmViewer* asm_viewer_;        //!< The disassembler viewer.
        THexViewer* hex_viewer_;        //!< The hex viewer object.
        TEditorInfo editor_info_;       //!< The information about the editor.
        TComparator* comparator_;       //!< The comparator engine used to compare data between editors.
        TBaseViewer* active_viewer_;    //!< The active viewer object.
        TUndoEngine* undo_engine_;      //!< The undo engine.
        TTextViewer* text_viewer_;      //!< The text viewer.
        TScriptViewer* script_viewer_;  //!< The script viewer.
    public:
        bool file_opened_;                                                  //!< Flag: true, if the file is open, false otherwise.
        TViewMode view_mode_;                                               //!< The view mode. One of the TViewMode constants.
        std::size_t search_string_length_;                                  //!< The length of the current search string.
        unsigned char search_string_[HE_EDITOR_MAX_SEARCH_STRING_LENGTH];   //!< The current search string.
    public:
        TEditor(TConsole* console, TEditorInfo* editor_info, const char* file_name, TSettings* settings, TComparator* comparator);
        TEditor(const TEditor& source) = delete;
        TEditor& operator=(const TEditor& source) = delete;
        TEditor(TEditor&&) = delete;
        TEditor& operator=(TEditor&&) = delete;
        ~TEditor();
        void DrawComplete(bool active);
        void DrawFileName(bool active);
        void DrawFileContent();
        void UpdateCursor();
        void EditorFunction1();
        void EditorFunction2();
        bool ReadBytesAtOffset(int64_t offset, unsigned char* buffer, uint32_t count) noexcept;
        int64_t GetFileSize() noexcept;
        int64_t GetFileOffset() const noexcept;
        void CursorUp();
        void CursorDown();
        void CursorLeft();
        void CursorRight();
        void PageUp();
        void PageDown();
        void First();
        void Last();
        bool InsertCharacter(unsigned char character);
        static bool Exists(const char* file_name);
        bool WriteActiveSelectionToFile(const char* file_name, bool overwrite);
        bool InsertFile(int64_t position, const char* file_name);
        bool InsertSpace(int64_t position, int32_t length);
        bool FillSelection(const unsigned char* fill_string, int32_t fill_string_len) noexcept;
        void DrawPercentBar(int32_t percent) noexcept;
        void Undo();
        void StartSelection();
        void EndSelection();
        int64_t CurrentAbsPos();
        int32_t CurrentRelPos();
        void SetCurrentAbsPos(int64_t offset);
        void SetCurrentRelPos(int32_t offset);
        void UpdateStatus();
        void SetViewMode(TViewMode view_mode, TString script_name = TString(""));
        void SetChanged() noexcept;
        bool IsChanged() const noexcept;
        TMarker* GetMarker() noexcept;
    };

#endif  // HEDIT_SRC_EDITOR_HPP_
