// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new message box.
 * To display the message box, either Display() or GetString() must be called.
 * @param console The console object used for all output.
 * @param title The title of the message box.
 * @param text_color The text color for the message box.
 * @param back_color The background color for the message box.
 */
TMessageBox::TMessageBox(TConsole* console, const char* title, TColor text_color, TColor back_color)
{
    // Initialize all data
    this->console_ = console;
    this->title_ = title;
    this->text_color_ = text_color;
    this->back_color_ = back_color;
}

/**
 * Displays an output-only message box.
 * @param type The type of message box:<br/>
 * TMessageBoxType::INFO: Returns false on failure, true otherwise.<br/>
 * TMessageBoxType::YESNO: Returns true for "Yes", false for "No".
 * @param text1 The first line of text for the message box.
 * @param text2 The second line of text for the message box.
 * @return true on success (yes), false otherwise (no).
 */
bool TMessageBox::Display(TMessageBoxType type, TString text1, TString text2)
{
    int32_t key_code = 0;
    uint32_t width = 0;

    // Calculate MessageBox width (Title + 4. because there are brackets added)
    width = 0;
    if ((this->title_.Length() + 4) > width) width = this->title_.Length() + 4;
    if (text1.Length() > width) width = text1.Length();
    if (text2.Length() > width) width = text2.Length();
    width += 4;

    // Create and display dialog
    auto message_box = new TWindow(this->console_, -1, -1, static_cast<int32_t>(width), 9, this->title_, this->text_color_, this->back_color_);

    this->console_->SetCursor((static_cast<int32_t>(width - text1.Length()) / 2) + 1, 3);
    this->console_->Print(text1);
    this->console_->SetCursor((static_cast<int32_t>(width - text2.Length()) / 2) + 1, 4);
    this->console_->Print(text2);

    // Draw the corresponding buttons
    switch (type)
    {
        case TMessageBoxType::INFO:
            this->DrawFrame((static_cast<int32_t>(width - 8) / 2) + 1, 6, 8, 3, this->text_color_, this->back_color_);
            this->console_->SetCursor((static_cast<int32_t>(width - 2) / 2) + 1, 7);
            this->console_->Print("Ok");
            break;
        case TMessageBoxType::YESNO:
            this->DrawFrame((static_cast<int32_t>(width - 18) / 2) + 1, 6, 8, 3, this->text_color_, this->back_color_);
            this->DrawFrame(static_cast<int32_t>(width / 2) + 2, 6, 8, 3, this->text_color_, this->back_color_);
            this->console_->SetCursor((static_cast<int32_t>(width - 14) / 2) + 1, 7);
            this->console_->Print("Yes");
            this->console_->SetCursor(static_cast<int32_t>(width / 2) + 5, 7);
            this->console_->Print("No");
            break;
    }

    // Wait for valid key (Enter, ESC, y, Y, n, N)
    this->console_->DisableCursor();
    this->console_->Refresh();
    do
    {
        key_code = this->console_->WaitForKey();

        if ((KEYCODE(key_code) == 'Y') || (KEYCODE(key_code) == 'y')) key_code = HE_CONSOLE_KEY_CODE_RETURN;
        if ((KEYCODE(key_code) == 'N') || (KEYCODE(key_code) == 'n')) key_code = HE_CONSOLE_KEY_CODE_ESC;

    } while ((KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_ESC) && (KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_RETURN));

    // Delete the window object
    delete message_box;

    return (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_ESC) ? false : true;
}

/**
 * Displays an input box for a text or hex string.
 * @param message The text that is displyed left of the input.
 * @param input_text A pointer to the string that receives the input data.
 * @param max_input_length The maximum allowed input text length.
 * @param hex_string true to allow only hex characters (0-9, A-F), false otherwise.
 * @return true on success, false on failure or cancel.
 */
