// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Opens a new text window.
 * @param console The console object used for all output.
 * @param x The x coordinate of the window (-1 to center).
 * @param y The y coordinate of the window (-1 to center).
 * @param width The width of the window.
 * @param height The height of the window.
 * @param title The title of the window or nullptr for no title.
 * @param text_color The foreground (text) color of the window.
 * @param back_color The background color of the window.
 */
TWindow::TWindow(TConsole* console, int32_t x, int32_t y, int32_t width, int32_t height, const char* title, TColor text_color, TColor back_color)
{
    // Store parameters
    this->console_ = console;
    this->x_ = x;
    this->y_ = y;
    this->width_ = width;
    this->height_ = height;
    this->title_ = title;
    this->text_color_ = text_color;
    this->back_color_ = back_color;

    // Open the window
    this->Open();
}

/**
 * Closes the window.
 */
TWindow::~TWindow()
{
    this->Close();
}

/**
 * Saves the the area occupied by the window and displays the new window.
 */
void TWindow::Open()
{
    // Validate window dimensions
    if (this->width_ > this->console_->Width()) this->width_ = this->console_->Width();
    if (this->height_ > this->console_->Height()) this->height_ = this->console_->Height();

    // Calculate the position of the window, if not specified
    if (this->x_ == -1) this->x_ = ((this->console_->Width() - this->width_) / 2) + 1;  // Center horizontally
    if (this->y_ == -1) this->y_ = ((this->console_->Height() - this->height_) / 2) + 1;  // Center vertically

    // Prepare the window area
    this->console_->DefineWindow(x_, y_, this->width_, this->height_, text_color_, back_color_);
    this->console_->SetColor(text_color_);
    this->console_->SetBackground(back_color_);

    // Head line
    this->console_->SetCursor(1, 1);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_UPPER_LEFT);
    this->console_->PrintBorders(HE_CONSOLE_BORDER_UPPER_MIDDLE, this->width_ - 2);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_UPPER_RIGHT);

    // Intermediate lines
    for (int32_t i = 2; i < this->height_; i++)
    {
        this->console_->SetCursor(1, i);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_LEFT);
        this->console_->PrintBorders(' ', this->width_ - 2);
        this->console_->PrintBorder(HE_CONSOLE_BORDER_MIDDLE_RIGHT);
    }

    // Bottom line
    this->console_->SetCursor(1, this->height_);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_LEFT);
    this->console_->PrintBorders(HE_CONSOLE_BORDER_LOWER_MIDDLE, this->width_ - 2);
    this->console_->PrintBorder(HE_CONSOLE_BORDER_LOWER_RIGHT);

    // Check if a window title was specified
    if (title_.Length() == 0) return;

    // Get maximum title length
    const int32_t max_length = width_ - 8;

    // Validate allowed length (window may be too small for title)
    if (max_length <= 0) return;

    // Check if the window title is too long
    if (static_cast<int32_t>(title_.Length()) > max_length)
    {
        // Too long, remove the rest
        this->title_.RemoveRight(title_.Length() - static_cast<std::size_t>(max_length));
    }

    TString title_full = TString("[ ");
    title_full += this->title_;
    title_full += " ]";

    // Draw title
    this->console_->SetColor(text_color_);
    this->console_->SetBackground(back_color_);
    this->console_->SetCursor(2 + ((width_ - 1 - static_cast<int32_t>(title_full.Length())) / 2), 1);
    this->console_->Print(title_full);
}

/**
 * Closes the window and restores the old background.
 */
void TWindow::Close() noexcept
{
    this->console_->CloseWindow();
    this->console_->Refresh();
}
