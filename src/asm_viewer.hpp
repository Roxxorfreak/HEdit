// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_ASM_VIEWER_HPP_

    // Header included
    #define HEDIT_SRC_ASM_VIEWER_HPP_

    /**
     * @brief The disassembler viewer (that can be used by an editor window).
     */
    class TAsmViewer final : public TBaseViewer
    {
    private:
        char** screen_buffer_;          //!< The array that represents the virtual screen buffer that is used to display the disassembled instructions.
        int32_t hex_column_;            //!< The horizontal position of the "hex" part of the viewer.
        int32_t asm_width_;             //!< The width of the "asm" part of the viewer.
        int32_t asm_column_;            //!< The horizontal position of the "asm" part of the viewer.
        int32_t current_line_;          //!< The currently selected disassembled line (zero-based). This is used to mark the current selection.
        int32_t last_current_line_;     //!< The previously selected disassembled line (zero-based). This is used to mark the current selection.
        int32_t disassembled_lines_;    //!< The number of currently disassembled lines.
        uint32_t machine_code_bytes_;   //!< The maximum number of machine code bytes to display (as hex code). This depends on the editor width.
        TAsmBuffer* buffer_;            //!< The buffer for the data to disassemble.
        TArchitecture architecture_;    //!< The current architecture for the disassembler (see TArchitecture).
        TDisassembler* disassembler_;   //!< The disassembler engine used to disassemble the code.
        TAsmInstruction* instruction_;  //!< The array of disassembled instructions, each to be displayed on a separate line.
    public:
        explicit TAsmViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator);
        TAsmViewer(const TAsmViewer& source) = delete;
        TAsmViewer& operator=(const TAsmViewer& source) = delete;
        TAsmViewer(TAsmViewer&&) = delete;
        TAsmViewer& operator=(TAsmViewer&&) = delete;
        ~TAsmViewer() CPPBUILDER_OVERRIDE;
        // The virtual functions, every editor must implement
        void Init(TString& string) noexcept override;
        void Draw() override;
        void UpdateCursor() noexcept override;
        void UpdateStatus() noexcept override;
        void CursorUp() override;
        void CursorDown() noexcept override;
        void CursorLeft() noexcept override {}
        void CursorRight() noexcept override {}
        void PageUp() noexcept override {}
        void PageDown() noexcept override;
        void First() noexcept override;
        void Last() noexcept override {}
        bool InsertCharacter(unsigned char character) noexcept override;
        void Undo() noexcept override {}
        void StartSelection() noexcept override {}
        void EndSelection() noexcept override {}
        int64_t CurrentAbsPos() noexcept override;
        int32_t CurrentRelPos() noexcept override;
        void SetCurrentAbsPos(int64_t offset) override;
        void SetCurrentRelPos(int32_t offset) noexcept override;
        void EditorFunction1() noexcept override;
        void EditorFunction2() override;
    };

#endif  // HEDIT_SRC_ASM_VIEWER_HPP_
