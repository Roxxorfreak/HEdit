// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new hex viewer, allocates the neccessary memory and saves the parameters.
 * @param console The console object used for all output.
 * @param file The file object that is used to access the data to be is displayed.
 * @param file_pos The current absolute position within the file.
 * @param settings The editor settings object.
 * @param editor The information about the editor that owns the viewer.
 * @param marker The marker object (used to determine if a byte is marked).
 * @param comparator The comparator engine (used to color the differences).
 * @param undo_engine The undo engine used to track and undo changes.
 */
THexViewer::THexViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept
    : TBaseViewer(console, file, file_pos, settings, editor, marker, comparator, undo_engine)
{
    this->cursor_pos_ = 0;
    this->nibble_pos_ = 0;
    this->edit_mode_ = THexEditMode::HEX;

    // Calculate the block positions
    this->hex_column_ = this->editor_->address_column_ + this->editor_->address_width_ + 2;
    this->dec_column_ = this->hex_column_ + 49;
}

/**
 * Initializes the hex viewer.
 * @param string The initialisation string.
 */
void THexViewer::Init(TString &string) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(string);
    #endif
}

/**
 * Draws the whole hex viewer.
 */
void THexViewer::Draw()
{
    char address[32] = {};

    // Check, if to draw the editor contents
    if (!this->IsChanged()) return;

    // Allocate memory, if that fails do not continue
    auto buffer = new unsigned char[static_cast<unsigned>((this->editor_->lines_ * this->editor_->width_) + 1)];

    // Draw addresses
    for (int32_t j = 0; j < this->editor_->lines_; j++)
    {
        // Create the address string
        snprintf(address, sizeof(address), "%016" PRIX64, static_cast<int64_t>(((*this->file_pos_) + (static_cast<int64_t>(j) * 16))));
        // Display the address string, but emit the first bytes (if not enough space)
        this->console_->SetCursor(this->editor_->address_column_, 1 + this->editor_->start_line_ + j);
        this->console_->Print(&address[16 - this->editor_->address_width_]);
    }

    // Load data
    const auto bytes_read = static_cast<int32_t>(this->file_->ReadAt(buffer, static_cast<uint32_t>(this->editor_->lines_ * 16), *this->file_pos_));

    // Calculate the starting position
    const auto start_y = this->editor_->start_line_ + 1;

    // Process all read bytes
    for (int32_t j = 0; j < bytes_read; j++)
    {
        // Check if the current byte is selected
        if (this->marker_->IsSelected((*this->file_pos_) + j))
        {
            // The byte is selected, use the selection color
            this->console_->SetColor(this->settings_->marked_text_color_);
            this->console_->SetBackground(this->settings_->marked_text_back_color_);
        }
        else
        {
            // The byte is not selected, check if compare mode is active
            if (this->settings_->compare_mode_ == true)
            {
                // The compare mode is active, check if all bytes are equal
                if (this->comparator_->CompareOffset(j) == false)
                {
                    // The bytes are different, use "difference color", depending on editor number
                    this->console_->SetColor(this->settings_->difference_color_[this->editor_->id_]);
                    this->console_->SetBackground(this->settings_->difference_back_color_[this->editor_->id_]);
                }
                else
                {
                    // The bytes are equal, use the default editor colors
                    this->console_->SetColor(this->settings_->text_color_);
                    this->console_->SetBackground(this->settings_->text_back_color_);
                }
            }
            else
            {
                // Nothing special, use the default editor colors
                this->console_->SetColor(this->settings_->text_color_);
                this->console_->SetBackground(this->settings_->text_back_color_);
            }
        }

        // Display the HEX character for the current byte
        this->console_->SetCursor(this->hex_column_ + ((j % 16) * 3), start_y + (j / 16));
        this->console_->PrintFormat("%02" PRIX8, static_cast<uint8_t>(buffer[j]));

        // Print the ASCII character for the current byte
        this->console_->SetCursor(this->dec_column_ + (j % 16), start_y + (j / 16));
        this->console_->PrintChar(buffer[j]);
    }

    // Set the default editor color
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // Clear the space that is not occupied by data (only if there is less data than display space)
    for (int32_t j = bytes_read; j < (this->editor_->lines_ * 16); j++)
    {
        this->console_->SetCursor(this->hex_column_ + ((j % 16) * 3), start_y + (j / 16));
        this->console_->Print("  ");
        this->console_->SetCursor(this->dec_column_ + (j % 16), start_y + (j / 16));
        this->console_->Print(" ");
    }

    // Delete the buffer for the file content
    delete[] buffer;

    // Mark the editor as unchanged
    this->ClearChanged();
}

