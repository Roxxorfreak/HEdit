// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new disassembler viewer, allocates the neccessary memory and saves the parameters.
 * @param console The console object used for all output.
 * @param file The file object that is used to access the data to be is displayed.
 * @param file_pos The current absolute position within the file.
 * @param settings The editor settings object.
 * @param editor The information about the editor that owns the viewer.
 * @param marker The marker object (used to determine if a byte is marked).
 * @param comparator The comparator engine (used to color the differences).
 */
TAsmViewer::TAsmViewer(TConsole* console, TFile* file, int64_t* file_pos, TSettings* settings, TEditorInfo* editor, TMarker* marker, TComparator* comparator)
    : TBaseViewer(console, file, file_pos, settings, editor, marker, comparator, nullptr),
    screen_buffer_(nullptr),
    hex_column_(0),
    asm_width_(0),
    asm_column_(0),
    current_line_(0),
    last_current_line_(0),
    disassembled_lines_(0),
    machine_code_bytes_(0),
    buffer_(nullptr),
    architecture_(TArchitecture::x86_32),
    disassembler_(nullptr),
    instruction_(nullptr)
{
    // Determine the maximum number of machine code bytes to be displayed
    this->machine_code_bytes_ = static_cast<uint32_t>(this->editor_->width_ - 53) / 3;

    // Limit number of machine code bytes to 15 (max. allowed instruction bytes)
    if (this->machine_code_bytes_ > HE_MAX_INSTRUCTION_LENGTH) this->machine_code_bytes_ = HE_MAX_INSTRUCTION_LENGTH;

    // Calculate the block positions
    this->hex_column_ = this->editor_->address_column_ + this->editor_->address_width_ + 2;
    this->asm_column_ = this->hex_column_ + static_cast<int32_t>(this->machine_code_bytes_ * 3);
    this->asm_width_ = this->editor_->width_ - this->asm_column_ - 2;

    // Initialize the disassembler engine
    this->disassembler_ = new TDisassembler();

    // Allocate the memory for the machine code buffer
    // (assuming a single instruction can be up to HE_MAX_INSTRUCTION_LENGTH bytes in size)
    this->buffer_ = new TAsmBuffer(static_cast<std::size_t>(this->editor_->lines_) * HE_MAX_INSTRUCTION_LENGTH);

    // Allocate the memory for the data and instruction buffer
    this->instruction_ = new TAsmInstruction[static_cast<unsigned>(this->editor_->lines_)];

    // Allocate the memory for the screen buffer (rows)
    this->screen_buffer_ = new char*[static_cast<unsigned>(this->editor_->lines_)];

    for (int32_t i = 0; i < this->editor_->lines_; i++)
    {
        // Allocate the memory for the screen buffer (columns)
        this->screen_buffer_[i] =  new char[static_cast<std::size_t>(this->editor_->width_) + 1];

        // Clear the line buffer
        memset(this->screen_buffer_[i], 0, static_cast<std::size_t>(this->editor_->width_) + 1);
    }
}

/**
 * Frees the memory.
 */
TAsmViewer::~TAsmViewer()
{
    // Free the screen buffer
    if (this->screen_buffer_ != nullptr)
    {
        for (int32_t i = 0; i < this->editor_->lines_; i++)
        {
            if (this->screen_buffer_[i] != nullptr) delete[] this->screen_buffer_[i];
        }
        delete[] this->screen_buffer_;
    }

    // Free the data and instruction buffer
    if (this->instruction_ != nullptr) delete[] this->instruction_;

    // Free the file data buffer
    if (this->buffer_ != nullptr) delete this->buffer_;

    // Free the disassembler engine
    delete this->disassembler_;
}

/**
 * Initializes the disassembler viewer.
 * @param string The initialisation string.
 */
void TAsmViewer::Init(TString &string) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(string);
    #endif
}

/**
 * The viewer-specific Draw() function, disassembles the specified number of lines and displays them.
 */
