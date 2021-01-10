// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Initializes the undo engine.
 * @param steps The number of steps for the undo engine.
 */
TUndoEngine::TUndoEngine(int32_t steps)
    : steps_(static_cast<uint32_t>(steps)),
    active_step_(0)
{
    // Ensure maximum number of undo steps
    if (this->steps_ > 200) this->steps_ = 200;

    // Create the undo buffer
    this->undo_buffer_ = std::unique_ptr<TUndoStep[]>(new TUndoStep[this->steps_]);
}

/**
 * Backups the specified character and the specified position data (LIFO).
 * @param file_pos The current positon within the file.
 * @param cursor_pos The current cursor position (relative to the file position).
 * @param nibble_pos The current nibble position (relative to the cursor position).
 * @param old_char The character to backup.
 * @return true on success, false otherwise.
 */
bool TUndoEngine::Backup(int64_t file_pos, int32_t cursor_pos, int32_t nibble_pos, unsigned char old_char) noexcept
{
    // Validate the internal data
    if (this->steps_ == 0) return false;
    if (this->undo_buffer_ == nullptr) return false;

    // If the maximum number of steps is reached overwrite the oldest (shift the list)
    if (this->active_step_ >= this->steps_)
    {
        // Shift the steps
        for (uint32_t i = 0; i < this->steps_ - 1; i++) this->undo_buffer_[i] = this->undo_buffer_[i + 1];

        // Adjust the current step
        this->active_step_ = this->steps_ - 1;
    }

    // Add the new entry
    this->undo_buffer_[this->active_step_].file_pos = file_pos;
    this->undo_buffer_[this->active_step_].cursor_pos = cursor_pos;
    this->undo_buffer_[this->active_step_].nibble_pos = nibble_pos;
    this->undo_buffer_[this->active_step_].value = old_char;

    // Increment the active step
    this->active_step_++;

    // Return success
    return true;
}

/**
 * Restores the specified character and position data (LIFO).
 * @param undo_step A pointer to an undo step that receives the backuped data.
 * @return true on success, false otherwise.
 */
bool TUndoEngine::Restore(TUndoStep* undo_step) noexcept
{
    // Validate the internal data
    if (steps_ == 0) return false;
    if (undo_buffer_ == nullptr) return false;

    // Ensure something was backup-ed
    if (active_step_ == 0) return false;

    // Decrement the active step
    active_step_--;

    // Return data of the last undo step
    undo_step->file_pos = undo_buffer_[active_step_].file_pos;
    undo_step->cursor_pos = undo_buffer_[active_step_].cursor_pos;
    undo_step->nibble_pos = undo_buffer_[active_step_].nibble_pos;
    undo_step->value = undo_buffer_[active_step_].value;

    // Return success
    return true;
}
