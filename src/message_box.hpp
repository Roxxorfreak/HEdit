// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_MESSAGE_BOX_HPP_

    // Header included
    #define HEDIT_SRC_MESSAGE_BOX_HPP_

    // Message box types
    enum class TMessageBoxType : int32_t {
        INFO,   //!< The message box only has an "Ok" button.
        YESNO   //!< The message box has a "Yes" and a "No" button.
    };

    /**
     * @brief The class that provides the message boxes.
     */
    class TMessageBox
    {
    private:
        TColor text_color_;             //!< The text color for the message box.
        TColor back_color_;             //!< The background color for the message box.
        TString title_;                 //!< The title of the message box.
        TConsole* console_;             //!< The console object used for all output.
    private:
        void DrawFrame(int32_t x, int32_t y, uint32_t width, uint32_t height, TColor text_color, TColor back_color) noexcept;
    public:
        TMessageBox(TConsole* console, const char* title, TColor text_color, TColor back_color);
        bool Display(TMessageBoxType type, TString text1, TString text2 = TString());
        bool GetString(TString message, TString* input_text, uint32_t max_input_length, bool hex_string);
    };

#endif  // HEDIT_SRC_MESSAGE_BOX_HPP_
