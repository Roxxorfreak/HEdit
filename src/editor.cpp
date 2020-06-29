// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

TEditorInfo::TEditorInfo() noexcept
{
    this->id_ = 0;
    this->lines_ = 0;
    this->width_ = 0;
    this->start_line_ = 0;
    this->status_line_ = 0;
    this->status_column_ = 0;
    this->address_width_ = 0;
    this->address_column_ = 0;
}

/**
 * Initializes an Editor object.
 * An editor manages the editing of a single file using different viewers.
 * @param console The console object for all output.
 * @param editor_info The editor information (sizes) from the editor.
 * @param file_name The file name of the file to edit.
 * @param settings The settings object.
 * @param comparator The comparator object.
 */
TEditor::TEditor(TConsole* console, TEditorInfo* editor_info, const char* file_name, TSettings* settings, TComparator* comparator)
{
    this->file_pos_ = 0;
    this->console_ = console;
    this->settings_ = settings;
    this->comparator_ = comparator;
    this->search_string_length_ = 0;
    memset(this->search_string_, 0, sizeof(this->search_string_));

    // Prepare editor info
    this->editor_info_.id_ = editor_info->id_;
    this->editor_info_.lines_ = editor_info->lines_;
    this->editor_info_.width_ = editor_info->width_;
    this->editor_info_.start_line_ = editor_info->start_line_;
    this->editor_info_.status_line_ = editor_info->status_line_;
    this->editor_info_.status_column_ = 3;
    this->editor_info_.address_width_ = hedit_min(this->editor_info_.width_ - 71, 16);
    this->editor_info_.address_column_ = 3;

    // Store the file name
    this->file_name_ = file_name;

    // Create the marker
    this->marker_ = new TMarker();

    // Create the file object
    this->file_ = new TFile(this->file_name_, this->settings_->use_caching_);

    // Try to open the file for editing
    this->file_opened_ = false;
    if (this->file_->Open(TFileMode::READWRITE) == false)
    {
        if (this->file_->Open(TFileMode::READ) == false)
        {
            this->file_opened_ = false;
        }
        else
        {
            this->file_opened_ = true;
        }
    }
    else
    {
        this->file_opened_ = true;
    }

    // Initialize the Undo engine
    this->undo_engine_ = new TUndoEngine(this->settings_->undo_steps_);

    // Add the newly created editor instance (this) to the compare manager
    this->comparator_->Add(this);

    // Initialize the viewers
    // Hex Viewer
    this->hex_viewer_ = new THexViewer(this->console_, this->file_, &this->file_pos_, this->settings_, &this->editor_info_, this->marker_, this->comparator_, this->undo_engine_);
    // Disassembler Viewer
    this->asm_viewer_ = new TAsmViewer(this->console_, this->file_, &this->file_pos_, this->settings_, &this->editor_info_, this->marker_, this->comparator_);
    // Text Viewer
    this->text_viewer_ = new TTextViewer(this->console_, this->file_, &this->file_pos_, this->settings_, &this->editor_info_, this->marker_, this->comparator_, this->undo_engine_);
    // Script Viewer
    this->script_viewer_ = new TScriptViewer(this->console_, this->file_, &this->file_pos_, this->settings_, &this->editor_info_, this->marker_, this->comparator_);

    // Set the initially active viewer
    this->view_mode_ = TViewMode::HEXDEC;
    this->active_viewer_ = this->hex_viewer_;
}

/**
 * Frees all internally used objects.
 */
TEditor::~TEditor()
{
    delete this->script_viewer_;
    delete this->text_viewer_;
    delete this->asm_viewer_;
    delete this->hex_viewer_;
    delete this->undo_engine_;
    delete this->file_;
    delete this->marker_;
}

/**
 * Draws the complete editor: Clears the background, draws the file name and the file content.
 * @param active true to draw the editor as active, false otherwise.
 */