/**
 * Updates the cursor position depending on the selected edit mode.
 */
void THexViewer::UpdateCursor() noexcept
{
    switch (this->edit_mode_)
    {
    case THexEditMode::HEX:
        this->console_->SetCursor((this->hex_column_ + ((this->cursor_pos_ % 16) * 3) + this->nibble_pos_), (1 + this->editor_->start_line_ + (this->cursor_pos_ / 16)));
        break;
    case THexEditMode::DEC:
        this->console_->SetCursor((this->dec_column_ + (this->cursor_pos_ % 16)), (1 + this->editor_->start_line_ + (this->cursor_pos_ / 16)));
        break;
    }
}

/**
 * Displays information about the byte under the cursor in the status bar.
 */
void THexViewer::UpdateStatus()
{
    this->UpdateCurrentCharStatus(this->cursor_pos_);
}

/**
 * Moves the cursor one line up, scrolling the file if neccessary.
 */
void THexViewer::CursorUp() noexcept
{
    // If the cursor is not in the first line, just move the cursor one line up
    if (this->cursor_pos_ >= 16)
    {
        this->cursor_pos_ -= 16;
        return;
    }

    // Otherwise change the file position
    if (((*this->file_pos_) - 16) >= 0)
    {
        (*this->file_pos_) -= 16;

        // Mark the editor as "changed"
        this->SetChanged();
    }
}

/**
 * Moves the cursor one line down, scrolling the file if neccessary.
 */
void THexViewer::CursorDown() noexcept
{
    if ((this->cursor_pos_ + 16) < this->editor_->lines_ * 16)
    {
        this->cursor_pos_ += 16;
        return;
    }

    if (((*this->file_pos_) + this->cursor_pos_) < this->file_->FileSize())
    {
        (*this->file_pos_) += 16;

        // Mark the editor as "changed"
        this->SetChanged();
    }
}

/**
 * Moves the cursor one nibble (HEX) or byte (DEC) left, scrolling the file if neccessary.
 */
