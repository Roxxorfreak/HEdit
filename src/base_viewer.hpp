// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_BASE_VIEWER_HPP_

    // Header included
    #define HEDIT_SRC_BASE_VIEWER_HPP_

    // Foreward declaration (to avoid circular reference)
    class TComparator;

    /**
     * @brief The structure that holds all data about the part of the editor that can be used by the viewer.
     * @details This is the information that is used to draw the editor window (by the viewer).
     */
    class TEditorInfo
    {
    public:
        int32_t id_;                //!< The id (index) of the editor (used for the colors of the differences).
        int32_t lines_;             //!< The number of screen lines for this editor (excluding title bar and status line).
        int32_t width_;             //!< The screen width (in characters) for this editor (including the border).
        int32_t start_line_;        //!< The vertical position of the editor (including the title bar), relative to the console screen.
        int32_t status_line_;       //!< The vertical position of the status line (same for all editors), relative to the console screen.
        int32_t status_column_;     //!< The column index of the status text.
        int32_t address_width_;     //!< The width of the address field. It depends on the window size, but is 16 characters at max (64bit address).
        int32_t address_column_;    //!< The horizontal cursor position of the line address (file offset), if used by the viewer.
    public:
        TEditorInfo() noexcept;
    };

    /**
     * @brief The base class for all viewers.
     * @details The base viewer provides a message box, number output and stores all editor data.
     */
    class TBaseViewer
    {
    private:
        bool redraw_;               //!< Flag: true if the editor should be repaint the next time the draw method is called, false otherwise.
    protected:
        int64_t* file_pos_;         //!< The current absolute position within the file.
        TFile* file_;               //!< The file object that is used to access the data to be is displayed.
        TMarker* marker_;           //!< The marker object (used to determine if a byte is selected).
        TConsole* console_;         //!< The console object used for all output.
        TSettings* settings_;       //!< The program settings.
        TComparator* comparator_;   //!< The comparator engine use to compare data between editors.
        TUndoEngine* undo_engine_;  //!< The undo engine used to track and undo changes.
        TEditorInfo* editor_;       //!< The information about the editor.
    public:
        TBaseViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept;
        TBaseViewer(const TBaseViewer&) = delete;
        TBaseViewer& operator=(const TBaseViewer&) = delete;
        TBaseViewer(TBaseViewer&&) = delete;
        TBaseViewer& operator=(TBaseViewer&&) = delete;
        virtual ~TBaseViewer();
        void SetChanged() noexcept;
        void ClearChanged() noexcept;
        bool IsChanged() const noexcept;
        void MessageBox(const char* title, const char* text1, const char* text2 = "");
        void UpdateCurrentCharStatus(int32_t offset);
        // The virtual functions, every editor must implement
        virtual void Init(TString &string) = 0;
        virtual void Draw() = 0;
        virtual void UpdateCursor() = 0;
        virtual void UpdateStatus() = 0;
        virtual void CursorUp() = 0;
        virtual void CursorDown() = 0;
        virtual void CursorLeft() = 0;
        virtual void CursorRight() = 0;
        virtual void PageUp() = 0;
        virtual void PageDown() = 0;
        virtual void First() = 0;
        virtual void Last() = 0;
        virtual bool InsertCharacter(unsigned char character) = 0;
        virtual void Undo() = 0;
        virtual void StartSelection() = 0;
        virtual void EndSelection() = 0;
        virtual int64_t CurrentAbsPos() = 0;
        virtual int32_t CurrentRelPos() = 0;
        virtual void SetCurrentAbsPos(int64_t offset) = 0;
        virtual void SetCurrentRelPos(int32_t offset) = 0;
        virtual void EditorFunction1() = 0;
        virtual void EditorFunction2() = 0;
    };

#endif  // HEDIT_SRC_BASE_VIEWER_HPP_
