// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new text viewer, allocates the neccessary memory and saves the parameters.
 * @param console The console object used for all output.
 * @param file The file object that is used to access the data to be is displayed.
 * @param file_pos The current absolute position within the file.
 * @param settings The editor settings object.
 * @param editor The information about the editor that owns the viewer.
 * @param marker The marker object (used to determine if a byte is marked).
 * @param comparator The comparator engine (used to color the differences).
 * @param undo_engine The undo engine used to track and undo changes.
 */
TTextViewer::TTextViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator, TUndoEngine* undo_engine) noexcept
    : TBaseViewer(console, file, file_pos, settings, editor, marker, comparator, undo_engine),
    cursor_pos_(0)
{
    // Calculate the block positions
    this->text_column_ = this->editor_->address_column_ + this->editor_->address_width_ + 2;

    // Calculate the maximum length of the text line
    this->text_width_ = this->editor_->width_ - this->text_column_ - 1;
}

/**
 * Initializes the text viewer.
 * @param string The initialisation string.
 */
void TTextViewer::Init(TString &string) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(string);
    #endif
}

/**
 * Draws the whole text viewer.
 */
void TTextViewer::Draw()
{
    char address[32] = {};

    // Check, if to draw the editor contents
    if (this->IsChanged() == false) return;

    // Memory for the text data read from the file
    auto buffer = new unsigned char[static_cast<unsigned>((this->editor_->lines_ * this->editor_->width_) + 1)];

    // Draw addresses
    for (int32_t j = 0; j < this->editor_->lines_; j++)
    {
        // Create the address string
        snprintf(address, sizeof(address), "%016" PRIX64, static_cast<int64_t>((*this->file_pos_) + (static_cast<int64_t>(j) * this->text_width_)));
        // Display the address string, but emit the first bytes (if not enough space)
        this->console_->SetCursor(this->editor_->address_column_, 1 + this->editor_->start_line_ + j);
        this->console_->Print(&address[16 - this->editor_->address_width_]);
    }

    // Load data
    const auto bytes_read = static_cast<int32_t>(this->file_->ReadAt(buffer, static_cast<unsigned>(this->editor_->lines_ * this->text_width_), *this->file_pos_));

    // Calculate the starting position
    const auto StartY = this->editor_->start_line_ + 1;

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

        // Print the ASCII character for the current byte
        this->console_->SetCursor(this->text_column_ + (j % this->text_width_), StartY + (j / this->text_width_));
        this->console_->PrintChar(buffer[j]);
    }

    // Set the default editor color
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // Clear the space that is not occupied by data (only if there is less data than display space)
    for (int32_t j = bytes_read; j < this->editor_->lines_ * this->text_width_; j++)
    {
        this->console_->SetCursor(this->text_column_ + (j % this->text_width_), StartY+(j / this->text_width_));
        this->console_->Print(" ");
    }

    // Delete the buffer for the file content
    delete[] buffer;

    // Mark the editor as unchanged
    this->ClearChanged();
}

/**
 * Updates the cursor position.
 */
void TTextViewer::UpdateCursor() noexcept
{
    this->console_->SetCursor(this->text_column_ + (this->cursor_pos_ % this->text_width_), 1 + this->editor_->start_line_ + (this->cursor_pos_ / this->text_width_));
}

/**
 * Displays information about the byte under the cursor in the status bar.
 */
void TTextViewer::UpdateStatus()
{
    this->UpdateCurrentCharStatus(this->cursor_pos_);
}

/**
 * Moves the cursor one line up, scrolling the file if neccessary.
 */
