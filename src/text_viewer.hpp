// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_TEXT_VIEWER_HPP_

    // Header included
    #define HEDIT_SRC_TEXT_VIEWER_HPP_

    /**
     * @brief The text viewer (that can be used by an editor window).
     */
    class TTextViewer final : public TBaseViewer
    {
    private:
        int32_t cursor_pos_;    //!< The cursor position relative to the current file position.
        int32_t text_width_;    //!< The maximum length of a line of text.
        int32_t text_column_;   //!< The horizontal cursor position of the current line data.
    public:
        explicit TTextViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept;
        TTextViewer(const TTextViewer& source) = delete;
        TTextViewer& operator=(const TTextViewer& source) = delete;
        // The virtual functions, every editor must implement
        void Init(TString &string) noexcept override;
        void Draw() override;
        void UpdateCursor() noexcept override;
        void UpdateStatus() override;
        void CursorUp() noexcept override;
        void CursorDown() noexcept override;
        void CursorLeft() noexcept override;
        void CursorRight() noexcept override;
        void PageUp() noexcept override;
        void PageDown() noexcept override;
        void First() noexcept override;
        void Last() noexcept override;
        bool InsertCharacter(unsigned char character) noexcept override;
        void Undo() noexcept override;
        void StartSelection() noexcept override;
        void EndSelection() noexcept override;
        int64_t CurrentAbsPos() noexcept override;
        int32_t CurrentRelPos() noexcept override;
        void SetCurrentAbsPos(int64_t offset) noexcept override;
        void SetCurrentRelPos(int32_t offset) noexcept override;
        void EditorFunction1() noexcept override;
        void EditorFunction2() override;
    };

#endif  // HEDIT_SRC_TEXT_VIEWER_HPP_