void TEditor::DrawComplete(bool active)
{
    // Clear window
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);
    for (int32_t i = this->editor_info_.start_line_ + 1; i <= (this->editor_info_.start_line_ + this->editor_info_.lines_); i++)
    {
        // This is faster than printing spaces
        this->console_->SetCursor(2, i);
        this->console_->ClearLine();
        this->console_->SetCursor(this->editor_info_.width_, i);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }
    this->DrawFileName(active);
    this->SetChanged();
    this->DrawFileContent();
}

/**
 * Draws the name of the file that is associated with the editor, either
 * active or inactive, appending the file's status (read-only, denied, ...).
 * @param active true to draw the file name as active, false otherwise.
 */
void TEditor::DrawFileName(bool active)
{
    TString file_name;
    TString display_string;

    // Select the color of the file name, depending on the "active" state of the editor
    if (active)
    {
        this->console_->SetColor(this->settings_->active_file_color_);
        this->console_->SetBackground(this->settings_->active_file_back_color_);
    }
    else
    {
        this->console_->SetColor(this->settings_->text_color_);
        this->console_->SetBackground(this->settings_->text_back_color_);
    }

    // Create the filename that is displayed
    file_name = this->file_name_;
    file_name += " ";

    // Append file status
    if (this->file_opened_ == false)
    {
        file_name += "[DENIED] ";
    }
    else if (this->file_->IsReadOnly())
    {
        file_name += "[READ-ONLY] ";
    }

    // Title of editor
    display_string = " File: ";

    // Ensure that the file name is smaller than the window
    if (file_name.Length() < static_cast<std::size_t>(this->editor_info_.width_ - 9))
    {
        display_string += file_name;
    }
    else
    {
        display_string += "...";
        display_string += file_name.SubString(file_name.Length() - (static_cast<std::size_t>(this->editor_info_.width_) - 14));
    }

    // Draw file name
    this->console_->SetCursor(3, this->editor_info_.start_line_);
    this->console_->Print(display_string);
    this->console_->Refresh();
}

/**
 * Prepares the drawing context (Colors, file check) and calls
 * the Draw() function of the currently active editor.
 */
void TEditor::DrawFileContent()
{
    // Disable the cursor
    this->console_->DisableCursor();

    // Set the default text color
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // Do not draw anything if access to the file was denied
    if ((this->file_opened_ == false) && (this->active_viewer_->IsChanged()))
    {
        // Display error
        this->console_->SetColor(TColor::RED);
        this->console_->SetBackground(this->settings_->text_back_color_);
        this->console_->SetCursor(3, this->editor_info_.start_line_ + 1);
        this->console_->Print("File Access Denied!");

        // Prevent further update
        this->active_viewer_->ClearChanged();
    }
    else
    {
        // Draw the viewer contents
        this->active_viewer_->Draw();
    }

    // Refresh the console
    this->console_->Refresh();

    // Re-enable the cursor
    this->console_->EnableCursor();
}

/**
 * Calls the UpdateCursor() function of the currently active viewer.
 * If the file access is denied, set the cursor to test first letter of the "access denied" text.
 */
void TEditor::UpdateCursor()
{
    // Check, if the file is open
    if (file_opened_ == false)
    {
        this->console_->SetCursor(3, this->editor_info_.start_line_ + 1);
        return;
    }

    // Update the cursor of the current editor
    this->active_viewer_->UpdateCursor();

    // Refresh the console
    this->console_->Refresh();
}

/**
 * Calls the first viewer-specific function of the currently active viewer.
 * For the HEX Viwer this is "Toggle Edit Mode".
 * For the Assembler Viewer this is "Toggle Bit Mode (16/32 Bit)".
 */
void TEditor::EditorFunction1()
{
    this->active_viewer_->EditorFunction1();
}

/**
 * Calls the second viewer-specific function of the currently active viewer.
 * For the HEX and the DEC Viwer this is "Show Selection Info".
 * For the Assembler Viewer this is "Show Opcode Info".
 */
void TEditor::EditorFunction2()
{
    this->active_viewer_->EditorFunction2();
}

/**
 * Reads the specified number of bytes from the specified (absolute) offset
 * into the specified buffer (This function is e.g. called from the
 * comparator, to decide if bytes at a specified offset are different).
 * @param offset The zero-based file offset to read at.
 * @param buffer The buffer to read the data into.
 * @param count The number of bytes to read.
 * @return true on success, false otherwise.
 */
