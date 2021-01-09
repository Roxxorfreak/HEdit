// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_HEX_VIEWER_HPP_

    // Header included
    #define HEDIT_SRC_HEX_VIEWER_HPP_

    // Edit Modes
    enum class THexEditMode : int32_t {
        HEX,    //!< Edit mode: Edit hex code
        DEC     //!< Edit mode: Edit text
    };

    /**
     * @brief The hex viewer (that can be used by an editor window). This is the default viewer.
     */
    class THexViewer final : public TBaseViewer
    {
    private:
        int32_t hex_column_;        //!< The horizontal position of the "hex" part of the viewer.
        int32_t dec_column_;        //!< The horizontal position of the "dec" part of the viewer.
        int32_t cursor_pos_;        //!< The cursor position relative to the current file position.
        int32_t nibble_pos_;        //!< The nibble (half-byte) position relative to the cursor position.
        THexEditMode edit_mode_;    //!< The current edit mode (HEX or DEC).
    public:
        explicit THexViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept;
        THexViewer(const THexViewer& source) = delete;
        THexViewer& operator=(const THexViewer& source) = delete;
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
    private:
        bool IsHexCharacter(unsigned char character_code);
    };

#endif  // HEDIT_SRC_HEX_VIEWER_HPP_
