// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new HEdit application, initializes the console object and loads the application settings.
 */
THEdit::THEdit()
    : files_(0),
    console_(new TConsole()),
    settings_(nullptr),
    search_mode_(TSearchMode::NONE),
    comparator_()
{
    // The string for the home directory
    TString home_dir(512);

    // Initialize the virtual console
    this->console_->Init();

    // Get path to the user's home directory
    this->console_->GetHomeDirectory(home_dir);

    // Load the editor settings
    this->settings_ = std::unique_ptr<TSettings>(new TSettings(home_dir));
}

/**
 * Cleanes up the editor.
 */
THEdit::~THEdit()
{
    // Release all editors
    for (int32_t i = 0; i < this->files_; i++) delete this->editor_[i];

    // Delete clipboard temp file if required
    if (this->settings_->temp_file_persistent_ == false)
    {
        remove(this->settings_->temp_file_name_);
    }

    // Cleanup the virtual console
    this->console_->Cleanup();

    // Deletes the console object
    delete this->console_;
}

/**
 * Creates the editor instances, does the initialisation work and calls the main loop.
 * @param file_count The number of files that are to be edited.
 * @param file The array of files to edit.
 */
void THEdit::Edit(int32_t file_count, char* file[])
{
    // Remember the number of files to edit
    this->files_ = file_count;
    if (this->files_ == 0) throw std::invalid_argument("No files specified");

    // Determine the size of the HEdit editor window
    const auto hedit_width = this->console_->Width();
    const auto console_height = this->console_->Height();
    if ((hedit_width < 80) || (console_height < 18)) throw std::runtime_error("Invalid console size (Must be at least 80x19)");

    // Calculate the height of the HEdit main window
    this->window_height_ = (console_height - 2) - (((console_height - 2) - (this->files_ + 1)) % this->files_);

    // Calculate the height of a single editor window
    const auto editor_height = (this->window_height_ - (this->files_ + 1)) / this->files_;

    // Create HEdit main window
    std::unique_ptr<TWindow> main_window(new TWindow(this->console_, 1, 1, hedit_width, this->window_height_, nullptr, this->settings_->text_color_, this->settings_->text_back_color_));

    // Draw separating bars between the editors
    for (int32_t i = 1; i < this->files_; i++)
    {
        this->console_->SetCursor(1, 1 + (i * editor_height) + i);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_LEFT_CONNECT);
        this->console_->PrintBorders(HE_CONSOLE_BORDER_MIDDLE_MIDDLE, hedit_width - 2);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_RIGHT_CONNECT);
    }

    // Create an editor instance for each file
    for (int32_t i = 0; i < this->files_; i++)
    {
        TEditorInfo info;

        // Create editor info
        info.id_ = i;
        info.width_ = hedit_width;
        info.lines_ = editor_height;
        info.start_line_ = 1 + (i * editor_height) + i;
        info.status_line_ = this->window_height_;

        // Create editor windows
        this->editor_[i] = new TEditor(this->console_, &info, file[i], this->settings_.get(), &this->comparator_);
    }

    // The editors are drawn after ALL editor objects have been created, so the comparator knows ALL files
    for (int32_t i = 0; i < this->files_; i++)
    {
        this->editor_[i]->DrawComplete((i == 0));
    }

    // Update the first editor
    this->editor_[0]->UpdateStatus();
    this->editor_[0]->UpdateCursor();

    // Enter the main loop
    this->MainLoop();
}

/**
 * The application's main loop
 */