void TAsmViewer::Draw()
{
    char string[128] = {};

    // Check, if to draw the editor contents
    if (!this->IsChanged()) return;

    // Load machine code instructions (byte code)
    this->buffer_->LoadFromFile(this->file_, *this->file_pos_);
    if (this->buffer_->GetCodeLength() == 0) return;

    // Disassemble as many lines as there are lines to display (in the editor window)
    this->disassembled_lines_ = this->disassembler_->Disassemble(this->buffer_, this->instruction_, this->editor_->lines_, this->architecture_, this->settings_->number_format_);

    // Adjust the cursor, if there are less lines to display than before
    if (this->current_line_ > this->disassembled_lines_ - 1) this->current_line_ = this->disassembled_lines_ - 1;

    // Set the default colors
    this->console_->SetColor(this->settings_->text_color_);
    this->console_->SetBackground(this->settings_->text_back_color_);

    // Process all disassembled lines
    for (int32_t i = 0; i < this->disassembled_lines_; i++)
    {
        // Clear the screen buffer
        memset(this->screen_buffer_[i], 0, static_cast<std::size_t>(this->editor_->width_) + 1);

        // Create one complete line (all calculated column values must be -1 because the border is not added to the line)
        // Create and add the address string
        snprintf(string, sizeof(string), "%016" PRIX64, this->instruction_[i].GetAddress());
        snprintf(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), " %s  ", &string[16 - this->editor_->address_width_]);

        // Calculate the maximum number of opcode bytes to display
        const std::size_t opcode_bytes = (this->instruction_[i].GetLength() > this->machine_code_bytes_) ? (this->machine_code_bytes_ - 1) : this->instruction_[i].GetLength();

        // Hex part
        for (std::size_t j = 0; j < opcode_bytes; j++)
        {
            snprintf(string, sizeof(string), "%02" PRIX8 " ", this->instruction_[i].GetMachineCode()[j]);
            strcat_s(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), string);
        }
        // Append dots, if the opcode is longer than displayed
        if (this->instruction_[i].GetLength() > this->machine_code_bytes_)
        {
            strcat_s(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), "...");
        }


        // Must be calculated first, because the string length changes in the loop
        auto length = ((static_cast<std::size_t>(this->asm_column_ - 1) - strlen(this->screen_buffer_[i])));
        // Fill line with blanks (up to asm part)
        for (std::size_t j = 0; j < length; j++)
        {
            strcat_s(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), " ");
        }

        // Disasm part
        snprintf(string, sizeof(string), "%.*s", this->asm_width_, static_cast<const char*>(this->instruction_[i].assembler_code_));
        strcat_s(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), string);

        // Must be calculated first, because the string length changes in the loop
        length = ((static_cast<std::size_t>(this->editor_->width_ - 2) - strlen(this->screen_buffer_[i])));
        // Fill line with blanks
        for (std::size_t j = 0; j < length; j++)
        {
            strcat_s(this->screen_buffer_[i], static_cast<std::size_t>(this->editor_->width_ + 1), " ");
        }

        // Draw line
        this->console_->SetCursor(2, this->editor_->start_line_ + i + 1);
        this->console_->Print(this->screen_buffer_[i]);
    }

    // Process the remaining lines
    for (int32_t i = this->disassembled_lines_; i < this->editor_->lines_; i++)
    {
        // Clear the screen buffer
        memset(this->screen_buffer_[i], 0, static_cast<std::size_t>(this->editor_->width_) + 1);

        // Clear the space that is not occupied by data (only if there is less data than display space)
        this->console_->SetCursor(2, this->editor_->start_line_ + i + 1);
        this->console_->ClearLine();
        this->console_->SetCursor(this->editor_->width_, this->editor_->start_line_ + i + 1);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }

    // Mark the editor as unchanged
    this->ClearChanged();
}