bool TEditor::ReadBytesAtOffset(int64_t offset, unsigned char* buffer, uint32_t count) noexcept
{
    if (this->file_->ReadAt(buffer, count, offset) < count) return false;
    return true;
}

/**
 * Returns the size of the file associated with the editor (Called from GotoAddress).
 * @return The file size.
 */
int64_t TEditor::GetFileSize() noexcept
{
    return this->file_->FileSize();
}

/**
 * Returns the current file position.
 * @return The current file position.
 */
int64_t TEditor::GetFileOffset() const noexcept
{
    return this->file_pos_;
}

/**
 * Calls the CursorUp() function of the currently active viewer.
 */
void TEditor::CursorUp()
{
    this->active_viewer_->CursorUp();
}

/**
 * Calls the CursorDown() function of the currently active viewer.
 */
void TEditor::CursorDown()
{
    this->active_viewer_->CursorDown();
}

/**
 * Calls the CursorLeft() function of the currently active viewer.
 */
void TEditor::CursorLeft()
{
    this->active_viewer_->CursorLeft();
}

/**
 * Calls the CursorRight() function of the currently active viewer.
 */
void TEditor::CursorRight()
{
    this->active_viewer_->CursorRight();
}

/**
 * Calls the PageUp() function of the currently active viewer.
 */
void TEditor::PageUp()
{
    this->active_viewer_->PageUp();
}

/**
 * Calls the PageDown() function of the currently active viewer.
 */
void TEditor::PageDown()
{
    this->active_viewer_->PageDown();
}

/**
 * Calls the First() function of the currently active viewer.
 */
void TEditor::First()
{
    this->active_viewer_->First();
}

/**
 * Calls the Last() function of the currently active viewer.
 */
void TEditor::Last()
{
    this->active_viewer_->Last();
}

/**
 * Calls the KeyPressed() function of the currently active viewer if the viewer accepts input.
 * @param character The character key that was pressed.
 */
bool TEditor::InsertCharacter(unsigned char character)
{
    // If the file is read-only, ignore the request
    if (this->file_->IsReadOnly()) return false;

    // The file is writeable, pass the request to the viewer
    return this->active_viewer_->InsertCharacter(character);
}

/**
 * Returns true if the specified file exists, false otherwise.
 * @param file_name The name of the file to check.
 * @return true if the specified file exists, false otherwise.
 */
bool TEditor::Exists(const char* file_name)
{
    // Create the file object
    auto file = new TFile(file_name, false);

    // Try to open the file for reading
    const auto return_value = file->Open(TFileMode::READ);

    // Close the file, if it was open
    file->Close();

    // Delete the file object
    delete file;

    // Return status
    return return_value;
}

/**
 * Writes the bytes that are currently selected into the specified file.
 * If this file exists and the overwrite flag is true
 * the user is asked before overwriting the file, if not it is overwritten.
 * If nothing is selected, the function fails.
 * @param file_name The name of the file to write to.
 * @param overwrite true to overwrite the file without confirmation, false to ask.
 * @return true, if the file was written, false otherwise.
 */
bool TEditor::WriteActiveSelectionToFile(const char* file_name, bool overwrite)
{
    unsigned char buffer[2] = {};

    if (this->marker_->Length() == 0) return false;

    // Verify that the target file doen't exist or is save to overwrite
    if (this->Exists(file_name))
    {
        if (!overwrite)
        {
            std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Warning", this->settings_->dialog_color_, this->settings_->dialog_back_color_));
            if (!message_box->Display(TMessageBoxType::YESNO, TString("The file already exists!"), TString("Do you want to overwrite it?"))) return false;
        }
    }

    // Create the file object
    auto target_file = new TFile(file_name, this->settings_->use_caching_);

    // Open the target file
    if (target_file->Open(TFileMode::CREATE) == false)
    {
        // Delete file object
        delete target_file;

        // Display the error
        std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Error", this->settings_->dialog_color_, this->settings_->dialog_back_color_));
        message_box->Display(TMessageBoxType::INFO, TString("Unable to open the target file!"), TString());

        // Return failure
        return false;
    }

    // Copy byte by byte, displaying the the status after each 1k transferred data
    this->file_->Seek(this->marker_->Start());
    for (int64_t i = 0; i < this->marker_->Length(); i++)
    {
        this->file_->Read(buffer, 1);
        target_file->Write(buffer, 1);
        if ((i % 1024) == 0) this->DrawPercentBar(static_cast<int32_t>((static_cast<double>(i) / static_cast<double>(this->marker_->Length())) * 100.0));
    }

    // Draw the full percent bar
    this->DrawPercentBar(100);

    // Delete file object
    delete target_file;

    // Return success
    return true;
}

