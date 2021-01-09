// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Initializes the console.
 */
TConsole::TConsole() noexcept
    : windows_(0)
{
    #if defined(WIN32)
        // Under Windows, the initial screen is NOT a window, treat it so anyway
        this->windows_ = 1;

        // Get the console handle
        this->console_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(this->console_handle_, &this->window_[0].old_info);
        this->window_[0].new_rect.Left = 1;
        this->window_[0].new_rect.Top = 1;
        this->window_[0].new_rect.Right = this->window_[0].old_info.dwSize.X;
        this->window_[0].new_rect.Bottom = this->window_[0].old_info.dwSize.Y;
    #else
        // Do not accept Escape Sequences
        ESCDELAY = 0;

        // Under Linux the initial screen IS a window
        this->windows_ = 1;
        this->window_[0].handle = stdscr;

        // No color pairs defined so far
        this->actual_color_pair_ = 0;
    #endif
}

/**
 * Inits the console.
 */
void TConsole::Init() noexcept
{
    #if defined(WIN32)
        SetConsoleTitle(HE_PROGRAM_TITLE);
    #else
        // Initialize the ncurses library
        ::initscr();
        // Raw character processing
        ::raw();
        // Don't echo characters to the screen
        ::noecho();
        // Enable function key processing
        ::keypad(this->window_[0].handle, TRUE);
        // Don't wait for ESC sequences
        ::notimeout(this->window_[0].handle, FALSE);
        // Wait for key on wgetch
        nodelay(this->window_[0].handle, FALSE);
        // Initialize the color handling
        ::start_color();
        // Initialize the colors
        this->text_color_ = TColor::WHITE;
        this->background_color_ = TColor::BLUE;

        // Init the map with special characters
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_UPPER_LEFT, ACS_ULCORNER));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_UPPER_MIDDLE, ACS_HLINE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_UPPER_RIGHT, ACS_URCORNER));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_MIDDLE_LEFT, ACS_VLINE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_MIDDLE_MIDDLE, ACS_HLINE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_MIDDLE_RIGHT, ACS_VLINE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_LOWER_LEFT, ACS_LLCORNER));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_LOWER_MIDDLE, ACS_HLINE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_LOWER_RIGHT, ACS_LRCORNER));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_LEFT_CONNECT, ACS_LTEE));
        border_map_.insert(std::make_pair(HE_CONSOLE_BORDER_RIGHT_CONNECT, ACS_RTEE));
        border_map_.insert(std::make_pair(HE_CONSOLE_CHARACTER_PROGRESS_BAR, ACS_BLOCK));
    #endif
}

/**
 * Cleans up the console.
 */
void TConsole::Cleanup() noexcept
{
    #if defined(WIN32)
        DWORD count = 0;

        // Restore the old console attributes (saved at start-up)
        SetConsoleTextAttribute(this->console_handle_, this->window_[0].old_info.wAttributes);

        // Fill the console with spaces
        // (see https://docs.microsoft.com/en-us/windows/console/fillconsoleoutputcharacter)
        FillConsoleOutputCharacter(this->console_handle_, ' ', static_cast<DWORD>(this->window_[0].old_info.dwSize.X * this->window_[0].old_info.dwSize.Y), { 0 , 0 }, & count);

        // Position the cursor in the upper left corner
        SetConsoleCursorPosition(this->console_handle_, { 0, 0 });

    #else
        // Exit Curses mode
        ::endwin();
    #endif
}

/**
 * Sets the foreground color for text in the console.
 * @param color The new foreground color.
 */