void THEdit::MainLoop()
{
    int32_t key_code = 0;
    int32_t active_editor = 0;

    do
    {
        // Wait for key
        key_code = this->console_->WaitForKey();

        // Reset the flag to determine whether to update the cursor
        auto update_cursor = false;

        // Evaluate keys
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F1) && (!IS_SHIFT_KEY(key_code)))  // F1 - Help
        {
            this->Help(this->editor_[active_editor]->view_mode_);
            update_cursor = true;
        }
        if ((IS_CHARACTER_KEY(key_code)) && (KEYCODE(key_code) == 1))  // CTRL - A:  About
        {
            // Display About box
            this->About();

            // Update cursor always (prevent wrong cursor position under Linux)
            this->editor_[active_editor]->UpdateStatus();
            this->editor_[active_editor]->UpdateCursor();

            // Since CTRL-A is mapped to a character key, end here to prevent input
            continue;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_TAB))  // TAB - Select next editor
        {
            this->editor_[active_editor]->DrawFileName(false);
            active_editor++;
            if (active_editor >= this->files_) active_editor = 0;
            this->editor_[active_editor]->DrawFileName(true);
            update_cursor = true;
        }

        // Allow NO operations on closed files (except TAB, F1, CTRL-A)
        if (this->editor_[active_editor]->file_opened_ == false)
        {
            this->editor_[active_editor]->UpdateStatus();
            this->editor_[active_editor]->UpdateCursor();
            continue;
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_BACKSPACE))  // BACKSPACE - Undo
        {
            this->editor_[active_editor]->Undo();
            update_cursor = true;
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_INSERT))  // Insert - Toggle Marker mode
        {
            this->settings_->ToggleMarkerMode();
            update_cursor = true;
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_DELETE))  // Delete - Clear marker
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || (this->settings_->cursor_lock_mode_))
                {
                    this->editor_[i]->GetMarker()->Clear();
                    this->editor_[i]->SetChanged();
                }
            }
            update_cursor = true;
        }

        // Start selection
        if (this->settings_->marker_mode_)
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || (this->settings_->cursor_lock_mode_))
                {
                    // Start selection only if not already started (The StartSelection() function takes care of that)
                    this->editor_[i]->StartSelection();
                }
            }
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F2) && (!IS_SHIFT_KEY(key_code)))  // F2 - Toggle Edit Mode (HEX/DEC) or toggle bit mode (Disassembler)
        {
            this->editor_[active_editor]->EditorFunction1();
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F3) && (!IS_SHIFT_KEY(key_code)))  // F3 - Toggle FileCompare mode
        {
            this->settings_->ToggleCompareMode();
            for (int32_t i = 0; i < this->files_; i++)
            {
                this->editor_[i]->SetChanged();
                this->editor_[i]->DrawFileContent();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F4) && (!IS_SHIFT_KEY(key_code)))  // F4 - Toggle CursorLock mode
        {
            // Ensure that all editors have the same cursor offset
            this->settings_->ToggleCursorLockMode();

            const auto offset = this->editor_[active_editor]->CurrentRelPos();
            for (int32_t i = 0; i < this->files_; i++)
            {
                if (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_) this->editor_[i]->SetCurrentRelPos(offset);
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F5) && (!IS_SHIFT_KEY(key_code)))  // F5 - Assign current address
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || (this->settings_->cursor_lock_mode_))
                {
                    this->editor_[i]->SetCurrentAbsPos(this->editor_[active_editor]->CurrentAbsPos());
                    this->editor_[i]->SetChanged();
                }
            }

            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F5) && (IS_SHIFT_KEY(key_code)))  // Shift-F5 - Calculator
        {
            this->Calculator();
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F6) && (!IS_SHIFT_KEY(key_code)))  // F6 - Goto absolute address
        {
            if (this->GotoAddress(active_editor, TPositioningMode::PM_ABSOLUTE)) this->editor_[active_editor]->SetChanged();
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F6) && (IS_SHIFT_KEY(key_code)))  // Shift-F6 - Goto relative address
        {
            if (this->GotoAddress(active_editor, TPositioningMode::PM_RELATIVE)) this->editor_[active_editor]->SetChanged();
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F7) && (!IS_SHIFT_KEY(key_code)))  // F7 - Continue search Down
        {
            if (this->Search(this->search_mode_, TSearchDirection::FORWARD, active_editor))
            {
                // If a search was performed that affects all files
                // (Comparing Hex or Difference) then update ALL editors
                // othewise only the active one
                if ((this->search_mode_ == TSearchMode::HEX_COMPARING) || (this->search_mode_ == TSearchMode::ANY_DIFFERENCE))
                {
                    for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->SetChanged();
                }
                else
                {
                    this->editor_[active_editor]->SetChanged();
                }
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F7) && (IS_SHIFT_KEY(key_code)))  // Shift-F7 - Continue search Up
        {
            if (this->Search(this->search_mode_, TSearchDirection::BACKWARD, active_editor))
            {
                // If a search was performed that affects all files
                // (Comparing Hex or Difference) then update ALL editors
                // othewise only the active one
                if ((this->search_mode_ == TSearchMode::HEX_COMPARING) || (this->search_mode_ == TSearchMode::ANY_DIFFERENCE))
                {
                    for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->SetChanged();
                }
                else
                {
                    this->editor_[active_editor]->SetChanged();
                }
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F8) && (!IS_SHIFT_KEY(key_code)))  // F8 - Search Down
        {
            if (this->FindMenu(active_editor, TSearchDirection::FORWARD))
            {
                // If a search was performed that affects all files
                // (Comparing Hex or Difference) then update ALL editors
                // othewise only the active one
                if ((this->search_mode_ == TSearchMode::HEX_COMPARING) || (this->search_mode_ == TSearchMode::ANY_DIFFERENCE))
                {
                    for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->SetChanged();
                }
                else
                {
                    this->editor_[active_editor]->SetChanged();
                }
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F8) && (IS_SHIFT_KEY(key_code)))  // Shift-F8 - Search Up
        {
            if (this->FindMenu(active_editor, TSearchDirection::BACKWARD))
            {
                // If a search was performed that affects all files
                // (Comparing Hex or Difference) then update ALL editors
                // othewise only the active one
                if ((this->search_mode_ == TSearchMode::HEX_COMPARING) || (this->search_mode_ == TSearchMode::ANY_DIFFERENCE))
                {
                    for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->SetChanged();
                }
                else
                {
                    this->editor_[active_editor]->SetChanged();
                }
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F9) && (!IS_SHIFT_KEY(key_code)))  // F9 - File operations
        {
            if (this->FileMenu(active_editor)) this->editor_[active_editor]->SetChanged();
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F10) && (!IS_SHIFT_KEY(key_code)))  // F10 - Filter
        {
            this->SwitchViewer(active_editor);
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F10) && (IS_SHIFT_KEY(key_code)))  // Shift-F10 - Disassembler: Show opcode info / HexDec: Show Selection Info:
        {
            this->editor_[active_editor]->EditorFunction2();
            update_cursor = true;
        }
        // Change view modes
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F1) && (IS_SHIFT_KEY(key_code)))  // Shift-F1 - Hex/Dec View
        {
            this->editor_[active_editor]->SetViewMode(TViewMode::HEXDEC);
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F2) && (IS_SHIFT_KEY(key_code)))  // Shift-F2 - Disassembler View
        {
            this->editor_[active_editor]->SetViewMode(TViewMode::DISASSEMBLER);
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F3) && (IS_SHIFT_KEY(key_code)))  // Shift-F3 - Dec View
        {
            this->editor_[active_editor]->SetViewMode(TViewMode::TEXT);
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_F4) && (IS_SHIFT_KEY(key_code)))  // Shift-F4 - User defined viewer
        {
            this->editor_[active_editor]->SetViewMode(TViewMode::SCRIPT, this->settings_->plugin_file_);
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_DOWN))  // Cursor Down
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->CursorDown();
            }
            update_cursor = true;
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_UP))  // Cursor Up
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->CursorUp();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_LEFT))  // Cursor Left
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->CursorLeft();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_RIGHT))  // Cursor Right
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->CursorRight();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_PAGE_DOWN))  // PgDn
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->PageDown();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_PAGE_UP))  // PgUp
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->PageUp();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_HOME))  // Pos1
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->First();
            }
            update_cursor = true;
        }
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_END))  // End
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || ((this->settings_->cursor_lock_mode_) && (this->editor_[active_editor]->view_mode_ == this->editor_[i]->view_mode_))) this->editor_[i]->Last();
            }
            update_cursor = true;
        }

        // Process selection
        if (this->settings_->marker_mode_ == true)
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                if ((i == active_editor) || (this->settings_->cursor_lock_mode_))
                {
                    this->editor_[i]->EndSelection();
                    this->editor_[i]->SetChanged();
                }
            }
        }

        // All character keys will be treated as write attempts
        if (IS_CHARACTER_KEY(key_code))
        {
            // Insert the character in the active editor (the cursor should be updated only if the editor contents were really changed)
            update_cursor = this->editor_[active_editor]->InsertCharacter(static_cast<unsigned char>(KEYCODE(key_code)));
        }

        // Check if compare mode is on
        if (this->settings_->compare_mode_ == true)
        {
            // Mark all editors for update, if the active editor was changed
            if (this->editor_[active_editor]->IsChanged() == true)
            {
                for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->SetChanged();
            }
        }

        // Draw all editors that have changed
        for (int32_t i = 0; i < this->files_; i++)
        {
            this->editor_[i]->DrawFileContent();
        }

        // Update the cursor
        if (update_cursor == true)
        {
            this->editor_[active_editor]->UpdateStatus();
            this->editor_[active_editor]->UpdateCursor();
        }

    } while (!((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_ESC)));
}

/**
 * A shortcut function for displaying a message box.
 * @param title The title of the message box.
 * @param text1 The first line of text.
 * @param text2 The second line of text (can be an empty string).
 */
void THEdit::MessageBox(const char* title, const char* text1, const char* text2)
{
    // Create the message box
    std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, title, this->settings_->dialog_color_, this->settings_->dialog_back_color_));

    // Display the message box
    message_box->Display(TMessageBoxType::INFO, TString(text1), TString(text2));
}

