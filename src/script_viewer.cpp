// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new script viewer, allocates the neccessary memory and saves the parameters.
 * @param console The console object used for all output.
 * @param file The file object that is used to access the data to be is displayed.
 * @param file_pos The current absolute position within the file.
 * @param settings The editor settings object.
 * @param editor The information about the editor that owns the viewer.
 * @param marker The marker object (used to determine if a byte is marked).
 * @param comparator The comparator engine (used to color the differences).
 */
TScriptViewer::TScriptViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator)
    : TBaseViewer(console, file, file_pos, settings, editor, marker, comparator, nullptr),
    total_lines_(0),
    current_line_(0)
{
    // Initialize the script interpreter
    this->script_interpreter_ = std::unique_ptr<TScriptInterpreter>(new TScriptInterpreter(this->settings_->plugin_path_));

    // Create the virtual screen buffer
    this->screen_buffer_ = new TString[HE_SCRIPT_MAX_SCREEN_ROWS];
}

/**
 * Frees the virtual screen buffer and releases the script interpreter.
 */
TScriptViewer::~TScriptViewer()
{
    // Free the virtual screen buffer
    delete[] this->screen_buffer_;
}

/**
 * Initializes the script viewer and loads the script.
 * @param string The initialisation string.
 */
void TScriptViewer::Init(TString & string)
{
    // Load the script
    this->script_interpreter_->LoadScript(string);

    // Reset the cursor
    this->current_line_ = 0;
}

/**
 * The viewer-specific Draw() function, executes the script for the current offset
 * and presents the output of the virtual screen as scrollable window.
 */
void TScriptViewer::Draw()
{
    // Check, if to draw the editor contents
    if (this->IsChanged() == false) return;

    // Clear the virtual screen buffer
    for (int32_t i = 0; i < HE_SCRIPT_MAX_SCREEN_ROWS; i++) this->screen_buffer_[i].Free();

    // Execute the selected script
    this->total_lines_ = this->script_interpreter_->ScriptExecuteAt(this->file_, *this->file_pos_, this->screen_buffer_);
    if (this->current_line_ > this->total_lines_ - 1) this->current_line_ = this->total_lines_ - 1;

    // Validate cursor
    if (this->current_line_ < 0) this->current_line_ = 0;

    // Set the script color
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // Draw the screen buffer lines
    for (int32_t i = 0; i < hedit_min(this->total_lines_, this->editor_->lines_); i++)
    {
        // Set the cursor position
        this->console_->SetCursor(2, this->editor_->start_line_ + i + 1);

        // Create and truncate current line
        TString line = this->screen_buffer_[this->current_line_ + i];
        if (line.Length() > static_cast<std::size_t>(this->editor_->width_ - 2)) line[static_cast<std::size_t>(this->editor_->width_) - 2] = 0;

        // Filter possible illegal characters: tab and newlines
        line.StripChar(0x09);
        line.StripChar(0x0a);
        line.StripChar(0x0d);

        // Print line
        this->console_->Print(line);

        // Clear to eol and redraw right border
        this->console_->ClearLine();
        this->console_->SetCursor(this->editor_->width_, this->editor_->start_line_ + i + 1);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }

    // Clear the rest of the lines
    for (int32_t i = this->total_lines_; i < this->editor_->lines_; i++)
    {
        this->console_->SetCursor(2, this->editor_->start_line_ + i + 1);
        this->console_->ClearLine();
        this->console_->SetCursor(this->editor_->width_, this->editor_->start_line_ + i + 1);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }

    // Mark the editor as unchanged
    this->ClearChanged();
}

/**
 * Displays the script name in the status bar.
 */
void TScriptViewer::UpdateStatus() noexcept
{
    // Display the status line
    this->console_->SetCursor(this->editor_->status_column_, this->editor_->status_line_);
    this->console_->PrintFormat("[ %-26.26s ]", this->script_interpreter_->ScriptDescription());
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);
}

/**
 * Moves the virtual screen one line up.
 */
void TScriptViewer::CursorUp() noexcept
{
    // If we are already in the first line, do nothing
    if (this->current_line_ <= 0) return;

    // Move one line up
    this->current_line_--;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the virtual screen one line down.
 */
void TScriptViewer::CursorDown() noexcept
{
    // If we are alredy at the end, do nothing
    if (this->current_line_ + this->editor_->lines_ >= this->total_lines_) return;

    // Move one line down
    this->current_line_++;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves to the first line of the virtual screen.
 */
void TScriptViewer::First() noexcept
{
    this->current_line_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves to the last line of the virtual screen (- editor lines).
 */
void TScriptViewer::Last() noexcept
{
    this->current_line_ = this->total_lines_ - this->editor_->lines_;
    if (this->current_line_ < 0) this->current_line_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * [Unused]
 * @param character The key code of the key that was pressed.
 * @return true if a change occured that requires the cursor to be updated, false otherwise.
 */
bool TScriptViewer::InsertCharacter(unsigned char character) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(character);
    #endif
    // Nothing changed
    return false;
}

/**
 * Returns the current absolute file position (here only the file position).
 * @return The current absolute file position (here only the file position).
 */
int64_t TScriptViewer::CurrentAbsPos() noexcept
{
    return (*this->file_pos_);
}

/**
 * Returns the current relative file position (here always zero).
 * @return The current relative file position (here always zero).
 */
int32_t TScriptViewer::CurrentRelPos() noexcept
{
    return 0;
}

/**
 * Sets the current absolute file position.
 * @param offset The absolute file offset to move the file pointer to.
 */
void TScriptViewer::SetCurrentAbsPos(int64_t offset) noexcept
{
    (*this->file_pos_) = offset;
}

/**
 * Sets the current relative file position (cursor position). [Unused]
 * @param offset The relative offset to move the cursor to.
 */
void TScriptViewer::SetCurrentRelPos(int32_t offset) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(offset);
    #endif
}

/**
 * Displays the datatype sizes of all available data types.
 */
void TScriptViewer::EditorFunction1()
{
    char text1[128], text2[128];

    // Create the text lines
    snprintf(text1, sizeof(text1), "byte: %i bit - short/word: %i bit", static_cast<int32_t>(sizeof(uint8_t) * 8), static_cast<int32_t>(sizeof(uint16_t) * 8));
    snprintf(text2, sizeof(text2), "long/dword: %i bit - qword: %i bit", static_cast<int32_t>(sizeof(uint32_t) * 8), static_cast<int32_t>(sizeof(uint64_t) * 8));

    // Display the message box
    this->MessageBox("Datatype Sizes", text1, text2);
}