bool TMessageBox::GetString(TString message, TString* input_text, uint32_t max_input_length, bool hex_string)
{
    int32_t key_code = 0;

    // Validate buffers
    if (input_text == nullptr) return false;

    // Calculate the maximum size for the input area (console width - main window borders - input label - spaces and border)
    const auto max_size = (static_cast<uint32_t>(this->console_->Width()) - 2 - message.Length() - 5);

    // Fix max input length, if needed
    max_input_length = hedit_min(max_input_length, max_size);

    // Determine the width of the input dialog (left label + maximum input size + spaces and border)
    const auto dialog_width = static_cast<int32_t>(message.Length() + max_input_length + 5);

    // Create and display the dialog
    auto input_dialog = new TWindow(this->console_, -1, -1, dialog_width, 3, this->title_, this->text_color_, this->back_color_);

    // Calculate the start of the input
    const auto start_pos = static_cast<int32_t>(message.Length() + 4);

    this->console_->SetCursor(3, 2);
    this->console_->Print(message);
    this->console_->SetCursor(start_pos, 2);
    this->console_->Refresh();

    // Clear the target buffer
    input_text->New(max_input_length, 0);

    do
    {
        // Store the current length of the string, it's used often
        const auto input_length = input_text->Length();

        // Update cursor position
        this->console_->SetCursor(start_pos + static_cast<int32_t>(input_length), 2);

        // Read the next key
        key_code = this->console_->WaitForKey();

        // Evaluate the key code
        switch (KEYCODE(key_code))
        {
            case HE_CONSOLE_KEY_CODE_BACKSPACE:  // Backspace
                if (IS_CONTROL_KEY(key_code))
                {
                    // Check if the string contains data
                    if (input_length > 0) input_text->operator[](input_length - 1) = 0;
                }
                break;
            case 65:  // A - F
            case 66:
            case 67:
            case 68:
            case 69:
            case 70:
            case 97:  // a - f
            case 98:
            case 99:
            case 100:
            case 101:
            case 102:
            case 48:  // 0 - 9
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
                if (IS_CHARACTER_KEY(key_code))
                {
                    if (input_length >= max_input_length) break;
                    input_text->operator+=(static_cast<char>(key_code));
                    input_text->operator+=(static_cast<char>(0));
                }
                break;
            default:  // DEC Only characters
                if (IS_CHARACTER_KEY(key_code))
                {
                    if (hex_string == true) break;
                    if (input_length >= max_input_length) break;
                    input_text->operator+=(static_cast<char>(key_code));
                    input_text->operator+=(static_cast<char>(0));
                }
                break;
        }

        // Print the updated string
        this->console_->SetCursor(start_pos, 2);
        this->console_->Print(input_text->operator const char *());

        // Clear to end of line
        this->console_->ClearLine();
        this->console_->SetCursor(dialog_width, 2);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);

    } while ((KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_RETURN) && (KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_ESC));

    // If nothing was entered, assume the input was cancelled
    if (input_text->Length() == 0) key_code = HE_CONSOLE_KEY_CODE_ESC;

    // Delete the input dialog
    delete input_dialog;

    // Return true on success or false if the input was cancelled
    return (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_ESC) ? false : true;
}

/**
 * Draws the frames for the buttons.
 * @param x The horizontal position of the frame.
 * @param y The vertical position of the frame.
 * @param width The width of the frame.
 * @param height The height of the frame.
 * @param text_color The foreground color for the frame.
 * @param back_color The background color for the frame.
 */
void TMessageBox::DrawFrame(int32_t x, int32_t y, uint32_t width, uint32_t height, TColor text_color, TColor back_color) noexcept
{
    // Set the frame colors
    this->console_->SetColor(text_color);
    this->console_->SetBackground(back_color);

    this->console_->SetCursor(x, y);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_UPPER_LEFT);
    this->console_->PrintBorders(HE_CONSOLE_BORDER_UPPER_MIDDLE, static_cast<int32_t>(width) - 2);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_UPPER_RIGHT);
    for (uint32_t i = 1; i < height - 1; i++)
    {
        this->console_->SetCursor(x, y + static_cast<int32_t>(i));
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_LEFT);
        this->console_->PrintBorders(' ', static_cast<int32_t>(width) - 2);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }
    this->console_->SetCursor(x, y + static_cast<int32_t>(height) - 1);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_LEFT);
    this->console_->PrintBorders(HE_CONSOLE_BORDER_UPPER_MIDDLE, static_cast<int32_t>(width) - 2);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_RIGHT);
}
