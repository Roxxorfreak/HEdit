// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_CONSOLE_HPP_

    // Header included
    #define HEDIT_SRC_CONSOLE_HPP_

    /**
     * @brief The maximum number of cascaded console windows that can be created using the DefineWindow() function.
     * @details If more windows are created, a std::exception will be thrown.
     */
    constexpr int32_t HE_CONSOLE_MAX_WINDOW_DEPTH = 10;

    // Character definitions
    constexpr unsigned char HE_CONSOLE_CHARACTER_PROGRESS_BAR = 254;  //!< The ASCII code for the character that is used to draw a progress bar.

    // Console key modifiers (modifier bits are in the upper byte of the keycode's word value, i.e. (keycode & 0xff00))
    constexpr int32_t HE_CONSOLE_KEY_MODIFIER_CONTROL   = 0x0100;   //!< A control key was pressed.
    constexpr int32_t HE_CONSOLE_KEY_MODIFIER_CHARACTER = 0x0200;   //!< Not a control key but a character key was pressed.
    constexpr int32_t HE_CONSOLE_KEY_MODIFIER_CTRL      = 0x0400;   //!< CTRL pressed together with the key.
    constexpr int32_t HE_CONSOLE_KEY_MODIFIER_ALT       = 0x0800;   //!< ALT pressed together with the key.
    constexpr int32_t HE_CONSOLE_KEY_MODIFIER_SHIFT     = 0x1000;   //!< SHIFT pressed together with the key.

    // Key constants (The constants are below 20d, so the don't mix with ASCII key codes produced by the keyboard)
    constexpr int32_t HE_CONSOLE_KEY_CODE_NONE          = 0x0000;
    constexpr int32_t HE_CONSOLE_KEY_CODE_ESC           = 0x0001;
    constexpr int32_t HE_CONSOLE_KEY_CODE_RETURN        = 0x0002;
    constexpr int32_t HE_CONSOLE_KEY_CODE_TAB           = 0x0003;
    constexpr int32_t HE_CONSOLE_KEY_CODE_BACKSPACE     = 0x0004;
    constexpr int32_t HE_CONSOLE_KEY_CODE_SPACE         = 0x0005;
    constexpr int32_t HE_CONSOLE_KEY_CODE_CURSOR_UP     = 0x0006;
    constexpr int32_t HE_CONSOLE_KEY_CODE_CURSOR_DOWN   = 0x0007;
    constexpr int32_t HE_CONSOLE_KEY_CODE_CURSOR_RIGHT  = 0x0008;
    constexpr int32_t HE_CONSOLE_KEY_CODE_CURSOR_LEFT   = 0x0009;
    constexpr int32_t HE_CONSOLE_KEY_CODE_PAGE_UP       = 0x000A;
    constexpr int32_t HE_CONSOLE_KEY_CODE_PAGE_DOWN     = 0x000B;
    constexpr int32_t HE_CONSOLE_KEY_CODE_HOME          = 0x000C;
    constexpr int32_t HE_CONSOLE_KEY_CODE_END           = 0x000D;
    constexpr int32_t HE_CONSOLE_KEY_CODE_INSERT        = 0x000E;
    constexpr int32_t HE_CONSOLE_KEY_CODE_DELETE        = 0x000F;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F1            = 0x0010;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F2            = 0x0011;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F3            = 0x0012;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F4            = 0x0013;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F5            = 0x0014;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F6            = 0x0015;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F7            = 0x0016;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F8            = 0x0017;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F9            = 0x0018;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F10           = 0x0019;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F11           = 0x001A;
    constexpr int32_t HE_CONSOLE_KEY_CODE_F12           = 0x001B;

    // Macros to extract key codes and key modifiers
    #define KEYCODE(code)               ((code) & 0x00FF)
    #define IS_CONTROL_KEY(code)        (((code) & HE_CONSOLE_KEY_MODIFIER_CONTROL) == HE_CONSOLE_KEY_MODIFIER_CONTROL)
    #define IS_CHARACTER_KEY(code)      (((code) & HE_CONSOLE_KEY_MODIFIER_CHARACTER) == HE_CONSOLE_KEY_MODIFIER_CHARACTER)
    #define IS_SHIFT_KEY(code)          (((code) & HE_CONSOLE_KEY_MODIFIER_SHIFT) == HE_CONSOLE_KEY_MODIFIER_SHIFT)

    // The console colors. These colors are mapped to the OS-specific color handling.
    enum class TColor : int16_t {
        BLACK = 0,
        GREEN = 1,
        BLUE = 2,
        RED = 3,
        YELLOW = 4,
        WHITE = 5,
        GRAY = 6
    };

    // Platform-specific declarations
    #if defined(WIN32)

        // Define the character codes for the console's PrintBorder(s) function (border characters)
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_LEFT    = 201;
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_MIDDLE  = 205;
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_RIGHT   = 187;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_LEFT   = 186;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_MIDDLE = 205;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_RIGHT  = 186;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_LEFT    = 200;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_MIDDLE  = 205;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_RIGHT   = 188;
        constexpr unsigned char HE_CONSOLE_BORDER_LEFT_CONNECT  = 204;
        constexpr unsigned char HE_CONSOLE_BORDER_RIGHT_CONNECT = 185;

        /**
         * @brief The class that provides the platform-specific console window data structure.
         * @details This contains all data for managing console windows under Windows.
         */
        struct TConsoleWindow
        {
            COORD old_size = {};                        //!< The size of the old window.
            CHAR_INFO* buffer = {};                     //!< The buffer for the character data that used to restore the old window content.
            SMALL_RECT old_rect = {};                   //!< The coordinates (position and size) of the old window.
            SMALL_RECT new_rect = {};                   //!< The coordinates (position and size) of the new window.
            CONSOLE_SCREEN_BUFFER_INFO old_info = {};   //!< The buffer for the character info that used to restore the old window content.
        };

    #else

        // Define the character codes for the console's PrintBorder(s) function (border characters)
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_LEFT    = 108;
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_MIDDLE  = 113;
        constexpr unsigned char HE_CONSOLE_BORDER_UPPER_RIGHT   = 107;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_LEFT   = 120;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_MIDDLE = 113;
        constexpr unsigned char HE_CONSOLE_BORDER_MIDDLE_RIGHT  = 120;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_LEFT    = 109;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_MIDDLE  = 113;
        constexpr unsigned char HE_CONSOLE_BORDER_LOWER_RIGHT   = 106;
        constexpr unsigned char HE_CONSOLE_BORDER_LEFT_CONNECT  = 216;
        constexpr unsigned char HE_CONSOLE_BORDER_RIGHT_CONNECT = 117;

        /**
         * @brief The class that provides the platform-specific console window data structure.
         * @details This contains all data for managing console windows under Linux/Raspian/MacOS.
         */
        struct TConsoleWindow
        {
            int32_t x = {};         //!< The horizontal coordinate of the old windows.
            int32_t y = {};         //!< The vertical coordinate of the old windows.
            int32_t width = {};     //!< The width of the old window.
            int32_t height = {};    //!< The height of the old window.
            WINDOW* handle = {};    //!< The window handle.
            chtype* buffer = {};    //!< The buffer for the character data/info that used to restore the old window content.
        };
    #endif

    /**
     * @brief The class that provides a platform-independent console object.
     * @details This involves all basic window, color, key and directory handling.
     */
    class TConsole
    {
    private:
        int32_t windows_ = {};                                      //!< The current window count. This is the number of windows that are currently open in the console.
        TConsoleWindow window_[HE_CONSOLE_MAX_WINDOW_DEPTH] = {};   //!< The structure containing the data from managing console windows (depending on the current platfrom)
        #if defined(WIN32)
            HANDLE console_handle_ = {};                    //!< The internal console handle.
            TColor old_text_color_ = {};                    //!< The last set text color. Used to prevent changing the color, if not neccessary.
            TColor old_background_color_ = {};              //!< The last set background color. Used to prevent changing the color, if not neccessary.
        #else
            TColor text_color_ = {};                        //!< The last set foregorund color (used to emulate the separate setting of the back and foreground color).
            TColor background_color_ = {};                  //!< The last set background color (used to emulate the separate setting of the back and foreground color).
            int16_t actual_color_pair_ = {};                //!< The currently selected color pair (determined from foreground and background color).
            std::map<unsigned char, int32_t> border_map_;   //!< The map for translating ASCII characters to ACS border characters.
        #endif
    private:
        #if !defined(WIN32)
        int16_t GetSystemColor(TColor color);
        int16_t GetColorPair(TColor text_color, TColor background_color);
        void SetColorInternal();
        static int ScandirSelector(HE_SCANDIR_CONST struct dirent* unused);
        #endif
    public:
        TConsole() noexcept;
        void Init() noexcept;
        void Cleanup() noexcept;
        void SetColor(TColor color) noexcept;
        void SetBackground(TColor color) noexcept;
        void SetCursor(int32_t x, int32_t y) noexcept;
        void ClearLine(void) noexcept;
        void Print(const char* string) noexcept;
        void PrintBorder(unsigned char character) noexcept;
        void PrintBorders(unsigned char character, int32_t count) noexcept;
        void PrintChar(unsigned char char_code) noexcept;
        void PrintFormat(const char* format, ...) noexcept;
        void EnableCursor(void) noexcept;
        void DisableCursor(void) noexcept;
        int32_t WaitForKey() noexcept;
        int32_t Width() noexcept;
        int32_t Height() noexcept;
        bool CheckCancel() noexcept;
        void ClearKeyboardBuffer() noexcept;
        void DefineWindow(int32_t x, int32_t y, int32_t width, int32_t height, TColor text_color, TColor background_color);
        void CloseWindow() noexcept;
        int32_t ListDirectory(const char* directory, const char* file_mask, TStringList* files);
        void GetHomeDirectory(TString& path) noexcept;
        void Refresh() noexcept;
    };

#endif  // HEDIT_SRC_CONSOLE_HPP_