void TConsole::SetColor(TColor color) noexcept
{
    #if defined(WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi = {};

        // End here, if the color did not change
        if (this->old_text_color_ == color) return;

        // Read the old colors
        GetConsoleScreenBufferInfo(this->console_handle_, &csbi);

        // Keep only background color
        csbi.wAttributes &= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);

        // Assign new front color
        if (color == TColor::BLACK)  csbi.wAttributes |= 0;
        if (color == TColor::RED)    csbi.wAttributes |= (FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (color == TColor::GREEN)  csbi.wAttributes |= (FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (color == TColor::BLUE)   csbi.wAttributes |= (FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        if (color == TColor::YELLOW) csbi.wAttributes |= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (color == TColor::GRAY)   csbi.wAttributes |= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        if (color == TColor::WHITE)  csbi.wAttributes |= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // Set the color
        SetConsoleTextAttribute(this->console_handle_, csbi.wAttributes);

        // Store current color
        this->old_text_color_ = color;
    #else
        this->text_color_ = color;
    #endif
}

/**
 * Sets the background color for text in the console.
 * @param color The new background color.
 */
void TConsole::SetBackground(TColor color) noexcept
{
    #if defined(WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi = {};

        // End here, if the color did not change
        if (this->old_background_color_ == color) return;

        // Read the old colors
        GetConsoleScreenBufferInfo(this->console_handle_, &csbi);

        // Keep only background color
        csbi.wAttributes &= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // Assign new front color
        if (color == TColor::BLACK)  csbi.wAttributes |= 0;
        if (color == TColor::RED)    csbi.wAttributes |= (BACKGROUND_RED);
        if (color == TColor::GREEN)  csbi.wAttributes |= (BACKGROUND_GREEN);
        if (color == TColor::BLUE)   csbi.wAttributes |= (BACKGROUND_BLUE );
        if (color == TColor::YELLOW) csbi.wAttributes |= (BACKGROUND_RED | BACKGROUND_GREEN);
        if (color == TColor::GRAY)   csbi.wAttributes |= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
        if (color == TColor::WHITE)  csbi.wAttributes |= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);

        // Set the color
        SetConsoleTextAttribute(this->console_handle_, csbi.wAttributes);

        // Store current color
        this->old_background_color_ = color;

    #else
        this->background_color_ = color;
    #endif
}

/**
 * Sets the cursor position in the console.
 * @param x The new x (horizontal) position (1-based).
 * @param y The new y (vertical) position (1-based).
 */
void TConsole::SetCursor(int32_t x, int32_t y) noexcept
{
    #if defined(WIN32)
        COORD cp;
        cp.X = static_cast<int16_t>(x) - 2 + this->window_[this->windows_ - 1].new_rect.Left;
        cp.Y = static_cast<int16_t>(y) - 2 + this->window_[this->windows_ - 1].new_rect.Top;
        SetConsoleCursorPosition(this->console_handle_, cp);
    #else
        ::wmove(this->window_[this->windows_ - 1].handle, y - 1, x - 1);
    #endif
}

/**
 * Clears all text till the end of the line.
 */
void TConsole::ClearLine(void) noexcept
{
#if defined(WIN32)
    DWORD characters_written = {};
    CONSOLE_SCREEN_BUFFER_INFO csbi = {};
    GetConsoleScreenBufferInfo(this->console_handle_, &csbi);
    int32_t count = (this->window_[this->windows_ - 1].new_rect.Left + this->window_[this->windows_ - 1].new_rect.Right) - csbi.dwCursorPosition.X - 1;
    if (count < 0) count = 0;
    FillConsoleOutputCharacter(this->console_handle_, ' ', static_cast<DWORD>(count), csbi.dwCursorPosition, &characters_written);
    FillConsoleOutputAttribute(this->console_handle_, csbi.wAttributes, static_cast<DWORD>(count), csbi.dwCursorPosition, &characters_written);
#else
    ::wclrtoeol(this->window_[this->windows_ - 1].handle);
#endif
}

/**
 * Prints the specified output at the current position in the current window.
 * @param string The string for the output.
 */
void TConsole::Print(const char* string) noexcept
{
    // Validate input
    if (string == nullptr) return;

#if defined(WIN32)
    // Output the string
    fputs(string, stdout);
#else
    // Set the output color
    this->SetColorInternal();
    waddstr(this->window_[this->windows_ - 1].handle, string);
#endif
}

/**
 * Prints a single border character at the current cursor position.
 * @param character The character to print.
 */
void TConsole::PrintBorder(unsigned char character) noexcept
{
#if defined(WIN32)
    fputc(character, stdout);
#else
    this->SetColorInternal();
    auto entry = this->border_map_.find(character);
    if (entry != border_map_.end()) {
        waddch(this->window_[this->windows_ - 1].handle, entry->second);
    } else {
        waddch(this->window_[this->windows_ - 1].handle, character);
    }
#endif
}

/**
 * Repeats a single border character at the current cursor position the specified number of times.
 * @param character The character to print.
 * @param count The number of characters to print (repetition).
 */
void TConsole::PrintBorders(unsigned char character, int32_t count) noexcept
{
    #if !defined(WIN32)
        chtype real_char;
        this->SetColorInternal();
        auto entry = this->border_map_.find(character);
        if (entry != border_map_.end()) {
            real_char = entry->second;
        } else {
            real_char = character;
        }
    #endif
    for (int32_t i = 0; i < count; i++)
    {
        #if defined(WIN32)
            fputc(character, stdout);
        #else
            waddch(this->window_[this->windows_ - 1].handle, real_char);
        #endif
    }
}

/**
 * Outputs a character to the console, verifying that the character will be
 * displayed correctly or replaced by a suitable substitute.
 * @param char_code The character to output.
 */
void TConsole::PrintChar(unsigned char char_code) noexcept
{
    #if defined(WIN32)
        if ((char_code == 0) || (char_code == 7) || (char_code == 8) || (char_code == 9) || (char_code == 13) || (char_code == 10))
            fprintf(stdout, ".");
        else
            fprintf(stdout, "%c", char_code);
    #else
        this->SetColorInternal();
        if ((char_code >= 127) || (char_code < 32))
        {
            if (char_code == 0)
                ::wprintw(this->window_[this->windows_ - 1].handle, ".");
            else
                ::wprintw(this->window_[this->windows_ - 1].handle, "^");
        }
        else
        {
            ::wprintw(this->window_[this->windows_ - 1].handle, "%c", char_code);
        }
    #endif
}

/**
 * Prints the specified (formatted) output at the current position in the current window.
 * @param format The format string for the output.
 * @param ... additional arguments
 */
#if !defined(_MSC_VER)
__attribute__((format(printf, 2, 3)))
#endif
void TConsole::PrintFormat(const char* format, ...) noexcept
{
    va_list arguments;

    // Validate input
    if (format == nullptr) return;

    // Get the start address of the variable arguments
    va_start(arguments, format);

    // Output the string
    #if defined(WIN32)
        vfprintf_s(stdout, format, arguments);
    #else
        this->SetColorInternal();
        ::vw_printw(this->window_[this->windows_ - 1].handle, format, arguments);
    #endif

    // End the argument processing
    va_end(arguments);
}

/**
 * Shows the cursor.
 */
void TConsole::EnableCursor() noexcept
{
    #if defined(WIN32)
        CONSOLE_CURSOR_INFO cci;
        GetConsoleCursorInfo(this->console_handle_, &cci);
        cci.bVisible = true;
        SetConsoleCursorInfo(this->console_handle_, &cci);
    #else
        ::curs_set(1);
    #endif
}

/**
 * Hides the cursor.
 */
void TConsole::DisableCursor() noexcept
{
    #if defined(WIN32)
        CONSOLE_CURSOR_INFO cci;
        GetConsoleCursorInfo(this->console_handle_, &cci);
        cci.bVisible = false;
        SetConsoleCursorInfo(this->console_handle_, &cci);
    #else
        ::curs_set(0);
    #endif
}

/**
 * Waits for a key to be pressed and returns the code of the key pressed.
 * @return The key code (translated into HEDit key codes)
 */
int32_t TConsole::WaitForKey() noexcept
{
    int32_t key_code = 0;
    #if defined(WIN32)
        int32_t key[20] = {};

        int32_t index = 0;
        key[0] = 0;
        key[1] = 0;
        do
        {
            key[index % 20] = _getch();
            index++;
        } while (_kbhit());

        // Evaluate the first key code: 0 or 224 (0xE0) indicate a special key.
        // See: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getch-getwch
        if ((key[0] != 0) && (key[0] != 224))  // Characters keys
        {
            // Assume it's an character key
            key_code = HE_CONSOLE_KEY_MODIFIER_CHARACTER | KEYCODE(key[0]);
            // Process exceptions
            if (key[0] == 0x08) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_BACKSPACE;
            if (key[0] == 0x09) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_TAB;
            if (key[0] == 0x0D) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_RETURN;
            if (key[0] == 0x1B) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_ESC;
        }
        else  // Control keys
        {
            key_code = HE_CONSOLE_KEY_CODE_NONE;

            if (key[1] == 59)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F1;
            if (key[1] == 60)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F2;
            if (key[1] == 61)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F3;
            if (key[1] == 62)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F4;
            if (key[1] == 63)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F5;
            if (key[1] == 64)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F6;
            if (key[1] == 65)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F7;
            if (key[1] == 66)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F8;
            if (key[1] == 67)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F9;
            if (key[1] == 68)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F10;
            if (key[1] == 84)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F1;
            if (key[1] == 85)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F2;
            if (key[1] == 86)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F3;
            if (key[1] == 87)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F4;
            if (key[1] == 88)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F5;
            if (key[1] == 89)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F6;
            if (key[1] == 90)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F7;
            if (key[1] == 91)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F8;
            if (key[1] == 92)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F9;
            if (key[1] == 93)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F10;
            if (key[1] == 72)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_UP;
            if (key[1] == 80)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_DOWN;
            if (key[1] == 75)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_LEFT;
            if (key[1] == 77)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_RIGHT;
            if (key[1] == 81)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_PAGE_DOWN;
            if (key[1] == 73)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_PAGE_UP;
            if (key[1] == 71)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_HOME;
            if (key[1] == 79)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_END;
            if (key[1] == 82)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_INSERT;
            if (key[1] == 83)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_DELETE;
        }

    #else
        int32_t key;
        key = ::wgetch(this->window_[this->windows_ - 1].handle);

        if (key < 256)  // Not a NCURSES translated control key
        {
            // Assume it's an character key
            key_code = HE_CONSOLE_KEY_MODIFIER_CHARACTER | KEYCODE(key);
            // Process exceptions
            if (key == 0x09) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_TAB;
            if (key == 0x0A) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_RETURN;
            if (key == 0x1B) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_ESC;
        }
        else  // NCURSES Key, report as HE_CONSOLE_KEY_CODE_NONE if not known specifically
        {
            key_code = HE_CONSOLE_KEY_CODE_NONE;

            if (key == KEY_BACKSPACE) key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_BACKSPACE;
            if (key == KEY_F(1))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F1;
            if (key == KEY_F(2))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F2;
            if (key == KEY_F(3))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F3;
            if (key == KEY_F(4))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F4;
            if (key == KEY_F(5))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F5;
            if (key == KEY_F(6))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F6;
            if (key == KEY_F(7))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F7;
            if (key == KEY_F(8))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F8;
            if (key == KEY_F(9))      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F9;
            if (key == KEY_F(10))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_F10;
            if (key == KEY_F(11))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F1;
            if (key == KEY_F(12))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F2;
            if (key == KEY_F(13))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F3;
            if (key == KEY_F(14))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F4;
            if (key == KEY_F(15))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F5;
            if (key == KEY_F(16))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F6;
            if (key == KEY_F(17))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F7;
            if (key == KEY_F(18))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F8;
            if (key == KEY_F(19))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F9;
            if (key == KEY_F(20))     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_MODIFIER_SHIFT | HE_CONSOLE_KEY_CODE_F10;
            if (key == KEY_UP)        key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_UP;
            if (key == KEY_DOWN)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_DOWN;
            if (key == KEY_LEFT)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_LEFT;
            if (key == KEY_RIGHT)     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_CURSOR_RIGHT;
            if (key == KEY_NPAGE)     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_PAGE_DOWN;
            if (key == KEY_PPAGE)     key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_PAGE_UP;
            if (key == KEY_IC)        key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_INSERT;
            if (key == KEY_DC)        key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_DELETE;
            // Home and End may be mapped to either HOME and END or FIND and SELECT
            if (key == KEY_FIND)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_HOME;
            if (key == KEY_SELECT)    key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_END;
            if (key == KEY_HOME)      key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_HOME;
            if (key == KEY_END)       key_code = HE_CONSOLE_KEY_MODIFIER_CONTROL | HE_CONSOLE_KEY_CODE_END;
        }
    #endif

    return key_code;
}

/**
 * Returns the width of the console in characters.
 * @return The width of the console.
 */
int32_t TConsole::Width() noexcept
{
    #if defined(WIN32)
        // Get console width and height
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(this->console_handle_, &csbi);
        return static_cast<int32_t>(csbi.srWindow.Right - csbi.srWindow.Left) + 1;
    #else
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        rows++;  // Prevent "unused" warning
        return static_cast<int32_t>(cols);
    #endif
}

/**
 * Returns the height of the console in characters.
 * @return The height of the console.
 */
int32_t TConsole::Height() noexcept
{
    #if defined(WIN32)
        // Get screen width and height
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(this->console_handle_, &csbi);
        return static_cast<int32_t>(csbi.srWindow.Bottom - csbi.srWindow.Top) + 1;
    #else
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        return static_cast<int32_t>(rows);
    #endif
}

/**
 * Checks, if the ESC key was pressed (does not wait for a key to be pressed).
 * @return true if ESC was pressed, false otherwise.
 */
bool TConsole::CheckCancel() noexcept
{
    #if defined(WIN32)
        while (_kbhit())
        {
            if (_getch() == 27) return true;
        }
        return false;
    #else
        int key_code;
        // Don't wait for key on wgetch
        ::nodelay(this->window_[this->windows_ - 1].handle, TRUE);
        // Query key
        key_code = ::wgetch(this->window_[this->windows_ - 1].handle);
        // Wait for key on wgetch
        ::nodelay(this->window_[this->windows_ - 1].handle, FALSE);
        if (key_code == KEY_EXIT) return true;
        return false;
    #endif
}

/**
 * Clears the keyboard buffer.
 */
void TConsole::ClearKeyboardBuffer() noexcept
{
    #if defined(WIN32)
        while (_kbhit()) static_cast<void>(_getch());
    #else
        ::flushinp();
    #endif
}

/**
 * Defines a new text window, saving background and old window settings.
 * @param x The x position of the window.
 * @param y The y position of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param text_color The foreground color of the window.
 * @param background_color The background color of the window.
 */
void TConsole::DefineWindow(int32_t x, int32_t y, int32_t width, int32_t height, TColor text_color, TColor background_color)
{
    // Ensure there are not too many windows
    if (this->windows_ >= HE_CONSOLE_MAX_WINDOW_DEPTH) throw std::runtime_error("Too many windows");

    #if defined (_WIN32)
        COORD pos = {};
        UNREFERENCED_PARAMETER(text_color);
        UNREFERENCED_PARAMETER(background_color);

        // Backup current screen
        GetConsoleScreenBufferInfo(this->console_handle_, &this->window_[this->windows_].old_info);
        this->window_[this->windows_].buffer = new CHAR_INFO[static_cast<unsigned int>((this->window_[this->windows_].old_info.dwSize.X * this->window_[this->windows_].old_info.dwSize.Y))];

        pos.X = 0;
        pos.Y = 0;
        this->window_[this->windows_].old_size.X = static_cast<SHORT>(width);
        this->window_[this->windows_].old_size.Y = static_cast<SHORT>(height);
        this->window_[this->windows_].old_rect.Left = static_cast<SHORT>(x - 1);
        this->window_[this->windows_].old_rect.Top = static_cast<SHORT>(y - 1);
        this->window_[this->windows_].old_rect.Right = this->window_[this->windows_].old_rect.Left + static_cast<SHORT>(width);
        this->window_[this->windows_].old_rect.Bottom = this->window_[this->windows_].old_rect.Top + static_cast<SHORT>(height);

        if (ReadConsoleOutput(this->console_handle_, this->window_[this->windows_].buffer, this->window_[this->windows_].old_size, pos,  &window_[this->windows_].old_rect) == 0)
        {
            delete[] this->window_[this->windows_].buffer;
            throw std::runtime_error("Window dimensions exceed maximum");
        }

        // Define the new window region
        this->window_[this->windows_].new_rect.Left = static_cast<SHORT>(x);
        this->window_[this->windows_].new_rect.Top = static_cast<SHORT>(y);
        this->window_[this->windows_].new_rect.Right = static_cast<SHORT>(width);
        this->window_[this->windows_].new_rect.Bottom = static_cast<SHORT>(height);
        this->windows_++;
    #else
        int color_pair;
        // Shorten access
        const auto win = &this->window_[this->windows_ - 1];
        // Store window dimensions
        win->x = x - 1;
        win->y = y - 1;
        win->width = width;
        win->height = height;
        // Allocate memory for the screen backup
        win->buffer = new chtype[(width + 1) * height];
        // Backup the screen data
        for (int32_t i = 0; i < win->height; i++)
        {
            wmove(win->handle, win->y + i, win->x);
            winchnstr(win->handle, &win->buffer[(win->width + 1) * i], win->width);
        }

        // Create the new window and set it's background color and pattern
        this->window_[this->windows_].handle = newwin(height, width, y - 1, x - 1);
        if (this->window_[this->windows_].handle == nullptr) throw std::runtime_error("Window creation failed (TConsole)");
        color_pair = this->GetColorPair(text_color, background_color);
        wbkgd(this->window_[this->windows_].handle, 32 | COLOR_PAIR(color_pair));
        // Enable function key processing
        keypad(this->window_[this->windows_].handle, TRUE);
        // Don't wait for ESC sequences
        notimeout(this->window_[this->windows_].handle, FALSE);
        // Wait for key on wgetch
        nodelay(this->window_[this->windows_].handle, FALSE);
        // Increase window counter
        this->windows_++;
    #endif
}

/**
 * Closes the topmost text window, restoring background and old window settings.
 */
void TConsole::CloseWindow() noexcept
{
    // Ignore the function call, if no windows are open
    if (this->windows_ == 0) return;

    #if defined(WIN32)
        COORD pos = {};
        this->windows_--;
        // Restore old cursor position
        SetConsoleCursorPosition(this->console_handle_, this->window_[this->windows_].old_info.dwCursorPosition);
        // Restore the old window state and region
        pos.X = 0;
        pos.Y = 0;
        WriteConsoleOutput(this->console_handle_, this->window_[this->windows_].buffer, this->window_[this->windows_].old_size, pos, &this->window_[this->windows_].old_rect);
        delete[] this->window_[this->windows_].buffer;
    #else
        // Decrease the counter
        this->windows_--;
        // Delete the current window
        delwin(this->window_[this->windows_].handle);
        // Restore old window, only if not the main window
        if (this->windows_ > 1)
        {
            // Improve access
            const auto win = &this->window_[this->windows_ - 1];
            // Redraw old window
            for (int32_t i = 0; i < win->height; i++)
            {
                wmove(win->handle, win->y + i, win->x);
                waddchnstr(win->handle, &win->buffer[(win->width + 1) * i], win->width);
            }
            // Free memory
            delete[] this->window_[this->windows_ - 1].buffer;
        }
    #endif
}

#if !defined(WIN32)

/**
 * Translates the HEdit colors into ncurses color constants.
 * @param color The color to translate.
 * @return The ncurses color value.
 */
int16_t TConsole::GetSystemColor(TColor color)
{
    switch (color)
    {
    case TColor::BLACK:
        return COLOR_BLACK;
    case TColor::GREEN:
        return COLOR_GREEN;
    case TColor::BLUE:
        return COLOR_BLUE;
    case TColor::RED:
        return COLOR_RED;
    case TColor::YELLOW:
        return COLOR_YELLOW;
    case TColor::WHITE:
        return COLOR_WHITE;
    case TColor::GRAY:
        return COLOR_WHITE;
    }
    return COLOR_BLACK;
}

/**
 * NCurses accepts only color pairs. Since the functions used
 * througout the entire editor always specify EITHER background OR foreground
 * color, this function takes care of a growing set of color pairs and
 * selects the appropriate one, if it already exists otherwise creates
 * a new pair (Linux only).
 * @param text_color The HEdit color constant for the foreground color.
 * @param background_color The HEdit color constant for the background color.
 * @return The color pair identifier.
 */
int16_t TConsole::GetColorPair(TColor text_color, TColor background_color)
{
    int16_t color_pair;
    int16_t color1, color2;

    // No color pair found yet, use the default
    color_pair = 0;

    // Check all color pairs
    for (int32_t i = 1; i < this->actual_color_pair_; i++)
    {
        pair_content(i, &color1, &color2);
        if ((this->GetSystemColor(text_color) == color1) && (this->GetSystemColor(background_color) == color2)) color_pair = i;
    }
    // No color pair yet defined for the desired combination
    // of colors, try to define one and use that
    if (color_pair == 0)
    {
        // Maximun number of color pairs reached?
        if (this->actual_color_pair_ < COLOR_PAIRS - 1)  // No, create a new one
        {
            this->actual_color_pair_++;
            init_pair(this->actual_color_pair_, this->GetSystemColor(text_color), this->GetSystemColor(background_color));
            color_pair = this->actual_color_pair_;
        }
        else  // Yes, use default color pair
        {
            color_pair = 0;
        }
    }

    // Return the color pair id
    return color_pair;
}

/**
 * Actually sets the color for subsequent output (Linux ONLY)
 */
void TConsole::SetColorInternal()
{
    auto color_pair = this->GetColorPair(this->text_color_, this->background_color_);
    // Set the color and attribute of the subsequent output
    wattron(this->window_[this->windows_ - 1].handle, COLOR_PAIR(color_pair));
    if ((this->text_color_ == TColor::RED) || (this->text_color_ == TColor::WHITE))
        wattron(this->window_[this->windows_ - 1].handle, A_BOLD);
    else
        wattroff(this->window_[this->windows_ - 1].handle, A_BOLD);
}
#endif

/**
 * Fills the contents of the specified directory matching the specified mask into an array.
 * @param directory The directory to list.
 * @param file_mask The file mask.
 * @param files The list for the files to return.
 * @return The number of files found.
 */
int32_t TConsole::ListDirectory(const char* directory, const char* file_mask, TStringList* files)
{
    // No files found yet
    int32_t file_count = 0;

    // Validate files list pointer
    if (files == nullptr) return 0;

    #if defined(WIN32)
        TString search_path;
        struct _finddata_t found_file;

        // Create the full path
        search_path = directory;
        search_path += file_mask;

        // Search for files
        const auto search_handle = _findfirst(search_path, &found_file);
        if (search_handle >= 0)
        {
            do
            {
                files->push_back(TString(found_file.name));
                file_count++;
            } while (_findnext(search_handle, &found_file) == 0);

            // Close the search
            _findclose(search_handle);
        }
    #else
        struct dirent **entry;
        // Scan the specified directory
        file_count = scandir(directory, &entry, ScandirSelector, alphasort);
        // If no files were found, cancel here
        if (file_count < 0) return 0;
        // Copy the file names
        for (int32_t i = 0; i < file_count; i++)
        {
            // Copy the file name
            files->push_back(TString(entry[i]->d_name));
            // Free the entry
            free(entry[i]);
        }
        // Free the file list
        free(entry);
    #endif

    // Return the number of files
    return file_count;
}

#if !defined(WIN32)
/**
 * The callback selector function for the linux scandir function, 
 * used in the ListDirectory() function (Linux only).
 * @param entry The directory entry (file)
 * @return 1 if the file should be added to the list, 0 otherwise
 */
int TConsole::ScandirSelector(HE_SCANDIR_CONST struct dirent *entry)
{
    // Return regular files
    if (entry->d_type == DT_REG) return 1;
    // Return links
    if (entry->d_type == DT_LNK) return 1;
    // Deny all other files types
    return 0;
}
#endif

/**
 * Returns the user's home directory.
 * If the user's home directory cannot be read, the current work directory is returned.
 * The path always ends with a path delimiter (HE_PATH_DELIMITER_STRING).
 * @param path The string variable to receive the full qualified path of the home directory.
 */
void TConsole::GetHomeDirectory(TString& path) noexcept
{
    #if defined(WIN32)
        char user_folder[MAX_PATH];

        // Clear the target variable
        memset(user_folder, 0, sizeof(user_folder));

        // Try to read out the path to the user folder, typically "C:\Users\username"
        if (SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, SHGFP_TYPE_CURRENT, user_folder) != S_OK)
        {
            // If the path to user folder could not be read, use the current path
            strcpy_s(user_folder, MAX_PATH, ".");
        }

    #else
        // In a linux environment retrieve the user's home directory
        char user_folder[256];
        auto current_user_id = getuid();
        auto user_data = getpwuid(current_user_id);
        if (user_data == nullptr)
        {
            // No user data available, try current working dir
            if (getcwd(user_folder, sizeof(user_folder)) == nullptr)
            {
                // No current working directory, use current directory
                strcpy_s(user_folder, 256, "./");
            }
        }
        else
        {
            // No home directory avaliable, use current working dir
            if (user_data->pw_dir == nullptr)
            {
                if (getcwd(user_folder, sizeof(user_folder)) == nullptr)
                {
                    // No current working directory, use current directory
                    strcpy_s(user_folder, 256, "./");
                }
            }
            else
            {
                // Success: Query the user's home dir
                strcpy_s(user_folder, 256, user_data->pw_dir);
            }
        }
    #endif

    // Store the path
    path = user_folder;

    // Append trailing (back)slash if not present
    if (!path.EndsWith(HE_PATH_DELIMITER)) path += HE_PATH_DELIMITER;
}

/**
 * Refreshes the currently active window
 */
void TConsole::Refresh() noexcept
{
    #if !defined(WIN32)
        ::wrefresh(this->window_[this->windows_ - 1].handle);
    #endif
}