/**
 * Inserts the specified file at the specified offset.
 * @param position The position to insert the data.
 * @param file_name The name of the file to insert.
 * @return true on success, false otherwise.
 */
bool TEditor::InsertFile(int64_t position, const char* file_name)
{
    unsigned char buffer[2] = {};

    // Check, if the specified source exists
    if (this->Exists(file_name) == false)
    {
        std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Error", this->settings_->dialog_color_, this->settings_->dialog_back_color_));
        message_box->Display(TMessageBoxType::INFO, TString("Nothing to insert!"));
        return false;
    }

    // Open the source file
    auto source_file = new TFile(file_name, this->settings_->use_caching_);

    if (!source_file->Open(TFileMode::READ))
    {
        // Delete file object
        delete source_file;

        // Display the error
        std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Error", this->settings_->dialog_color_, this->settings_->dialog_back_color_));
        message_box->Display(TMessageBoxType::INFO, TString("Unable to open file!"));

        // Return failure
        return false;
    }

    // Copy byte by byte, displaying the the status after each 1k transferred data
    this->file_->Seek(position);
    const auto file_size = source_file->FileSize();
    for (int64_t i = 0; i < file_size; i++)
    {
        source_file->Read(buffer, 1);
        this->file_->Write(buffer, 1);
        if ((i % 1024) == 0) this->DrawPercentBar(static_cast<int32_t>((static_cast<double>(i) / static_cast<double>(file_size)) * 100.0));
    }

    this->DrawPercentBar(100);

    // Delete file object
    delete source_file;

    // Return success
    return true;
}

/**
 * Inserts the specified number of bytes filled with zeros at the specified absolute offset, shifting the following bytes.
 * @param position The position to insert the space at.
 * @param length The length of the space to insert.
 * @return true on success, false otherwise.
 */
bool TEditor::InsertSpace(int64_t position, int32_t length)
{
    unsigned char buffer[1] = {};

    // Verify the specified length
    if ((length < 1) || (length > 65536L))
    {
        std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Error", this->settings_->dialog_color_, this->settings_->dialog_back_color_));
        message_box->Display(TMessageBoxType::INFO, TString("The length must be between 1 and 65536!"));
        return false;
    }

    // Save the file size
    // This must be saved, because the length of the file will change
    const auto old_file_size = this->file_->FileSize();

    this->DrawPercentBar(0);
    for (int64_t i = old_file_size; i > position; i--)
    {
        // Shift bytes <lLength> bytes down
        this->file_->Seek(i - 1);
        this->file_->Read(buffer, 1);
        this->file_->Seek(i + length - 1);
        this->file_->Write(buffer, 1);
        this->file_->Seek(i - 1);
        buffer[0] = 0;
        this->file_->Write(buffer, 1);
        if ((i % 512) == 0) DrawPercentBar(static_cast<int32_t>((static_cast<double>(old_file_size - i) / static_cast<double>(old_file_size)) * 100.0));
    }
    this->DrawPercentBar(100);
    return true;
}

/**
 * Fills the currently selected bytes with the specified pattern.
 * This is done repeatedly, so that the whole selection is filled,
 * even if it is longer than the pattern. If the pattern is longer than the
 * selection, the pattern is truncated.
 * @param fill_string The filling pattern.
 * @param fill_string_len The length of the filling string.
 * @return true on success, false otherwise.
 */