/**
 * Displays the "About" box.
 */
void THEdit::About()
{
    TString text(80);

    // Create the info text
    snprintf(text, text.Size(), "%s (%s)", HE_PROGRAM_TITLE, HE_PROGRAM_BUILD_DATE);

    // Display the message box
    this->MessageBox("About...", text, HE_PROGRAM_COPYRIGHT);
}

/**
 * Displays the help screen depending on the view mode.
 * @param view_mode The view mode to display the help for.
 */
void THEdit::Help(TViewMode view_mode)
{
    int32_t page_no = 0;  // The zero-based number of the help page to be displayed
    int32_t key_code = 0;

    // The Help window should cover the whole screen
    const auto window_width = this->console_->Width();

    // Create the Help windows
    std::unique_ptr<TWindow> window(new TWindow(this->console_, 1, 1, window_width, this->window_height_, "HEdit Help", this->settings_->text_color_, this->settings_->text_back_color_));

    // Default information on every page
    this->console_->SetCursor(2, 2);     this->console_->Print(" Key:        Function:");
    this->console_->SetCursor(2, 15);    this->console_->Print(" In this help: ESC to leave, Cursor Up/Down or PgUp/PgDn to navigate");

    // Disable the cursor
    this->console_->DisableCursor();

    do
    {
        // Clear background
        for (int32_t i = 0; i < 10; i++)
        {
            this->console_->SetCursor(2, 4 + i);
            this->console_->ClearLine();
            this->console_->SetCursor(window_width, 4 + i);
            this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
        }
        if (page_no == 0)
        {
            if ((view_mode == TViewMode::HEXDEC) || (view_mode == TViewMode::TEXT))
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Ins         Toggle selection mode (On/Off)");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Del         Delete current selection");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Tab         Select the next editor");
                this->console_->SetCursor(2, 7);   this->console_->Print(" Backspace   Undo the last change");
                this->console_->SetCursor(2, 8);   this->console_->Print(" CTRL - A    About HEdit");
                this->console_->SetCursor(2, 9);   this->console_->Print(" ESC         Exit HEdit");
                this->console_->SetCursor(2, 10);  this->console_->Print(" Pos1        Jump to the beginning of the file");
                this->console_->SetCursor(2, 11);  this->console_->Print(" End         Jump to the end of the file");
                this->console_->SetCursor(2, 12);  this->console_->Print(" PgUp        Jump one page up");
                this->console_->SetCursor(2, 13);  this->console_->Print(" PgDown      Jump one page down");
            }
            if (view_mode == TViewMode::DISASSEMBLER)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Ins         Toggle selection mode (On/Off)");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Del         Delete current selection");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Tab         Select the next editor");
                this->console_->SetCursor(2, 7);   this->console_->Print(" Backspace   Undo the last change");
                this->console_->SetCursor(2, 8);   this->console_->Print(" CTRL - A    About HEdit");
                this->console_->SetCursor(2, 9);   this->console_->Print(" ESC         Exit HEdit");
                this->console_->SetCursor(2, 10);  this->console_->Print(" Pos1        Jump to the beginning of the file");
                this->console_->SetCursor(2, 13);  this->console_->Print(" PgDown      Jump one page down");
            }
            if (view_mode == TViewMode::SCRIPT)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Ins         Toggle selection mode (On/Off)");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Del         Delete current selection");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Tab         Select the next editor");
                this->console_->SetCursor(2, 8);   this->console_->Print(" CTRL - A    About HEdit");
                this->console_->SetCursor(2, 9);   this->console_->Print(" ESC         Exit HEdit");
            }
        }
        else if (page_no == 1)
        {
            if (view_mode == TViewMode::HEXDEC)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" F1          This help");
                this->console_->SetCursor(2, 5);   this->console_->Print(" F2          Switch edit mode (HEX/DEC)");
                this->console_->SetCursor(2, 6);   this->console_->Print(" F3          Toggle FileCompare mode");
                this->console_->SetCursor(2, 7);   this->console_->Print(" F4          Toggle cursor lock mode");
                this->console_->SetCursor(2, 8);   this->console_->Print(" F5          Assign current address");
                this->console_->SetCursor(2, 9);   this->console_->Print(" F6          Goto absolute address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" F7          Continue search [Down]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" F8          Start search [Down]");
                this->console_->SetCursor(2, 12);  this->console_->Print(" F9          File operations (Copy/Paste, etc.)");
                this->console_->SetCursor(2, 13);  this->console_->Print(" F10         Change view mode");
            }
            if (view_mode == TViewMode::TEXT)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" F1          This help");
                this->console_->SetCursor(2, 6);   this->console_->Print(" F3          Toggle FileCompare mode");
                this->console_->SetCursor(2, 7);   this->console_->Print(" F4          Toggle cursor lock mode");
                this->console_->SetCursor(2, 8);   this->console_->Print(" F5          Assign current address");
                this->console_->SetCursor(2, 9);   this->console_->Print(" F6          Goto absolute address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" F7          Continue search [Down]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" F8          Start search [Down]");
                this->console_->SetCursor(2, 12);  this->console_->Print(" F9          File operations (Copy/Paste, etc.)");
                this->console_->SetCursor(2, 13);  this->console_->Print(" F10         Change view mode");
            }
            if (view_mode == TViewMode::DISASSEMBLER)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" F1          This help");
                this->console_->SetCursor(2, 5);   this->console_->Print(" F2          Switch bit mode (16bit/32bit)");
                this->console_->SetCursor(2, 6);   this->console_->Print(" F3          Toggle FileCompare mode");
                this->console_->SetCursor(2, 7);   this->console_->Print(" F4          Toggle cursor lock mode");
                this->console_->SetCursor(2, 8);   this->console_->Print(" F5          Assign current address");
                this->console_->SetCursor(2, 9);   this->console_->Print(" F6          Goto absolute address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" F7          Continue search [Down]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" F8          Start search [Down]");
                this->console_->SetCursor(2, 12);  this->console_->Print(" F9          File operations (Copy/Paste, etc.)");
                this->console_->SetCursor(2, 13);  this->console_->Print(" F10         Change view mode");
            }
            if (view_mode == TViewMode::SCRIPT)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" F1          This help");
                this->console_->SetCursor(2, 6);   this->console_->Print(" F3          Toggle FileCompare mode");
                this->console_->SetCursor(2, 7);   this->console_->Print(" F4          Toggle cursor lock mode");
                this->console_->SetCursor(2, 9);   this->console_->Print(" F6          Goto absolute address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" F7          Continue search [Down]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" F8          Start search [Down]");
                this->console_->SetCursor(2, 12);  this->console_->Print(" F9          File operations (Copy/Paste, etc.)");
                this->console_->SetCursor(2, 13);  this->console_->Print(" F10         Change view mode");
            }
        }
        else if (page_no == 2)
        {
            if ((view_mode == TViewMode::HEXDEC) || (view_mode == TViewMode::TEXT))
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Shift-F1    Hex/Dec Viewer");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Shift-F2    Disassembler");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Shift-F3    Text Viewer");
                this->console_->SetCursor(2, 7);   this->console_->Print(" Shift-F4    Plugin Viewer (Set via config file)");
                this->console_->SetCursor(2, 8);   this->console_->Print(" Shift-F5    Calculator");
                this->console_->SetCursor(2, 9);   this->console_->Print(" Shift-F6    Goto relative address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" Shift-F7    Continue search [Up]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" Shift-F8    Search [Up]");
                this->console_->SetCursor(2, 13);  this->console_->Print(" Shift-F10   Selection info");
            }
            if (view_mode == TViewMode::DISASSEMBLER)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Shift-F1    Hex/Dec Viewer");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Shift-F2    Disassembler");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Shift-F3    Text Viewer");
                this->console_->SetCursor(2, 7);   this->console_->Print(" Shift-F4    Plugin Viewer (Set via config file)");
                this->console_->SetCursor(2, 8);   this->console_->Print(" Shift-F5    Calculator");
                this->console_->SetCursor(2, 9);   this->console_->Print(" Shift-F6    Goto relative address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" Shift-F7    Continue search [Up]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" Shift-F8    Search [Up]");
                this->console_->SetCursor(2, 13);  this->console_->Print(" Shift-F10   Opcode info");
            }
            if (view_mode == TViewMode::SCRIPT)
            {
                this->console_->SetCursor(2, 4);   this->console_->Print(" Shift-F1    Hex/Dec Viewer");
                this->console_->SetCursor(2, 5);   this->console_->Print(" Shift-F2    Disassembler");
                this->console_->SetCursor(2, 6);   this->console_->Print(" Shift-F3    Text Viewer");
                this->console_->SetCursor(2, 7);   this->console_->Print(" Shift-F4    Plugin Viewer (Set via config file)");
                this->console_->SetCursor(2, 8);   this->console_->Print(" Shift-F5    Calculator");
                this->console_->SetCursor(2, 9);   this->console_->Print(" Shift-F6    Goto relative address");
                this->console_->SetCursor(2, 10);  this->console_->Print(" Shift-F7    Continue search [Up]");
                this->console_->SetCursor(2, 11);  this->console_->Print(" Shift-F8    Search [Up]");
            }
        }

        // Wait for key
        key_code = this->console_->WaitForKey();

        // Navigation
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_PAGE_UP)) page_no--;
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_UP)) page_no--;
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_PAGE_DOWN)) page_no++;
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_DOWN)) page_no++;

        if (page_no > 2) page_no = 0;
        if (page_no < 0) page_no = 2;

    } while (KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_ESC);

    // Enable the cursor
    this->console_->EnableCursor();
}