/**
 * Draws the line with the cursor highlighted (and unselects the last line).
 */
void TAsmViewer::UpdateCursor() noexcept
{
    if (this->last_current_line_ != this->current_line_)
    {
        // Clear cursor in old line
        this->console_->SetColor(this->settings_->text_color_);
        this->console_->SetBackground(this->settings_->text_back_color_);
        this->console_->SetCursor(2, this->editor_->start_line_ + this->last_current_line_ + 1);
        this->console_->Print(this->screen_buffer_[this->last_current_line_]);
    }

    // Draw cursor in new line
    this->console_->SetColor(this->settings_->text_back_color_);
    this->console_->SetBackground(this->settings_->text_color_);
    this->console_->SetCursor(2, this->editor_->start_line_ + this->current_line_ + 1);
    this->console_->Print(this->screen_buffer_[this->current_line_]);

    // Set new old line
    this->last_current_line_ = this->current_line_;
}

/**
 * Displays the disassembler bit mode in the status bar.
 */
void TAsmViewer::UpdateStatus() noexcept
{
    this->console_->SetCursor(this->editor_->status_column_, this->editor_->status_line_);
    if (architecture_ == TArchitecture::x86_16)
        this->console_->Print("[ Using x86-16 mode (16bit)  ]");
    else if (architecture_ == TArchitecture::x86_32)
        this->console_->Print("[ Using x86-32 mode (32bit)  ]");
    else if (architecture_ == TArchitecture::x86_64)
        this->console_->Print("[ Using x86-64 mode (64bit)  ]");
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_MIDDLE);
}

/**
 * Moves the cursor one line up.
 */
void TAsmViewer::CursorUp()
{
    if (this->current_line_ > 0)  // Cursor is not on first line
    {
        this->current_line_--;
        return;
    }
    else  // Cursor is on first line
    {
        if ((*this->file_pos_) == 0)  // BOF already reached, do nothing
        {
            return;
        }
        else
        {
            TAsmInstruction temp_instruction[2] = {};

            // Set file pointer back one byte until the second disassembled command equals the current first
            while ((*this->file_pos_) > 0)
            {
                // Step back one byte
                (*this->file_pos_)--;

                // Read binary data for at least two instructions
                this->buffer_->LoadFromFile(this->file_, *this->file_pos_);
                if (this->buffer_->GetCodeLength() == 0) return;

                // Disassemble two instructions
                this->disassembler_->Disassemble(this->buffer_, temp_instruction, 2, this->architecture_, this->settings_->number_format_);

                // Compare the address of the second instruction with the address of the first line currently displayed
                if (this->instruction_[0].GetAddress() == temp_instruction[1].GetAddress()) break;
            }
            this->SetChanged();
        }
    }
}

/**
 * Moves the cursor one line down.
 */
void TAsmViewer::CursorDown() noexcept
{
    // Cursor is not on last line
    if (this->current_line_ < (this->disassembled_lines_ - 1))
    {
        this->current_line_++;
        return;
    }

    // Cursor is on last line (EOF already reached), do nothing
    if (this->disassembled_lines_ < this->editor_->lines_)
    {
        return;
    }

    // Advance file pointer
    const int64_t temp_pos = this->instruction_[disassembled_lines_ - 1].GetAddress() + this->instruction_[disassembled_lines_ - 1].GetLength();

    // Do not scroll if this would go beyond EOF
    if (temp_pos >= this->file_->FileSize()) return;
    (*this->file_pos_) += this->instruction_[0].GetLength();
    this->SetChanged();
}

/**
 * Jumps one page down.
 */
