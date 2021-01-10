// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new menu.
 * @param console The console to draw the menu in.
 * @param settings The settings object (used for getting the colors).
 * @param title The title for the menu.
 */
TMenu::TMenu(TConsole* console, TSettings* settings, const char* title)
{
    this->entries_ = 0;
    this->console_ = console;
    this->settings_ = settings;
    this->title_ = title;
}

/**
 * Adds a new entry to menu.
 * @param menu_item The title for the menu item.
 * @param active The flag that specified if the menu item is available (true, default) or grayed our (false).
 * @return true on success, false otherwise.
 */
bool TMenu::AddEntry(const char* menu_item, bool active)
{
    // Ensure the maximum number of entries is not exceeded
    if (this->entries_ >= HE_MENU_MAX_ENTRIES) return false;

    // Calculate the maximum menu item width
    const auto max_width = static_cast<std::size_t>(this->console_->Width());

    // Copy the menu item text
    this->menu_entry_[this->entries_] = menu_item;

    // Copy the menu item string, depending on the length
    if (this->menu_entry_[this->entries_].Length() >= max_width - 13)
    {
        // The menu item is larger than allowed, truncate the first part of the text and append an ellipse
        this->menu_entry_[this->entries_][max_width - 13] = 0;
        this->menu_entry_[this->entries_] += "...";
    }

    // Store the "active" status for the menu item entry
    this->menu_entry_active_[this->entries_] = active;

    // Count the menu item
    entries_++;

    // Return success
    return true;
}

/**
 * Displays the menu, waits for the user selection and returns the 1-based number of the selected menu item.
 * @return The 1-based menu item number on success, or 0 on cancel or failure.
 */
int32_t TMenu::Show()
{
    // Calculate the number of entries that are visible
    const auto visible_entries = hedit_min(this->entries_, HE_MENU_MAX_VISIBLE_ENTRIES);

    // Calculate the width of the menu items
    const auto width = this->PrepareMenuItems();

    // The number of visible entries + upper and lower border
    const auto height = visible_entries + 2;

    // Draw window
    auto window = new TWindow(this->console_, -1, -1, width + 2, height, this->title_, this->settings_->dialog_color_, this->settings_->dialog_back_color_);

    // Draw entire menu
    this->DrawEntireMenuContent(visible_entries, 0);

    // Disable cursor
    this->console_->DisableCursor();

    int32_t key_code = 0;
    int32_t menu_pos = 0;
    int32_t menu_shift = 0;
    do
    {
        const auto old_menu_pos = menu_pos;
        const auto old_menu_shift = menu_shift;

        // Draw the highlighted menu item
        this->DrawMenuItem(menu_shift, menu_pos, true);

        // Wait for the user
        key_code = this->console_->WaitForKey();

        // Evaluate the pressed key
        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_UP))  // Cursor Up
        {
            if ((menu_shift > 0) && (menu_pos - menu_shift == 0))
            {
                menu_pos--;
                menu_shift--;
            }
            else
            {
                auto temp_menu_pos = menu_pos - 1;
                while (temp_menu_pos > 0)
                {
                    if (this->menu_entry_active_[temp_menu_pos]) break;
                    temp_menu_pos--;
                }
                if (temp_menu_pos >= 0)
                {
                    menu_pos = temp_menu_pos;
                }
                else
                {
                    menu_pos = entries_ - 1;
                }
            }
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_CURSOR_DOWN))  // Cursor Down
        {
            auto temp_menu_pos = menu_pos + 1;
            while (temp_menu_pos < entries_ - 1)
            {
                if (this->menu_entry_active_[temp_menu_pos]) break;
                temp_menu_pos++;
            }
            if (temp_menu_pos < entries_)
            {
                menu_pos = temp_menu_pos;
            }
            else
            {
                menu_pos = 0;
            }
        }

        if ((IS_CONTROL_KEY(key_code)) && (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_HOME))  // Pos1
        {
            menu_pos = 0;
            menu_shift = 0;
        }

        if ((menu_pos - menu_shift < 0) || (menu_pos - menu_shift >= visible_entries))
        {
            if (menu_pos < visible_entries)
                menu_shift = 0;
            else
                menu_shift = menu_pos - visible_entries + 1;
        }

        // If the menu shift has changed
        if (old_menu_shift != menu_shift)
        {
            // Redraw complete menu
            this->DrawEntireMenuContent(visible_entries, menu_shift);
        }
        else  // otherwise
        {
            // Un-highlight old menu item
            this->DrawMenuItem(old_menu_shift, old_menu_pos, false);
        }
    } while ((KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_ESC) && (KEYCODE(key_code) != HE_CONSOLE_KEY_CODE_RETURN));

    // Enable cursor
    this->console_->EnableCursor();

    // Delete the window object
    delete window;

    return (KEYCODE(key_code) == HE_CONSOLE_KEY_CODE_ESC) ? 0 : menu_pos + 1;
}