bool TEditor::FillSelection(const unsigned char* fill_string, int32_t fill_string_len) noexcept
{
    // Fail if nothing is selected
    if (this->marker_->Length() == 0) return false;

    // Fill the current selection
    this->file_->Seek(this->marker_->Start());
    for (int32_t i = 0; i < static_cast<int32_t>(this->marker_->Length() / fill_string_len) * fill_string_len; i += fill_string_len)
    {
        this->file_->Write(fill_string, static_cast<uint32_t>(fill_string_len));
        if ((i % 1024) == 0) DrawPercentBar(static_cast<int32_t>((static_cast<double>(i) / static_cast<double>(this->marker_->Length())) * 100.0));
    }
    // Fill the rest (Truncated pattern)
    if ((this->marker_->Length() % fill_string_len) != 0) this->file_->Write(fill_string, static_cast<uint32_t>(this->marker_->Length() % fill_string_len));  // Fill rest of selection
    this->DrawPercentBar(100);
    return true;
}

/**
 * Draws a progress bar with the specified percentage in the status line.
 * @param percent The percentage to display.
 */
void TEditor::DrawPercentBar(int32_t percent) noexcept
{
    const auto x = static_cast<int32_t>((static_cast<double>(percent) / 100.0) * (static_cast<double>(this->editor_info_.width_) - 6.0));

    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);
    this->console_->SetCursor(3, this->editor_info_.status_line_);
    this->console_->Print("[");
    this->console_->SetCursor(this->editor_info_.width_ - 2, this->editor_info_.status_line_);
    this->console_->Print("]");
    this->console_->SetCursor(4, this->editor_info_.status_line_);
    for (int32_t i = 0; i < this->editor_info_.width_ - 6; i++)
    {
        if (i <= x)
        {
            this->console_->PrintBorder(HE_CONSOLE_CHARACTER_PROGRESS_BAR);
        }
        else
        {
            this->console_->PrintBorder(' ');
        }
    }
    this->console_->SetCursor((this->editor_info_.width_ / 2) - 2, editor_info_.status_line_);
    this->console_->PrintFormat(" %3" PRIi32 "%% ", percent);
    this->console_->Refresh();
}

/**
 * Calls the Undo() function of the currently active viewer.
 */
void TEditor::Undo()
{
    this->active_viewer_->Undo();
}

/**
 * Calls the StartSelection() function of the currently active viewer.
 */
void TEditor::StartSelection()
{
    this->active_viewer_->StartSelection();
}

/**
 * Calls the EndSelection() function of the currently active viewer.
 */
void TEditor::EndSelection()
{
    this->active_viewer_->EndSelection();
}

/**
 * Calls the CurrentAbsPos() function of the currently active viewer.
 * @return The current absolute file position (file + cursor).
 */
int64_t TEditor::CurrentAbsPos()
{
    return this->active_viewer_->CurrentAbsPos();
}

/**
 * Calls the CurrentRelPos() function of the currently active viewer.
 * @return The current relative position (cursor).
 */
int32_t TEditor::CurrentRelPos()
{
    return this->active_viewer_->CurrentRelPos();
}

/**
 * Calls the SetCurrentAbsPos() function of the currently active viewer.
 * @param offset The new absolute file position.
 */
void TEditor::SetCurrentAbsPos(int64_t offset)
{
    this->active_viewer_->SetCurrentAbsPos(offset);
}

/**
 * Calls the SetCurrentRelPos() function of the currently active viewer.
 * @param offset The new relative file position (cursor).
 */
void TEditor::SetCurrentRelPos(int32_t offset)
{
    this->active_viewer_->SetCurrentRelPos(offset);
}

/**
 * Displays the editor status and viewer-specific status.
 */
