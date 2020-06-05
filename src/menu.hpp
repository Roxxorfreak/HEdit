// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_MENU_HPP_

    // Header included
    #define HEDIT_SRC_MENU_HPP_

    // Constants for the menu class
    constexpr int32_t HE_MENU_MAX_ENTRIES = 30;             //!< The maximum number of entries in one menu
    constexpr int32_t HE_MENU_MAX_VISIBLE_ENTRIES = 12;     //!< The maximum number of entries that are visible in one menu (the rest is scrolled)

    /**
     * @brief The class that provides the popup menus.
     */
    class TMenu
    {
    private:
        TString title_;                                         //!< The title of the popup menu.
        int32_t entries_;                                       //!< The current number of popup menu items.
        TConsole* console_;                                     //!< The console object used for all output.
        TSettings* settings_;                                   //!< The editor settings used for the menu colors.
        TString menu_entry_[HE_MENU_MAX_ENTRIES];               //!< The array for the menu item strings.
        bool menu_entry_active_[HE_MENU_MAX_ENTRIES] = {};      //!< The array of "active" flags for the menu items.
    private:
        int32_t PrepareMenuItems();
        void DrawEntireMenuContent(int32_t visible_entries, int32_t menu_shift) noexcept;
        void DrawMenuItem(int32_t menu_shift, int32_t menu_pos, bool highlight) noexcept;
    public:
        TMenu(TConsole* console, TSettings* settings, const char* title);
        bool AddEntry(const char* menu_item, bool active = true);
        int32_t Show();
    };

#endif  // HEDIT_SRC_MENU_HPP_