/**
 * Asks for an address and repositions the file pointer of the currently active editor.
 * @param active_editor The id (index) of the active editor.
 * @param positioning_mode The mode that specifies whether the offset is relative or absolute (see TPositioningMode).
 * @return true if the file pointer position was changed, false otherwise.
 */
bool THEdit::GotoAddress(int32_t active_editor, TPositioningMode positioning_mode)
{
    TString text;
    TString address_string;

    if (positioning_mode == TPositioningMode::PM_RELATIVE)
        text = "Enter relative address (HEX):";
    else
        text = "Enter absolute address (HEX):";

    // Create message box
    std::unique_ptr<TMessageBox> message_box(new TMessageBox(this->console_, "Goto address", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

    // Get address string
    if (message_box->GetString(text, &address_string, 16, true) == false) return false;

    // Get address
    const auto address = address_string.ParseHex();

    if (positioning_mode == TPositioningMode::PM_RELATIVE)
    {
        if ((address + this->editor_[active_editor]->GetFileOffset() > this->editor_[active_editor]->GetFileSize()) ||
        (address + this->editor_[active_editor]->GetFileOffset() < 0))
        {
            this->MessageBox("Invalid address", "The specified address is invalid!", "(Out of range)");
            return false;
        }
        else
        {
            this->editor_[active_editor]->SetCurrentAbsPos(this->editor_[active_editor]->CurrentAbsPos() + address);
        }
    }
    else
    {
        if ((address > this->editor_[active_editor]->GetFileSize()) || (address < 0))
        {
            this->MessageBox("Invalid address", "The specified address is invalid!", "(Out of range)");
            return false;
        }
        else
        {
            this->editor_[active_editor]->SetCurrentAbsPos(address);
        }
    }
    return true;
}

/**
 * Lists all available viewers.
 * @param active_editor The id (index) of the active editor.
 * @return true, if the viewer was changed, false otherwise.
 */
bool THEdit::SwitchViewer(int32_t active_editor)
{
    TString plugin_desc;
    TStringList files;
    TStringList descriptions;

    // Create a script interpreter instance (without assigned file)
    auto script_interpreter = new TScriptInterpreter(this->settings_.get());

    // Enumerate all HEdit Scripts in the HEdit plugin directory
    // This files are *.hs files under windows. Under Linux all files
    // in the plugin directory are supposed to be plugins

    // List all scripts in the specified directory
    this->console_->ListDirectory(this->settings_->plugin_path_, "*.hs", &files);

    // Process all script files
    for (TString plugin_file : files)
    {
        // Try to load the script
        if (script_interpreter->LoadScript(plugin_file) == true)
        {
            // If the script executes, add the script name (description) to the list
            plugin_desc = script_interpreter->ScriptDescription();
        }
        else
        {
            // If the script does NOT execute, add the script file name to the list
            plugin_desc = plugin_file;
            plugin_desc += " (contains errors)";
        }
        descriptions.push_back(plugin_desc);
    }

    // Delete the script interpreter
    delete script_interpreter;

    // Build the menu
    auto menu = new TMenu(this->console_, this->settings_.get(), "Viewer");
    menu->AddEntry("HEX/DEC Viewer");
    menu->AddEntry("Disassembler");
    menu->AddEntry("Text Viewer");
    for (TString desc : descriptions) menu->AddEntry(desc, true);

    // Present the menu
    auto selected_menu_item = menu->Show();

    // Delete the menu object
    delete menu;

    // End here, if the selection was cancelled
    if (selected_menu_item == 0) return false;

    // Change the view mode according to the user's selection
    switch (selected_menu_item)
    {
        case 0:  // Cancelled
            break;
        case 1:
            this->editor_[active_editor]->SetViewMode(TViewMode::HEXDEC);
            break;
        case 2:
            this->editor_[active_editor]->SetViewMode(TViewMode::DISASSEMBLER);
            break;
        case 3:
            this->editor_[active_editor]->SetViewMode(TViewMode::TEXT);
            break;
        default:
            selected_menu_item -= 4;  // Calculate the selected plugin
            this->editor_[active_editor]->SetViewMode(TViewMode::SCRIPT, files[static_cast<std::size_t>(selected_menu_item)]);
            break;
    }

    // Return success
    return true;
}

/**
 * Displays the formula calculator
 */
void THEdit::Calculator()
{
    int32_t key_code = 0;

    // Create the calculator window
    std::unique_ptr<TWindow> window(new TWindow(this->console_, -1, -1, 76, 9, "Calculator", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

    // Display the label
    this->console_->SetCursor(3, 2);
    this->console_->Print("Term:");

    // Display the divider
    this->console_->SetCursor(1, 3);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LEFT_CONNECT);
    this->console_->PrintBorders(HE_CONSOLE_BORDER_MIDDLE_MIDDLE, 74);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_RIGHT_CONNECT);

    // Display the result labels
    this->console_->SetCursor(3, 4);
    this->console_->Print("Result ");
    this->console_->SetCursor(10, 4);
    this->console_->Print("Signed.....:");
    this->console_->SetCursor(10, 5);
    this->console_->Print("Unsigned...:");
    this->console_->SetCursor(10, 6);
    this->console_->Print("Hexdecimal.:");
    this->console_->SetCursor(10, 7);
    this->console_->Print("Binary.....:");

    // Determine the cursor position
    auto cursor_pos = this->formula_string_.Length();

    do
    {
        // Erase term
        this->console_->SetCursor(9, 2);  this->console_->ClearLine();
        this->console_->SetCursor(76, 2); this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
        // Erase results
        this->console_->SetCursor(24, 4); this->console_->ClearLine();
        this->console_->SetCursor(76, 4); this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
        this->console_->SetCursor(24, 5); this->console_->ClearLine();
        this->console_->SetCursor(76, 5); this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
        this->console_->SetCursor(24, 6); this->console_->ClearLine();
        this->console_->SetCursor(76, 6); this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
        this->console_->SetCursor(24, 7); this->console_->ClearLine();
        this->console_->SetCursor(76, 7); this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);

        // Display the term
        this->console_->SetCursor(9, 2);
        this->console_->Print(this->formula_string_);

        // Calculate the term and display the result
        const auto result = this->formula_.Evaluate(this->formula_string_);
        if (this->formula_.GetLastError() == TFormulaError::NONE)
        {
            TString binary;

            // Create 64 bit binary string
            binary.BinaryFromInt(result, 64);

            this->console_->SetCursor(24, 4); this->console_->PrintFormat("%" PRIi64, result);
            this->console_->SetCursor(24, 5); this->console_->PrintFormat("%" PRIu64, result);
            this->console_->SetCursor(24, 6); this->console_->PrintFormat("0x%016" PRIX64, result);
            this->console_->SetCursor(24, 7); this->console_->PrintFormat("%.32s", binary.ToString());
            this->console_->SetCursor(24, 8); this->console_->PrintFormat("%.32s", binary.SubString(32));
        }
        else
        {
            this->console_->SetCursor(24, 4); this->console_->Print(this->formula_.GetLastErrorString());
            this->console_->SetCursor(24, 5); this->console_->Print(this->formula_.GetLastErrorString());
            this->console_->SetCursor(24, 6); this->console_->Print(this->formula_.GetLastErrorString());
            this->console_->SetCursor(24, 7); this->console_->Print(this->formula_.GetLastErrorString());
            this->console_->SetCursor(24, 8); this->console_->Print("                                ");
        }

        // Position the cursor
        this->console_->SetCursor(9 + static_cast<int32_t>(cursor_pos), 2);
        this->console_->Refresh();

        // Wait for key
        key_code = this->console_->WaitForKey();
        if (IS_CHARACTER_KEY(key_code))  // Characters
        {
            // Validate the key code
            auto key_valid = false;
            if ((KEYCODE(key_code) > 47) && (KEYCODE(key_code) < 58)) key_valid = true;
            if ((KEYCODE(key_code) > 64) && (KEYCODE(key_code) < 71)) key_valid = true;
            if ((KEYCODE(key_code) > 96) && (KEYCODE(key_code) < 103)) key_valid = true;
            if (KEYCODE(key_code) == '+') key_valid = true;
            if (KEYCODE(key_code) == '-') key_valid = true;
            if (KEYCODE(key_code) == '*') key_valid = true;
            if (KEYCODE(key_code) == '/') key_valid = true;
            if (KEYCODE(key_code) == '&') key_valid = true;
            if (KEYCODE(key_code) == '|') key_valid = true;
            if (KEYCODE(key_code) == 'x') key_valid = true;
            if (KEYCODE(key_code) == 'h') key_valid = true;
            if (KEYCODE(key_code) == 'd') key_valid = true;
            if (KEYCODE(key_code) == 'b') key_valid = true;

            // If the key is valid add the character to the formula
            if (key_valid)
            {
                this->formula_string_ += static_cast<char>(KEYCODE(key_code));
                cursor_pos++;
            }
        }
        else  // Control keys
        {
            if (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_BACKSPACE)
            {
                if (cursor_pos > 0)
                {
                    this->formula_string_.RemoveRight(1);
                    cursor_pos--;
                }
            }
        }
    } while (KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_ESC);
}

/**
 * Displays the menu with all available file operations and carries out the selected file operation.
 * @param active_editor The currently active editor.
 * @return true if the contents of the active editor have changed, false otherwise.
 */
bool THEdit::FileMenu(int32_t active_editor)
{
    TString buffer;
    TString file_name;
    TString length_string;

    // Create the menu
    std::unique_ptr<TMenu> menu(new TMenu(this->console_, this->settings_.get(), "File"));

    // Add the menu item entries
    menu->AddEntry("Insert from file", true);
    menu->AddEntry("Insert space", true);

    // Check if there is a marker active
    if (this->editor_[active_editor]->GetMarker()->Length() == 0)
    {
        // No marker active, add greyed out menu items
        menu->AddEntry("Write selection to file", false);
        menu->AddEntry("Fill selection", false);
        menu->AddEntry("Copy", false);
    }
    else
    {
        // Marker active, add menu items
        menu->AddEntry("Write selection to file", true);
        menu->AddEntry("Fill selection", true);
        menu->AddEntry("Copy", true);
    }
    menu->AddEntry("Paste", true);

    // Display the menu and wait for a selection
    const auto selected_menu_item = menu->Show();

    // Nothing has changed yet
    auto contents_changed = false;

    // Evaluate the selected menu item
    switch (selected_menu_item)
    {
        case 1:  // Insert from file
        {
            // Create the input box for the file name
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Insert file", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            // Get file name
            if (input_box->GetString(TString("Enter file name:"), &file_name, 80, false) == true)
            {
                // Insert the file at the current position
                contents_changed = this->editor_[active_editor]->InsertFile(this->editor_[active_editor]->CurrentAbsPos(), file_name);
            }
            break;
        }
        case 2:  // Insert space
        {
            // Create the input box for the number of bytes
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Insert space", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            // Get the number of characters to insert
            if (input_box->GetString(TString("Enter length (dec):"), &length_string, 5, false) == true)
            {
                // Insert space
                contents_changed = this->editor_[active_editor]->InsertSpace(this->editor_[active_editor]->CurrentAbsPos(), atoi(length_string));
            }
            break;
        }
        case 3:  // Write selection to file
        {
            // Create the input box for the file name
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Write to file", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            // Get file name
            if (input_box->GetString(TString("Enter file name:"), &file_name, 80, false) == true)
            {
                // Save the current selection to the specified file
                this->editor_[active_editor]->WriteActiveSelectionToFile(file_name, false);
            }
            break;
        }
        case 4:  // Fill selection
        {
            // Create the input box for the hex pattern
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Enter fill pattern", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            // Get the pattern string
            if (input_box->GetString(TString("Enter hex string :"), &buffer, 20, true) == true)
            {
                // Check if the lenght of the fill buffer is a multiple of 2 (only whole bytes)
                if ((buffer.Length() % 2) != 0)
                {
                    // The length of the fill buffer is invalid, display a message box
                    this->MessageBox("Fill selection", "The hex string must be byte-aligned!");
                }
                else
                {
                    // The length of the buffer is valid
                    unsigned char fill_string[64];

                    // Convert the hex string into a byte string
                    const auto fill_string_len = this->ConvertHexString(fill_string, sizeof(fill_string), buffer);

                    // Fill the selection with the byte string
                    contents_changed = this->editor_[active_editor]->FillSelection(fill_string, static_cast<int32_t>(fill_string_len));
                }
            }
            break;
        }
        case 5:  // Copy
        {
            // Copy the current selection to the clipboard
            this->editor_[active_editor]->WriteActiveSelectionToFile(this->settings_->temp_file_name_, true);
            break;
        }
        case 6:  // Paste
        {
            // Paste the current clipboard at the current file offset
            contents_changed = this->editor_[active_editor]->InsertFile(this->editor_[active_editor]->CurrentAbsPos(), this->settings_->temp_file_name_);
            break;
        }
    }

    // Return if the contents have changed
    return contents_changed;
}

/**
 * Displays the menu with the possible search modes and carries out
 * the selected search after querying the search parameters.
 * @param active_editor The id (index) of the active editor.
 * @param search_direction The search direction (see TSearchDirection).
 * @return true on success, false otherwise.
 */
bool THEdit::FindMenu(int32_t active_editor, TSearchDirection search_direction)
{
    TString buffer;
    TString menu_title;

    buffer.New(HE_EDITOR_MAX_SEARCH_STRING_LENGTH);

    // Determine the title for the menu
    if (search_direction == TSearchDirection::BACKWARD)
        menu_title = "Search [Up]";
    else
        menu_title = "Search [Down]";

    // Create the menu
    std::unique_ptr<TMenu> menu(new TMenu(this->console_, this->settings_.get(), menu_title));

    menu->AddEntry("Text", true);
    menu->AddEntry("Unicode Text", true);
    menu->AddEntry("Character range (HEX)", true);
    menu->AddEntry("Characters (HEX)", true);

    if (this->files_ != 1)
    {
        menu->AddEntry("Comparing (HEX)", true);
        menu->AddEntry("Any differences", true);
    }
    else
    {
        menu->AddEntry("Comparing (HEX)", false);
        menu->AddEntry("Any differences", false);
    }
    menu->AddEntry("Probable word", true);
    menu->AddEntry("Count character(s)", true);

    // Display menu
    const auto selected_menu_item = menu->Show();

    // Validate selection
    if (selected_menu_item == 0) return false;

    // Clear search parameters
    this->search_mode_ = TSearchMode::NONE;
    for (int32_t i = 0; i < this->files_; i++)
    {
        memset(this->editor_[i]->search_string_, 0, sizeof(this->editor_[i]->search_string_));
        this->editor_[i]->search_string_length_ = 0;
    }

    // No search started
    auto search_started = false;

    // Start the selcted search after querying the parameters
    switch (selected_menu_item)
    {
        case 1:  // Normal Text search (both case sensitive and insensitive)
        {
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Text search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            if (input_box->GetString(TString("Enter text:"), &buffer, 40, false) == true)
            {
                // Save the search string to all editors
                for (int32_t i = 0; i < this->files_; i++)
                {
                    strncpy_s(reinterpret_cast<char*>(this->editor_[i]->search_string_), HE_EDITOR_MAX_SEARCH_STRING_LENGTH, buffer, HE_EDITOR_MAX_SEARCH_STRING_LENGTH - 1);
                    this->editor_[i]->search_string_length_ = strlen(reinterpret_cast<char*>(this->editor_[i]->search_string_));
                }

                auto detail_menu = new TMenu(this->console_, this->settings_.get(), "How to search");
                detail_menu->AddEntry("Case sensitive", true);
                detail_menu->AddEntry("Case insensitive", true);
                switch (detail_menu->Show())
                {
                case 1:
                    this->search_mode_ = TSearchMode::TEXT_CS;
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                    break;
                case 2:
                    this->search_mode_ = TSearchMode::TEXT_CI;
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                    break;
                }

                delete detail_menu;
            }
            break;
        }
        case 2:  // Unicode text search
        {
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Unicode text search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            if (input_box->GetString(TString("Enter text:"), &buffer, 40, false) == true)
            {
                this->search_mode_ = TSearchMode::UNICODE_TEXT;
                for (int32_t i = 0; i < this->files_; i++)
                {
                    for (uint32_t j = 0; j < buffer.Length() * 2; j += 2)
                    {
                        this->editor_[i]->search_string_[j] = static_cast<unsigned char>(buffer[j / 2]);
                        this->editor_[i]->search_string_[j + 1] = 0;
                    }
                    this->editor_[i]->search_string_length_ = buffer.Length() * 2;
                }
                search_started = this->Search(this->search_mode_, search_direction, active_editor);
            }
            break;
        }
        case 3:  // Hex range search (Char between XX and YY)
        {
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Hex range search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            if (input_box->GetString(TString("Enter lower value:"), &buffer, 2, true) == true)
            {
                this->editor_[0]->search_string_[0] = static_cast<unsigned char>(buffer.ParseHex());

                std::unique_ptr<TMessageBox> input_box2(new TMessageBox(this->console_, "Hex range search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

                if (input_box2->GetString(TString("Enter higher value:"), &buffer, 2, true) == true)
                {
                    this->editor_[0]->search_string_[1] = static_cast<unsigned char>(buffer.ParseHex());
                    this->search_mode_ = TSearchMode::HEX_RANGE;
                    for (int32_t i = 0; i < this->files_; i++)
                    {
                        this->editor_[i]->search_string_[0] = this->editor_[0]->search_string_[0];
                        this->editor_[i]->search_string_[1] = this->editor_[0]->search_string_[1];
                        this->editor_[i]->search_string_length_ = 1;
                    }
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                }
            }
            break;
        }
        case 4:  // Hex character serach (single char or string)
        {
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Hex character search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            if (input_box->GetString(TString("Enter hex string:"), &buffer, 40, true) == true)
            {
                if ((buffer.Length() % 2) != 0)
                {
                    this->MessageBox("Search", "The hex string must be byte-aligned!");
                }
                else
                {
                    this->search_mode_ = TSearchMode::HEX_STRING;
                    for (int32_t i = 0; i < this->files_; i++)
                    {
                        this->editor_[i]->search_string_length_ = this->ConvertHexString(this->editor_[i]->search_string_, sizeof(this->editor_[i]->search_string_), buffer);
                    }
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                }
            }
            break;
        }
        case 5:  // Comparing hex search (Single char or string)
        {
            if (this->files_ > 1)
            {
                char temp[64] = {};
                for (int32_t i = 0; i < this->files_; i++)
                {
                    snprintf(temp, sizeof(temp), "Value in file %" PRIi32 ":", i + 1);
                    std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Comparing hex search", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

                    if (!input_box->GetString(TString(temp), &buffer, 40, true)) break;
                    if ((buffer.Length() % 2) != 0)
                    {
                        this->MessageBox("Search", "The hex string must be byte-aligned!");
                    }
                    else
                    {
                        this->editor_[i]->search_string_length_ = this->ConvertHexString(this->editor_[i]->search_string_, HE_EDITOR_MAX_SEARCH_STRING_LENGTH, buffer);
                    }
                }
                if (this->editor_[0]->search_string_length_ != 0)
                {
                    this->search_mode_ = TSearchMode::HEX_COMPARING;
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                }
            }
            break;
        }
        case 6:  // Search for differences
        {
            this->search_mode_ = TSearchMode::ANY_DIFFERENCE;
            for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->search_string_length_ = 1;
            search_started = this->Search(this->search_mode_, search_direction, active_editor);
            break;
        }
        case 7:  // Search for probable word
        {
            this->search_mode_ = TSearchMode::PROBABLE_WORD;
            for (int32_t i = 0; i < this->files_; i++) this->editor_[i]->search_string_length_ = static_cast<std::size_t>(this->settings_->probable_word_length_);
            search_started = this->Search(this->search_mode_, search_direction, active_editor);
            break;
        }
        case 8:  // Count character(s)
        {
            std::unique_ptr<TMessageBox> input_box(new TMessageBox(this->console_, "Count character(s)", this->settings_->dialog_color_, this->settings_->dialog_back_color_));

            if (input_box->GetString(TString("Enter hex string:"), &buffer, 40, true) == true)
            {
                if ((buffer.Length() % 2) != 0)
                {
                    this->MessageBox("Count characters", "The hex string must be byte-aligned!");
                }
                else
                {
                    this->search_mode_ = TSearchMode::COUNT;
                    for (int32_t i = 0; i < this->files_; i++)
                    {
                        this->editor_[i]->search_string_length_ = this->ConvertHexString(this->editor_[i]->search_string_, sizeof(this->editor_[i]->search_string_), buffer);
                    }
                    search_started = this->Search(this->search_mode_, search_direction, active_editor);
                }
            }
            break;
        }
    }

    // Return the status
    return search_started;
}

/**
 * Converts a specified source string with hex chars (e.g. "FF023D") into it's binary representation.
 * @param target The target buffer that receives the converted string.
 * @param size The size of the target buffer in bytes.
 * @param source The null-terminated source string.
 * @return The number of converted (created) bytes, thus the length of the binary string.
 */
std::size_t THEdit::ConvertHexString(unsigned char* target, std::size_t size, const char* source)
{
    TString buffer;

    // Clear the target buffer
    memset(target, 0, size);

    // Calculate the (expected) size of the target buffer in bytes.
    const auto length = strlen(source) / 2;

    // Initialize the buffer for the target string
    buffer.New(5);

    // Find the minimum of the length of the source string and the size of the input buffer
    const std::size_t byte_count = hedit_min(size, length);

    // Process the source string
    for (std::size_t i = 0; i < byte_count; i++)
    {
        // Copy one hex byte (two characters) to the buffer string
        strncpy_s(buffer, 3, &source[i << 1], 2);
        buffer[2u] = 0;
        target[i] = static_cast<unsigned char>(buffer.ParseHex());
    }

    // Return the length (in bytes) of the hex string that was stored
    return byte_count;
}

/**
 * Carries out the specified search.
 * @param search_mode The type of search to perform (see TSearchMode).
 * @param search_direction The search direction (see TSearchDirection).
 * @param active_editor The id (index) of the active editor.
 * @return true on success, false otherwise.
 */
bool THEdit::Search(TSearchMode search_mode, TSearchDirection search_direction, int32_t active_editor)
{
    bool search_ended = false;
    bool search_found = false;
    bool search_cancelled = false;
    int32_t progress = 0;
    int32_t old_progress = -1;
    int32_t search_counter = 0;
    std::size_t search_string_length = 0;
    TString dialog_title;

    if (search_direction == TSearchDirection::BACKWARD)
        dialog_title = "Search [Up]";
    else
        dialog_title = "Search [Down]";

    // Check for valid search type
    if (search_mode == TSearchMode::NONE)
    {
        this->MessageBox(dialog_title, "Nothing to search!");
        return false;
    }

    // Clear keyboard buffer (discard all input)
    this->console_->ClearKeyboardBuffer();

    // Calculate starting positions
    int64_t start_pos = 0;
    if (search_direction == TSearchDirection::BACKWARD) start_pos = this->editor_[active_editor]->CurrentAbsPos() - 1;
    if (search_direction == TSearchDirection::FORWARD) start_pos = this->editor_[active_editor]->CurrentAbsPos() + 1;
    if (start_pos < 0) start_pos = 0;

    // Calculate search string length
    if ((search_mode == TSearchMode::HEX_COMPARING) || (search_mode == TSearchMode::ANY_DIFFERENCE))
    {
        search_string_length = 0;
        for (int32_t i = 0; i < this->files_; i++)
        {
            if (this->editor_[i]->search_string_length_ > search_string_length) search_string_length = this->editor_[i]->search_string_length_;
        }
    }
    else
    {
        search_string_length = this->editor_[active_editor]->search_string_length_;
    }

    // Retrieve file size
    const auto file_size = this->editor_[active_editor]->GetFileSize();

    do
    {
        // perform the next search operation
        search_found = this->CheckCondition(active_editor, search_mode, start_pos, search_string_length, search_counter);
        if (search_found == true) break;

        // Adjust file pointer
        if (search_direction == TSearchDirection::BACKWARD)
        {
            start_pos--;
            if (start_pos < 0) search_ended = true;
        }
        else
        {
            start_pos++;
            if (static_cast<int64_t>(start_pos + search_string_length) >= file_size) search_ended = true;
        }

        // Draw progress bar
        if (file_size == 0)
        {
            progress = 100;
        }
        else
        {
            if (search_direction == TSearchDirection::BACKWARD)
                progress = static_cast<int32_t>((1 - (static_cast<double>(start_pos) / static_cast<double>(file_size))) * 100);
            else
                progress = static_cast<int32_t>((static_cast<double>(start_pos) / static_cast<double>(file_size)) * 100.0);
        }
        if (old_progress != progress) this->editor_[active_editor]->DrawPercentBar(progress);

        old_progress = progress;

        // Check only every megabyte passes for ESC (Cancel)
        if ((start_pos % 1058576L) == 0)
        {
            search_cancelled = this->console_->CheckCancel();
        }
    } while ((!search_ended) && (!search_cancelled));

    if (search_found)  // SUCCESS
    {
        if ((search_mode == TSearchMode::HEX_COMPARING) || (search_mode == TSearchMode::ANY_DIFFERENCE))  // Update all editors with the new position for this search modes
        {
            for (int32_t i = 0; i < this->files_; i++)
            {
                this->editor_[i]->SetCurrentAbsPos(start_pos);
            }
        }
        else  // Update only currently selected editor for all other search modes
        {
            this->editor_[active_editor]->SetCurrentAbsPos(start_pos);
        }
    }
    else  // FAILURE
    {
        if (search_cancelled)
        {
            this->MessageBox(dialog_title, "The search was cancelled!");
        }
        else
        {
            this->editor_[active_editor]->DrawPercentBar(100);
            switch (search_mode)
            {
                case TSearchMode::TEXT_CS:
                case TSearchMode::TEXT_CI:
                case TSearchMode::UNICODE_TEXT:
                case TSearchMode::HEX_STRING:
                {
                    this->MessageBox(dialog_title, "Search string not found!");
                    break;
                }
                case TSearchMode::HEX_RANGE:
                case TSearchMode::HEX_COMPARING:
                {
                    this->MessageBox(dialog_title, "Value string not found!");
                    break;
                }
                case TSearchMode::ANY_DIFFERENCE:
                {
                    this->MessageBox(dialog_title, "No difference found!");
                    break;
                }
                case TSearchMode::PROBABLE_WORD:
                {
                    this->MessageBox(dialog_title, "No probable word found!");
                    break;
                }
                case TSearchMode::COUNT:
                {
                    if (search_counter == 0)
                    {
                        this->MessageBox(dialog_title, "The search string could not be found!");
                    }
                    else
                    {
                        TString temp_string(80);
                        snprintf(temp_string, temp_string.Size(), "The search string was found %" PRIi32 " times", search_counter);

                        this->MessageBox(dialog_title, temp_string);
                    }
                    break;
                }
                case TSearchMode::NONE:
                    break;
            }
        }
    }

    // Return true, if the search was successfull
    return search_found;
}

/**
 * Checks the file(s) at the specified position, if the file(s) meet the specified search criteria.
 * @param active_editor The id (index) of the active editor.
 * @param search_mode The type of search to perform (See TSearchMode).
 * @param position The current file position.
 * @param search_string_length The length of the search string.
 * @param search_counter The counter if the "count characters" function is used.
 * @return true on success, false otherwise.
 */
bool THEdit::CheckCondition(int32_t active_editor, TSearchMode search_mode, int64_t position, std::size_t search_string_length, int32_t& search_counter) noexcept
{
    unsigned char buffer[HE_MAX_EDITORS][HE_EDITOR_MAX_SEARCH_STRING_LENGTH] = {};

    auto condition = false;

    // Read data to compare
    if ((search_mode == TSearchMode::HEX_COMPARING) || (search_mode == TSearchMode::ANY_DIFFERENCE))
    {
        for (int32_t i = 0; i < this->files_; i++)
        {
            if (!this->editor_[i]->ReadBytesAtOffset(position, buffer[i], search_string_length)) return false;
        }
    }
    else
    {
        if (!this->editor_[active_editor]->ReadBytesAtOffset(position, buffer[active_editor], search_string_length)) return false;
    }

    switch (search_mode)
    {
        case TSearchMode::TEXT_CI:
            if (_strnicmp(reinterpret_cast<char*>(this->editor_[active_editor]->search_string_), reinterpret_cast<char*>(buffer[active_editor]), search_string_length) == 0) condition = true;
            break;
        case TSearchMode::TEXT_CS:
        case TSearchMode::UNICODE_TEXT:
        case TSearchMode::HEX_STRING:
            if (this->strequal(this->editor_[active_editor]->search_string_, buffer[active_editor], search_string_length)) condition = true;
            break;
        case TSearchMode::HEX_RANGE:
            if ((buffer[active_editor][0] >= this->editor_[active_editor]->search_string_[0]) && (buffer[active_editor][0] <= this->editor_[active_editor]->search_string_[1])) condition = true;
            break;
        case TSearchMode::HEX_COMPARING:
            condition = true;
            for (int32_t i = 0; i < this->files_; i++)
            {
                if (!this->strequal(this->editor_[i]->search_string_, buffer[i], this->editor_[i]->search_string_length_)) condition = false;
            }
            break;
        case TSearchMode::ANY_DIFFERENCE:
            condition = false;
            for (int32_t i = 0; i < this->files_ - 1; i++)
            {
                if (buffer[i][0] != buffer[i + 1][0]) condition = true;
            }
            break;
        case TSearchMode::PROBABLE_WORD:
            condition = true;
            for (uint32_t i = 0; i < search_string_length; i++)
            {
                if ((strchr(static_cast<const char*>(this->settings_->probable_word_char_set_), static_cast<char>(buffer[active_editor][i])) == nullptr) || (buffer[active_editor][i] == 0))
                {
                    condition = false;
                }
            }
            break;
        case TSearchMode::COUNT:
            if (this->strequal(this->editor_[active_editor]->search_string_, buffer[active_editor], search_string_length)) search_counter++;
            break;
        case TSearchMode::NONE:
            break;
    }
    return condition;
}

/**
 * Compares two binary strings (ignoring binary zeros) for the specified length.
 * @param string1 The first string to compare.
 * @param string2 The second string to compare.
 * @param length The length (in bytes) to compare.
 * @return true if the strings are equal, false otherwise.
 */
bool THEdit::strequal(const unsigned char* string1, const unsigned char* string2, std::size_t length) noexcept
{
    auto b = true;
    for (std::size_t i = 0; i < length; i++)
    {
        if (string1[i] != string2[i]) b = false;
        if (!b) break;
    }
    return b;
}