/**
 * Prepares all added menu items for being displayed.
 * This function is called internally just before the menu is made visible.
 * @return The width that is used for all menu items (the inner width of the menu).
 */
int32_t TMenu::PrepareMenuItems()
{
    uint32_t width = 0;
    TString temp_entry;

    // Process all entries and find the largest entry
    for (int32_t i = 0; i < this->entries_; i++)
    {
        if (this->menu_entry_[i].Length() > width) width = this->menu_entry_[i].Length();
    }

    // Add 4 character to the width (making 2 left and 2 right of the item)
    width += 4;

    // Process all entries and reformat them
    for (int32_t i = 0; i < this->entries_; i++)
    {
        // Calculate the space
        const uint32_t space = width - this->menu_entry_[i].Length();
        const auto half_space = static_cast<int32_t>(space >> 1);

        // Create the new item
        temp_entry.New(width);

        // Add the left spaces
        for (int32_t k = 0; k < half_space; k++) temp_entry += " ";

        // Append the item text
        temp_entry += this->menu_entry_[i];

        // Append the right spaces
        for (int32_t k = 0; k < half_space; k++) temp_entry += " ";

        // Append one space for odd-lengthed menu items
        if ((space % 2) != 0) temp_entry += " ";

        // Copy back to menu item
        this->menu_entry_[i] = temp_entry;
    }

    // Return the width
    return static_cast<int32_t>(width);
}

/**
 * Draws the entire visible menu content in its current state (selected, enabled, grayed, etc.).
 * @param visible_entries The number of visible menu items.
 * @param menu_shift The number of menu items shifted out of view.
 */
void TMenu::DrawEntireMenuContent(int32_t visible_entries, int32_t menu_shift) noexcept
{
    // Set the background color
    this->console_->SetBackground(this->settings_->dialog_back_color_);

    // Process all visible menu items
    for (int32_t i = 0; i < visible_entries; i++)
    {
        const auto index = menu_shift + i;

        if (this->menu_entry_active_[index])
            this->console_->SetColor(this->settings_->dialog_color_);
        else
            this->console_->SetColor(this->settings_->inactive_text_color_);

        this->console_->SetCursor(2, i + 2);
        this->console_->Print(this->menu_entry_[index].ToString());
    }

    // Refreseh the console
    this->console_->Refresh();
}

/**
 * Draws one menu item in its current state (selected, enabled, grayed, etc.).
 * @param menu_shift The number of menu items shifted out of view.
 * @param menu_pos The index of the menu item to draw.
 * @param highlight true to draw the item highlighted, false otherwise.
 */
void TMenu::DrawMenuItem(int32_t menu_shift, int32_t menu_pos, bool highlight) noexcept
{
    // Select color
    if (highlight == true)
    {
        this->console_->SetColor(this->settings_->dialog_back_color_);
        this->console_->SetBackground(this->settings_->dialog_color_);
    }
    else
    {
        this->console_->SetColor(this->settings_->dialog_color_);
        this->console_->SetBackground(this->settings_->dialog_back_color_);
    }

    // Draw the item
    this->console_->SetCursor(2, menu_pos - menu_shift + 2);
    this->console_->Print(this->menu_entry_[menu_pos].ToString());
    this->console_->Refresh();
}
