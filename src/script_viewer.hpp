// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_SCRIPT_VIEWER_HPP_

    // Header included
    #define HEDIT_SRC_SCRIPT_VIEWER_HPP_

    // Virtual Script Screen Size
    constexpr int32_t HE_SCRIPT_MAX_SCREEN_COLS = 200;  //!< The maximum number of columns on the virtual screen.
    constexpr int32_t HE_SCRIPT_MAX_SCREEN_ROWS = 90;   //!< The maximum number of rows on the virtual screen.

    /**
     * @brief The script viewer (that can be used by an editor window).
     */
    class TScriptViewer final : public TBaseViewer
    {
    private:
        int32_t total_lines_;       //!< The number of lines in the screen buffer.
        int32_t current_line_;      //!< The (current) cursor screen line on the virtual screen.
        TString* screen_buffer_;    //!< The virtual screen buffer for the script output.
        std::unique_ptr<TScriptInterpreter> script_interpreter_;    //!< The script interpreter.
    public:
        explicit TScriptViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator);
        TScriptViewer(const TScriptViewer& source) = delete;
        TScriptViewer& operator=(const TScriptViewer& source) = delete;
        TScriptViewer(TScriptViewer&&) = delete;
        TScriptViewer& operator=(TScriptViewer&&) = delete;
        ~TScriptViewer() CPPBUILDER_OVERRIDE;
        // The virtual functions, every editor must implement
        void Init(TString &string) override;
        void Draw() override;
        void UpdateCursor() noexcept override {}
        void UpdateStatus() noexcept override;
        void CursorUp() noexcept override;
        void CursorDown() noexcept override;
        void CursorLeft() noexcept override {}
        void CursorRight() noexcept override {}
        void PageUp() noexcept override {}
        void PageDown() noexcept override {}
        void First() noexcept override;
        void Last() noexcept override;
        bool InsertCharacter(unsigned char character) noexcept override;
        void Undo() noexcept override {}
        void StartSelection() noexcept override {}
        void EndSelection() noexcept override {}
        int64_t CurrentAbsPos() noexcept override;
        int32_t CurrentRelPos() noexcept override;
        void SetCurrentAbsPos(int64_t offset) noexcept override;
        void SetCurrentRelPos(int32_t offset) noexcept override;
        void EditorFunction1() override;
        void EditorFunction2() noexcept override {}
    };

#endif  // HEDIT_SRC_SCRIPT_VIEWER_HPP_
