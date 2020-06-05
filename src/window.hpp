// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_WINDOW_HPP_

    // Header included
    #define HEDIT_SRC_WINDOW_HPP_

    /**
     * @brief The class that provides the windows in the console.
     */
    class TWindow
    {
    private:
        int32_t x_;             //!< The horizontal position of the window. If set to -1 the windows is centered.
        int32_t y_;             //!< The vertical position of the window. If set to -1 the windows is centered.
        TColor text_color_;     //!< The text color.
        TColor back_color_;     //!< The background color.
        int32_t width_;         //!< The width of the window (in characters).
        int32_t height_;        //!< The height of the window (in characters).
        TString title_;         //!< The title of the window.
        TConsole* console_;     //!< The console object used for all output.
    private:
        void Open();
        void Close() noexcept;
    public:
        TWindow(TConsole* console, int32_t x, int32_t y, int32_t width, int32_t height, const char* title, TColor text_color, TColor back_color);
        TWindow(const TWindow&) = delete;
        TWindow& operator=(const TWindow&) = delete;
        TWindow(TWindow&&) = delete;
        TWindow& operator=(TWindow&&) = delete;
        ~TWindow();
    };

#endif  // HEDIT_SRC_WINDOW_HPP_