void TAsmViewer::PageDown() noexcept
{
    // Calculate the file address of the byte after the last instruction on the page
    int64_t temp_pos = this->instruction_[this->disassembled_lines_ - 1].GetAddress() + this->instruction_[this->disassembled_lines_ - 1].GetLength();

    // Ensure the position is within the file
    if (temp_pos >= this->file_->FileSize()) return;

    // Jump to the new position
    (*this->file_pos_) = temp_pos;
    this->current_line_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * Moves the file pointer to the beginning of the file.
 */
void TAsmViewer::First() noexcept
{
    (*this->file_pos_) = 0;
    this->current_line_ = 0;

    // Mark the editor as "changed"
    this->SetChanged();
}

/**
 * [Unused]
 * @param character The key code of the key that was pressed.
 * @return true if a change occured that requires the cursor to be updated, false otherwise.
 */
bool TAsmViewer::InsertCharacter(unsigned char character) noexcept
{
    #if defined(WIN32)
        UNREFERENCED_PARAMETER(character);
    #endif
    // Nothing changed
    return false;
}

/**
 * Returns the current absolute file position of the current line (file + cursor line).
 * @return The current absolute file position of the current line (file + cursor line).
 */
int64_t TAsmViewer::CurrentAbsPos() noexcept
{
    // Ensure a valid instruction
    if (this->current_line_ >= this->disassembled_lines_) return 0;

    // Return the address of the current line
    return this->instruction_[this->current_line_].GetAddress();
}

/**
 * Returns the current relative file position (cursor line).
 * @return The current relative file position (cursor line).
 */
int32_t TAsmViewer::CurrentRelPos() noexcept
{
    return this->current_line_;
}

/**
 * Sets the current absolute file position.
 * @param offset The absolute file offset to move the file pointer to.
 */
void TAsmViewer::SetCurrentAbsPos(int64_t offset)
{
    // Store the file pointer
    (*this->file_pos_) = offset;
    this->current_line_ = 0;

    // Load and disassemble from the new position, because if the CurrentAbsPos() is called before the next update (e.g. F5 is pressed), the line addresses would be wrong.
    this->buffer_->LoadFromFile(this->file_, *this->file_pos_);
    if (this->buffer_->GetCodeLength() == 0) return;

    // Disassemble as many lines as there are lines to display (editor lines)
    this->disassembled_lines_ = this->disassembler_->Disassemble(this->buffer_, this->instruction_, this->editor_->lines_, this->architecture_, this->settings_->number_format_);
}

/**
 * Sets the current relative file position (cursor position).
 * @param offset The relative offset to move the cursor to (line).
 */
void TAsmViewer::SetCurrentRelPos(int32_t offset) noexcept
{
    this->current_line_ = offset;
}

/**
 * Toggles the disassembler bit mode (16/32 bit).
 */
void TAsmViewer::EditorFunction1() noexcept
{
    // Toggle the disassembler mode
    if (this->architecture_ == TArchitecture::x86_16)
    {
        this->architecture_ = TArchitecture::x86_32;
    }
    else
    {
        this->architecture_ = TArchitecture::x86_16;
    }

    // Set dirty
    this->SetChanged();
}

/**
 * Shows information about the selected opcode (Description, Platform).
 */
void TAsmViewer::EditorFunction2()
{
    // Create the info window
    auto window = new TWindow(this->console_, -1, -1, 50, 14, "Opcode Info", this->settings_->dialog_color_, this->settings_->dialog_back_color_);

    // Load the instruction (from the current line)
    auto opcode = this->instruction_[this->current_line_].GetOpcode();

    // Check if a valid instruction was loaded
    if (opcode == nullptr)
    {
        this->console_->SetCursor(3, 2);
        this->console_->Print("No valid opcode selected. ");
    }
    else
    {
        // Print opcode description
        this->console_->SetCursor(3, 2);
        this->console_->Print(opcode->comment);
        this->console_->SetCursor(1, 3);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_LEFT_CONNECT);
        this->console_->PrintBorders(HE_CONSOLE_BORDER_MIDDLE_MIDDLE, 48);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_RIGHT_CONNECT);
    }

    this->console_->WaitForKey();

    // Delete the info window
    delete window;
}