void THexViewer::CursorLeft() noexcept
{
    if (this->edit_mode_ == THexEditMode::HEX)
    {
        if (this->nibble_pos_ == 1)
        {
            this->nibble_pos_ = 0;
            return;
        }
    }

    if (this->cursor_pos_ > 0)
    {
        this->cursor_pos_--;
        this->nibble_pos_ = 1;
    }
    else
    {
        if (((*this->file_pos_) - 16) >= 0)
        {
            (*this->file_pos_) -= 16;
            this->nibble_pos_ = 1;
            this->cursor_pos_ = 15;

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the cursor one nibble (HEX) or byte (DEC) right, scrolling the file if neccessary.
 */
void THexViewer::CursorRight() noexcept
{
    if (this->edit_mode_ == THexEditMode::HEX)
    {
        if (this->nibble_pos_ == 0)
        {
            this->nibble_pos_ = 1;
            return;
        }
    }

    if (this->cursor_pos_ < (this->editor_->lines_ * 16) - 1)
    {
        this->cursor_pos_++;
        this->nibble_pos_ = 0;
    }
    else
    {
        if (((*this->file_pos_) + this->cursor_pos_) < this->file_->FileSize())
        {
            (*this->file_pos_) += 16;
            this->nibble_pos_ = 0;
            this->cursor_pos_ = ((this->editor_->lines_ - 1) * 16);

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the file pointer one page up.
 */
void THexViewer::PageUp() noexcept
{
    if ((*this->file_pos_) - (static_cast<int64_t>(this->editor_->lines_) * 16) >= 0)
    {
        (*this->file_pos_) -= (static_cast<int64_t>(this->editor_->lines_) * 16);
    }
    else
    {
        (*this->file_pos_) = 0;
        this->cursor_pos_ = 0;
        this->nibble_pos_ = 0;
    }

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer one page down.
 */
void THexViewer::PageDown() noexcept
{
    if (((*this->file_pos_) + (static_cast<int64_t>(this->editor_->lines_) * 32)) < this->file_->FileSize())
    {
        (*this->file_pos_) += (static_cast<int64_t>(this->editor_->lines_) * 16);
    }
    else
    {
        (*this->file_pos_) = this->file_->FileSize() - (static_cast<int64_t>(this->editor_->lines_) * 16);
        this->cursor_pos_ = (this->editor_->lines_ * 16) - 1;
        this->nibble_pos_ = 0;
    }

    // Validate the file position
    if (*this->file_pos_ < 0) (*this->file_pos_) = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer to the beginning of the file.
 */
void THexViewer::First() noexcept
{
    (*this->file_pos_) = 0;
    this->cursor_pos_ = 0;
    this->nibble_pos_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer to EOF (-Page height).
 */
void THexViewer::Last() noexcept
{
    (*this->file_pos_) = this->file_->FileSize() - (static_cast<int64_t>(this->editor_->lines_) * 16);
    if ((*this->file_pos_) < 0) (*this->file_pos_) = 0;
    this->cursor_pos_ = (this->editor_->lines_ * 16) - 1;
    this->nibble_pos_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Changes the nibble (HEX) or byte (DEC) under the cursor, backing up the old value before.
 * @param character The key code of the key that was pressed.
 * @return true if a change occured that requires the cursor to be updated, false otherwise.
 */
bool THexViewer::InsertCharacter(unsigned char character) noexcept
{
    unsigned char this_char = 0;

    if (this->edit_mode_ == THexEditMode::HEX)
    {
        // Validate input character
        if (this->IsHexCharacter(character) == false) return false;

        // Read the previous character
        this->file_->ReadAt(&this_char, 1, (*this->file_pos_) + this->cursor_pos_);

        // Backup the previous character
        this->undo_engine_->Backup((*this->file_pos_), this->cursor_pos_, this->nibble_pos_, this_char);

        // Calculate the hex value from the typed character
        int32_t value = 0;
        if ((character >= 48) && (character <= 57))  value = character - 48;
        if ((character >= 65) && (character <= 70))  value = character - 55;
        if ((character >= 97) && (character <= 102)) value = character - 87;

        // Calculate the new value
        if (this->nibble_pos_ == 0)
        {
            value = value << 4;
            this_char = static_cast<unsigned char>(this_char & static_cast<unsigned char>(0x0F));
            this_char = static_cast<unsigned char>(this_char | static_cast<unsigned char>(value));
        }
        else
        {
            this_char = static_cast<unsigned char>(this_char & static_cast<unsigned char>(0xF0));
            this_char = static_cast<unsigned char>(this_char | static_cast<unsigned char>(value));
        }

        // Write the new (changed) character to file
        this->file_->WriteAt(&this_char, 1, (*this->file_pos_) + this->cursor_pos_);

        // Adjust nibble or cursor position
        if (this->nibble_pos_ == 0)
        {
            this->nibble_pos_ = 1;
        }
        else
        {
            if (this->cursor_pos_ < (this->editor_->lines_ * 16) - 1)
            {
                this->cursor_pos_++;
                this->nibble_pos_ = 0;
            }
            else
            {
                (*this->file_pos_) += 16;
                this->nibble_pos_ = 0;
                this->cursor_pos_ = ((this->editor_->lines_ - 1) * 16);
            }
        }

        // Mark the editor as "changed"
        this->SetChanged();

        // Return true to reposition cursor
        return true;
    }

    // DEC Edit mode: Filter the allowed characters
    // Accept no control characters in DEC mode (And no TAB, Backspace, CR or ESC as characters)
    if ((character != 0) && (character != 9) && (character != 8) && (character != 27) && (character != 13))
    {
        // Read the previous character
        this->file_->ReadAt(&this_char, 1, (*this->file_pos_) + this->cursor_pos_);

        // Backup the previous character
        this->undo_engine_->Backup((*this->file_pos_), this->cursor_pos_, this->nibble_pos_, this_char);

        // Write the new character to file
        this->file_->WriteAt(&character, 1, (*this->file_pos_) + this->cursor_pos_);

        // Adjust cursor position
        if (this->cursor_pos_ < (this->editor_->lines_ * 16) - 1)
        {
            this->cursor_pos_++;
            this->nibble_pos_ = 0;
        }
        else
        {
            (*this->file_pos_) += 16;
            this->nibble_pos_ = 0;
            this->cursor_pos_ = ((this->editor_->lines_ - 1) * 16);
        }

        // Mark the editor as "changed"
        this->SetChanged();

        // Return true to reposition cursor
        return true;
    }
    return false;
}

/**
 * Undoes the last change.
 */
void THexViewer::Undo() noexcept
{
    TUndoStep last_step;

    // Request the last change
    const auto success = this->undo_engine_->Restore(&last_step);

    // Ensure an undo step was loaded
    if (success == false) return;

    // Restore the file positions
    (*this->file_pos_) = last_step.file_pos;
    this->cursor_pos_ = last_step.cursor_pos;
    this->nibble_pos_ = last_step.nibble_pos;

    // Write the old value to the file
    this->file_->WriteAt(&last_step.value, 1, (*this->file_pos_) + this->cursor_pos_);

    // Mark the file as "changed"
    this->SetChanged();
}

/**
 * Sets the start of the marker to the current absolute position, if not yet set.
 */
void THexViewer::StartSelection() noexcept
{
    if (this->marker_->start_ == -1) this->marker_->start_ = this->CurrentAbsPos();
}

/**
 * Sets the end of the marker to the current absolute position.
 */
void THexViewer::EndSelection() noexcept
{
    this->marker_->end_ = this->CurrentAbsPos();
}

/**
 * Returns the current absolute file position (file + cursor).
 * @return The current absolute file position (file + cursor).
 */
int64_t THexViewer::CurrentAbsPos() noexcept
{
    return (*this->file_pos_) + this->cursor_pos_;
}

/**
 * Returns the current relative file position (cursor position).
 * @return The current relative file position (cursor position).
 */
int32_t THexViewer::CurrentRelPos() noexcept
{
    return this->cursor_pos_;
}

/**
 * Sets the current absolute file position.
 * @param offset The absolute file offset to move the file pointer to.
 */
void THexViewer::SetCurrentAbsPos(int64_t offset) noexcept
{
    (*this->file_pos_) = offset;
    this->cursor_pos_ = 0;
    this->nibble_pos_ = 0;
}

/**
 * Sets the current relative file position (cursor position).
 * @param offset The relative offset to move the cursor to.
 */
void THexViewer::SetCurrentRelPos(int32_t offset) noexcept
{
    this->cursor_pos_ = offset;
    this->nibble_pos_ = 0;
}

/**
 * Toggles the edit mode between Hex and Dec.
 */
void THexViewer::EditorFunction1() noexcept
{
    // Toggle the edit mode
    if (this->edit_mode_ == THexEditMode::HEX)
    {
        this->edit_mode_ = THexEditMode::DEC;
    }
    else
    {
        this->edit_mode_ = THexEditMode::HEX;
    }

    // Jump to the first nibble
    this->nibble_pos_ = 0;
}

/**
 * Displays the length of the currently active selection (marker).
 */
void THexViewer::EditorFunction2()
{
    TString text;

    // Get the length of the current selection marker (in bytes)
    const auto selection_length = this->marker_->Length();

    // Create the text depending on the selection length
    if (selection_length == 0)
    {
        text = "Nothing selected";
    }
    else
    {
        text.New(80);
        snprintf(text, text.Size(), "Selection lenght: %" PRIi64 " bytes", selection_length);
    }

    // Display the message box
    this->MessageBox("Selection Info", text);
}

/**
 * Checks if the specified character is a valid hex character.
 * @param character_code The character code to check.
 * @return true if the character is valid, false otherwise.
 */
bool THexViewer::IsHexCharacter(unsigned char character_code)
{
    if ((character_code >= 65) && (character_code <= 70)) return true;
    if ((character_code >= 97) && (character_code <= 102)) return true;
    if ((character_code >= 48) && (character_code <= 57)) return true;
    return false;
}
