// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_UNDO_ENGINE_HPP_

    // Header included
    #define HEDIT_SRC_UNDO_ENGINE_HPP_

    /**
     * @brief The structure that holds all data of a change operation.
     * @details This is the file position and the file data before the change occured.
     */
    struct TUndoStep
    {
        int64_t file_pos;       //!< The current file position when the change operation occured.
        int32_t cursor_pos;     //!< The current cursor position (relative to the file position) when the change operation occured.
        int32_t nibble_pos;     //!< The current nibble position (relative to the cursor position) when the change operation occured.
        unsigned char value;    //!< The original value of the character at the changed position before the change operation occured.
    };

    /**
     * @brief The class that manages the undoing of changes.
     */
    class TUndoEngine
    {
    private:
        uint32_t steps_;                            //!< The maximum number of undo steps.
        uint32_t active_step_;                      //!< The current undo step.
        std::unique_ptr<TUndoStep[]> undo_buffer_;  //!< The buffer for the undo data.
    public:
        explicit TUndoEngine(int32_t steps);
        bool Backup(int64_t file_pos, int32_t cursor_pos, int32_t nibble_pos, unsigned char old_char) noexcept;
        bool Restore(TUndoStep* undo_step) noexcept;
    };

#endif  // HEDIT_SRC_UNDO_ENGINE_HPP_