void TTextViewer::CursorUp() noexcept
{
    if (this->cursor_pos_ >= this->text_width_)
    {
        this->cursor_pos_ -= this->text_width_;
    }
    else
    {
        if (((*this->file_pos_) - this->text_width_) >= 0)
        {
            (*this->file_pos_) -= this->text_width_;

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the cursor one line down, scrolling the file if neccessary.
 */
void TTextViewer::CursorDown() noexcept
{
    if ((this->cursor_pos_ + this->text_width_) < this->editor_->lines_ * this->text_width_)
    {
        this->cursor_pos_ += this->text_width_;
    }
    else
    {
        if (((*this->file_pos_) + this->cursor_pos_) < this->file_->FileSize())
        {
            (*this->file_pos_) += this->text_width_;

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the cursor one byte left, scrolling the file if neccessary.
 */
void TTextViewer::CursorLeft() noexcept
{
    if (this->cursor_pos_ > 0)
    {
        this->cursor_pos_--;
    }
    else
    {
        if (((*this->file_pos_) - this->text_width_) >= 0)
        {
            (*this->file_pos_) -= this->text_width_;
            this->cursor_pos_ = this->text_width_;

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the cursor one byte right, scrolling the file if neccessary.
 */
void TTextViewer::CursorRight() noexcept
{
    if (this->cursor_pos_ < (this->editor_->lines_ * this->text_width_) - 1)
    {
        this->cursor_pos_++;
    }
    else
    {
        if (((*this->file_pos_) + this->cursor_pos_) < this->file_->FileSize())
        {
            (*this->file_pos_)  += this->text_width_;
            this->cursor_pos_ = ((this->editor_->lines_ - 1) * this->text_width_);

            // Mark the editor as "changed"
            this->SetChanged();
        }
    }
}

/**
 * Moves the file pointer one page up.
 */
void TTextViewer::PageUp() noexcept
{
    if ((*this->file_pos_) - (static_cast<int64_t>(this->editor_->lines_) * this->text_width_) >= 0)
    {
        (*this->file_pos_) -= (static_cast<int64_t>(this->editor_->lines_) * this->text_width_);
    }
    else
    {
        (*this->file_pos_) = 0;
        this->cursor_pos_ = 0;
    }

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer one page down.
 */
void TTextViewer::PageDown() noexcept
{
    if (((*this->file_pos_) + (static_cast<int64_t>(this->editor_->lines_) * this->text_width_ * 2)) < this->file_->FileSize())
    {
        (*this->file_pos_) += (static_cast<int64_t>(this->editor_->lines_) * this->text_width_);
    }
    else
    {
        (*this->file_pos_) = this->file_->FileSize() - (static_cast<int64_t>(this->editor_->lines_) * this->text_width_);
        this->cursor_pos_ = (this->editor_->lines_ * this->text_width_) - 1;
    }

    // Validate the file position
    if (*this->file_pos_ < 0) (*this->file_pos_) = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer to the beginning of the file.
 */
void TTextViewer::First() noexcept
{
    (*this->file_pos_) = 0;
    this->cursor_pos_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer to EOF (-Page height).
 */
void TTextViewer::Last() noexcept
{
    (*this->file_pos_) = this->file_->FileSize() - (static_cast<int64_t>(this->editor_->lines_) * this->text_width_);
    if ((*this->file_pos_) < 0) (*this->file_pos_) = 0;
    this->cursor_pos_ = (this->editor_->lines_ * this->text_width_) - 1;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Changes the byte under the cursor, backing up the old value before.
 * @param character The key code of the key that was pressed.
 * @return true if a change occured that requires the cursor to be updated, false otherwise.
 */
bool TTextViewer::InsertCharacter(unsigned char character) noexcept
{
    unsigned char thisChar = 0;

    // Filter the allowed characters
    // Accept no control characters in DEC mode (and no TAB, Backspace, CR or ESC as characters)
    if ((character != 0) && (character != 9) && (character != 8) && (character != 27) && (character != 13))
    {
        // Read the previous character
        this->file_->ReadAt(&thisChar, 1, (*this->file_pos_) + this->cursor_pos_);

        // Backup the previous character
        this->undo_engine_->Backup((*this->file_pos_), this->cursor_pos_, 0, thisChar);

        // Write the new character to file
        this->file_->WriteAt(&character, 1, (*this->file_pos_) + this->cursor_pos_);

        // Adjust cursor position
        if (cursor_pos_ < (this->editor_->lines_ * this->text_width_) - 1)
        {
            this->cursor_pos_++;
        }
        else
        {
            (*this->file_pos_)  += this->text_width_;
            this->cursor_pos_ = ((this->editor_->lines_ - 1) * this->text_width_);
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
void TTextViewer::Undo() noexcept
{
    TUndoStep last_step;

    // Request the last change
    const auto success = this->undo_engine_->Restore(&last_step);

    // Ensure an undo step was loaded
    if (success == false) return;

    // Restore the file positions
    (*this->file_pos_) = last_step.file_pos;
    this->cursor_pos_ = last_step.cursor_pos;

    // Write the old value to the file
    this->file_->WriteAt(&last_step.value, 1, (*this->file_pos_) + this->cursor_pos_);

    // Mark the file as "changed"
    this->SetChanged();
}

/**
 * Sets the start of the marker to the current absolute position, if not yet set.
 */
void TTextViewer::StartSelection() noexcept
{
    if (this->marker_->start_ == -1) this->marker_->start_ = this->CurrentAbsPos();
}

/**
 * Sets the end of the marker to the current absolute position.
 */
void TTextViewer::EndSelection() noexcept
{
    this->marker_->end_ = this->CurrentAbsPos();
}

/**
 * Returns the current absolute file position (file + cursor).
 * @return The current absolute file position (file + cursor).
 */
int64_t TTextViewer::CurrentAbsPos() noexcept
{
    return (*this->file_pos_) + this->cursor_pos_;
}

/**
 * Returns the current relative file position (cursor position).
 * @return The current relative file position (cursor position).
 */
int32_t TTextViewer::CurrentRelPos() noexcept
{
    return this->cursor_pos_;
}

/**
 * Sets the current absolute file position.
 * @param offset The absolute file offset to move the file pointer to.
 */
void TTextViewer::SetCurrentAbsPos(int64_t offset) noexcept
{
    (*this->file_pos_) = offset;
    this->cursor_pos_ = 0;
}

/**
 * Sets the current relative file position (cursor position).
 * @param offset The relative offset to move the cursor to.
 */
void TTextViewer::SetCurrentRelPos(int32_t offset) noexcept
{
    this->cursor_pos_ = offset;
}

/**
 * [Unused]
 */
void TTextViewer::EditorFunction1() noexcept
{
}

/**
 * Displays the length of the currently active selection (marker).
 */
void TTextViewer::EditorFunction2()
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