void TEditor::UpdateStatus()
{
    char address[32] = {};
    double percentage = 0;
    int32_t remaining_spaces = 0;

    // Disable the cursor
    this->console_->DisableCursor();

    // Set the color for the border
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // If the file is not open display "Access denied", otherwise
    // call the viewer-specific function
    if (this->file_opened_ == false)
    {
        this->console_->SetCursor(this->editor_info_.status_column_, this->editor_info_.status_line_);
        this->console_->Print("[ Access to file was denied! ]");
        this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);
    }
    else  // File open
    {
        this->active_viewer_->UpdateStatus();
    }

    // Display the absolute position of the editor (HEX Address)
    // Create the address string
    snprintf(address, sizeof(address), "%016" PRIX64, this->CurrentAbsPos());

    // Display the address string, but emit the first bytes (if not enough space)
    this->console_->SetCursor(34, this->editor_info_.status_line_);
    this->console_->PrintFormat("[ %s ]", &address[16 - this->editor_info_.address_width_]);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);

    // Display the position in the file in %
    // Calculate the percentage
    const auto file_size = this->GetFileSize();
    if (file_size != 0) percentage = ((static_cast<double>(this->CurrentAbsPos())) / (static_cast<double>(file_size)) * 100.0);
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;
    this->console_->SetCursor(39 + this->editor_info_.address_width_, this->editor_info_.status_line_);
    this->console_->PrintFormat("[ %3.0f%% ]", percentage);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);

    // Display FileCompare and CursorLock status if more that one file is open,
    // otherwise display the version info
    if (comparator_->EditorCount() == 1)
    {
        // Display the program version
        this->console_->SetCursor(48 + this->editor_info_.address_width_, this->editor_info_.status_line_);
        this->console_->PrintFormat("[ %s ]", HE_PROGRAM_TITLE);

        // Calculate the remaining spaces
        remaining_spaces = this->editor_info_.width_ - (52 + static_cast<int32_t>(strlen(HE_PROGRAM_TITLE)) + this->editor_info_.address_width_);
    }
    else
    {
        this->console_->SetCursor(48 + this->editor_info_.address_width_, this->editor_info_.status_line_);
        if (this->settings_->compare_mode_)
            this->console_->Print("[ FC: ON  ]");
        else
            this->console_->Print("[ FC: OFF ]");
        this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);
        if (this->settings_->cursor_lock_mode_)
            this->console_->Print("[ CL: ON  ]");
        else
            this->console_->Print("[ CL: OFF ]");

        // Calculate the remaining spaces  (size of the CL info is 11 characters)
        remaining_spaces = this->editor_info_.width_ - (71 + this->editor_info_.address_width_);
    }

    // Fill the rest of the line with slashes
    this->console_->PrintBorders(HE_CONSOLE_BORDER_LOWER_MIDDLE, remaining_spaces);

    // Redraw console
    this->console_->Refresh();
    this->console_->EnableCursor();
}

/**
 * Changes the currently active viewer.
 * @param view_mode The new view mode.
 * @param script_name The name of the script to execute.
 */
void TEditor::SetViewMode(TViewMode view_mode, TString script_name)
{
    // Change the view mode
    this->view_mode_ = view_mode;

    // Change the active editor object
    this->active_viewer_ = nullptr;
    if (this->view_mode_ == TViewMode::HEXDEC)          this->active_viewer_ = this->hex_viewer_;
    if (this->view_mode_ == TViewMode::TEXT)            this->active_viewer_ = this->text_viewer_;
    if (this->view_mode_ == TViewMode::DISASSEMBLER)    this->active_viewer_ = this->asm_viewer_;
    if (this->view_mode_ == TViewMode::SCRIPT)          this->active_viewer_ = this->script_viewer_;

    // Ensure a valid editor object
    if (this->active_viewer_ == nullptr) throw std::runtime_error("No valid editor object");

    // Initialize the viewer editor
    this->active_viewer_->Init(script_name);

    // Redraw the complete editor
    this->DrawComplete(true);
}

/**
 * Marks the currently selected editor as changed, causing redraw the next time the editor is painted.
 */
void TEditor::SetChanged() noexcept
{
    this->active_viewer_->SetChanged();
}

/**
 * Returns true, if the current editor was changed.
 * @return true, if the current editor was changed.
 */
bool TEditor::IsChanged() const noexcept
{
    return this->active_viewer_->IsChanged();
}

/**
 * Returns the marker object of the editor.
 * @return The marker object of the editor.
 */
TMarker *TEditor::GetMarker() noexcept
{
    return this->marker_;
}
