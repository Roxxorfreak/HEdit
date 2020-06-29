// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Loads the settings from the config file.
 * @param config_dir The directory to store the config file (usually the user's home directory). Must end with a path delimiter (HE_PATH_DELIMITER).
 */
TSettings::TSettings(const char* config_dir)
{
    // Store the config dir
    this->config_dir_ = config_dir;

    // Store the config file name and path for later use
    this->config_file_ = TString(config_dir) + TString(HE_CONFIG_FILE);

    // Load the default settings
    this->LoadDefaultSettings();

    // Load the settings from the config file (if any)
    this->LoadSettings();
}

/**
 * Saves the settings to the config file.
 */
TSettings::~TSettings() noexcept(false)
{
    // Save the settings
    this->SaveSettings();
}

/**
 * Initializes all settings with their default values
 */
void TSettings::LoadDefaultSettings()
{
    // Assign the default values
    this->temp_file_persistent_     = false;
    this->probable_word_length_     = 4;
    this->number_format_            = TNumberFormat::HEX;
    this->undo_steps_               = 20;
    this->temp_file_name_           = "hedit.tmp";
    this->probable_word_char_set_   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    this->use_caching_              = true;
    this->plugin_file_              = "numeric.hs";

    // Create default path for script files (~\.hedit-scripts by default)
    this->plugin_path_ = this->config_dir_;
    this->plugin_path_ += ".hedit-scripts";
    this->plugin_path_ += HE_PATH_DELIMITER;

    // Default colors
    this->text_color_               = TColor::WHITE;
    this->text_back_color_          = TColor::BLUE;
    this->inactive_text_color_      = TColor::GRAY;
    this->active_file_color_        = TColor::BLUE;
    this->active_file_back_color_   = TColor::GREEN;
    this->marked_text_color_        = TColor::WHITE;
    this->marked_text_back_color_   = TColor::BLACK;
    this->dialog_color_             = TColor::WHITE;
    this->dialog_back_color_        = TColor::BLACK;
    this->difference_color_[0]      = TColor::WHITE;
    this->difference_back_color_[0] = TColor::GREEN;
    this->difference_color_[1]      = TColor::WHITE;
    this->difference_back_color_[1] = TColor::RED;
    this->difference_color_[2]      = TColor::WHITE;
    this->difference_back_color_[2] = TColor::YELLOW;
    this->difference_color_[3]      = TColor::WHITE;
    this->difference_back_color_[3] = TColor::GRAY;
    this->difference_color_[4]      = TColor::WHITE;
    this->difference_back_color_[4] = TColor::GREEN;

    // Default values for the volatile (temporary) settings
    this->compare_mode_     = false;
    this->cursor_lock_mode_ = false;
    this->marker_mode_      = false;
}

/**
 * Reads a color value from the specified line, if it contains the specified color name.
 * @param entry The config entry (containing "<color name>=<color value>").
 * @param color_name The color name to read (e.g. "TextColor").
 * @param color_value A pointer to an integer that receives the color value (TConsole color code).
 */
void TSettings::GetColor(const TConfigEntry& entry, const char* color_name, TColor* color_value) noexcept
{
    // Validate pointers
    if (color_name == nullptr) return;
    if (color_value == nullptr) return;

    // Extract color, if color name was found on the line
    if (entry.key.EqualsCI(color_name) == true) (*color_value) = this->GetColorId(entry.value);
}

/**
 * Write a newline to the specified (text) file.
 * @param file The file to write to.
 */
void TSettings::WriteNewline(TConfigFile* file) noexcept
{
    // Write newline string
    file->Write(reinterpret_cast<const unsigned char*>(HE_LINE_BREAK_STRING), strlen(HE_LINE_BREAK_STRING));
}

/**
 * Writes a single line to the config file. The appropriate newline character(s) are appended.
 * @param file The file to write to.
 * @param format The string to write to the file.
 * @param ... additional arguments
 */
#if !defined(_MSC_VER)
__attribute__((format(printf, 3, 4)))
#endif
void TSettings::WriteConfigLine(TConfigFile* file, const char* format, ...) noexcept
{
    va_list arguments;
    char string[128] = {};

    // Validate input
    if (format == nullptr) return;

    // Get the start address of the variable arguments
    va_start(arguments, format);

    // Create the string
    vsnprintf(string, sizeof(string), format, arguments);

    // Write the string to file
    file->Write(reinterpret_cast<const unsigned char*>(string), strlen(string));

    // Append newline
    file->Write(reinterpret_cast<const unsigned char*>(HE_LINE_BREAK_STRING), strlen(HE_LINE_BREAK_STRING));

    // End the argument processing
    va_end(arguments);
}

/**
 * Loads the settings from the specified config file, on failure the defaults are loaded
 * @return true on success, false otherwise
 */
bool TSettings::LoadSettings()
{
    TConfigEntry entry;

    // Create the config file object
    auto file = new TConfigFile(this->config_file_);

    // Open the config file
    if (!file->Open(TFileMode::READ))
    {
        // Free the file object
        delete file;

        // Return failure
        return false;
    }

    // Read the file line by line
    while (file->ReadNextEntry(&entry) == true)
    {
        // The colors
        this->GetColor(entry, "textcolor", &this->text_color_);
        this->GetColor(entry, "inactivetextcolor", &this->inactive_text_color_);
        this->GetColor(entry, "textbackgroundcolor", &this->text_back_color_);
        this->GetColor(entry, "activefilecolor", &this->active_file_color_);
        this->GetColor(entry, "activefilebackgroundcolor", &this->active_file_back_color_);
        this->GetColor(entry, "markedtextcolor", &this->marked_text_color_);
        this->GetColor(entry, "markedtextbackgroundcolor", &this->marked_text_back_color_);
        this->GetColor(entry, "differencecolor1", &this->difference_color_[0]);
        this->GetColor(entry, "differencebackgroundcolor1", &this->difference_back_color_[0]);
        this->GetColor(entry, "differencecolor2", &this->difference_color_[1]);
        this->GetColor(entry, "differencebackgroundcolor2", &this->difference_back_color_[1]);
        this->GetColor(entry, "differencecolor3", &this->difference_color_[2]);
        this->GetColor(entry, "differencebackgroundcolor3", &this->difference_back_color_[2]);
        this->GetColor(entry, "differencecolor4", &this->difference_color_[3]);
        this->GetColor(entry, "differencebackgroundcolor4", &this->difference_back_color_[3]);
        this->GetColor(entry, "differencecolor5", &this->difference_color_[4]);
        this->GetColor(entry, "differencebackgroundcolor5", &this->difference_back_color_[4]);
        this->GetColor(entry, "dialogcolor", &this->dialog_color_);
        this->GetColor(entry, "dialogbackgroundcolor", &this->dialog_back_color_);

        // The settings for word detection
        if (entry.key.EqualsCI("MinimumLength")) this->probable_word_length_ = static_cast<int32_t>(entry.value.ParseDec());
        if (entry.key.EqualsCI("CharSet")) this->probable_word_char_set_ = entry.value;

        // The temporary file name
        if (entry.key.EqualsCI("TempFile")) this->temp_file_name_ = entry.value;

        // Plugin path
        if (entry.key.EqualsCI("PluginPath")) this->plugin_path_ = entry.value;

        // The Plugin for Shift-F4
        if (entry.key.EqualsCI("PluginName")) this->plugin_file_ = entry.value;

        // The cache persistency setting
        if (entry.key.EqualsCI("Persistent"))
        {
            if (entry.value.EqualsCI("yes") == true)
            {
                this->temp_file_persistent_ = true;
            }
            else
            {
                this->temp_file_persistent_ = false;
            }
        }

        // The cache setting
        if (entry.key.EqualsCI("Caching"))
        {
            if (entry.value.EqualsCI("yes") == true)
            {
                this->use_caching_ = true;
            }
            else
            {
                this->use_caching_ = false;
            }
        }

        // The numeric format
        if (entry.key.EqualsCI("NumericFormat", 13))
        {
            if (entry.value.EqualsCI("dec") == true) this->number_format_ = TNumberFormat::DEC;
            if (entry.value.EqualsCI("hex") == true) this->number_format_ = TNumberFormat::HEX;
            if (entry.value.EqualsCI("bin") == true) this->number_format_ = TNumberFormat::BIN;
        }

        // The number of undo steps
        if (entry.key.EqualsCI("Steps", 5)) this->undo_steps_ = static_cast<int32_t>(entry.value.ParseDec());

    }

    // Append trailing (back)slash to plugin path if not present
    if (!this->plugin_path_.EndsWith(HE_PATH_DELIMITER)) this->plugin_path_ += HE_PATH_DELIMITER;

    // Delete the file object
    delete file;

    // Return success
    return true;
}

/**
 * Writes the settings to the specified config file, on failure does nothing.
 * @return true on success, false otherwise.
 */
bool TSettings::SaveSettings()
{
    // Create the config file object
    TConfigFile file(this->config_file_);

    // Try to open the config file
    if (file.Open(TFileMode::CREATE) == false)
    {
        // Return failure
        return false;
    }

    // Header
    this->WriteConfigLine(&file, "; This is the config file for %s", HE_PROGRAM_TITLE);

    // Caching
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; Specifies the use of caching (default is on)");
    if (this->use_caching_ == true)
        this->WriteConfigLine(&file, "Caching = yes");
    else
        this->WriteConfigLine(&file, "Caching = no");

    // Temp File
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; Specifies the name of the temp file for copy/paste operations");
    this->WriteConfigLine(&file, "TempFile = %s", this->temp_file_name_.ToString());
    if (this->temp_file_persistent_ == true)
        this->WriteConfigLine(&file, "Persistent = yes");
    else
        this->WriteConfigLine(&file, "Persistent = no");

    // Probable word
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; Options for the search mode \"Search probable word\"");
    this->WriteConfigLine(&file, "MinimumLength = %" PRIi32, this->probable_word_length_);
    this->WriteConfigLine(&file, "CharSet = %s", this->probable_word_char_set_.ToString());

    // Disassembler
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; Specifies the numeric format for the disassembler (HEX or DEC)");
    if (this->number_format_ == TNumberFormat::HEX) this->WriteConfigLine(&file, "NumericFormat = HEX");
    if (this->number_format_ == TNumberFormat::DEC) this->WriteConfigLine(&file, "NumericFormat = DEC");
    if (this->number_format_ == TNumberFormat::BIN) this->WriteConfigLine(&file, "NumericFormat = BIN");

    // Colors
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; The colors (Possible values: black, white, blue, green, gray, red, yellow)");
    this->WriteConfigLine(&file, "TextColor = %s", this->GetColorName(this->text_color_));
    this->WriteConfigLine(&file, "InactiveTextColor = %s", this->GetColorName(this->inactive_text_color_));
    this->WriteConfigLine(&file, "TextBackgroundColor = %s", this->GetColorName(this->text_back_color_));
    this->WriteConfigLine(&file, "ActiveFileColor = %s", this->GetColorName(this->active_file_color_));
    this->WriteConfigLine(&file, "ActiveFileBackgroundColor = %s", this->GetColorName(this->active_file_back_color_));
    this->WriteConfigLine(&file, "MarkedTextColor = %s", this->GetColorName(this->marked_text_color_));
    this->WriteConfigLine(&file, "MarkedTextBackgroundColor = %s", this->GetColorName(this->marked_text_back_color_));
    for (int32_t i = 0; i < 5; i++)
    {
        this->WriteConfigLine(&file, "DifferenceColor%i = %s", i + 1, this->GetColorName(this->difference_color_[i]));
        this->WriteConfigLine(&file, "DifferenceBackgroundColor%i = %s", i + 1, this->GetColorName(this->difference_back_color_[i]));
    }
    this->WriteConfigLine(&file, "DialogColor = %s", this->GetColorName(this->dialog_color_));
    this->WriteConfigLine(&file, "DialogBackgroundColor = %s", this->GetColorName(this->dialog_back_color_));

    // Undo
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; Specify the number of undo steps stored in memory");
    this->WriteConfigLine(&file, "Steps = %" PRIi32, this->undo_steps_);

    // Plugins
    this->WriteNewline(&file);
    this->WriteConfigLine(&file, "; The path to look for the plugins");
    this->WriteConfigLine(&file, "PluginPath = %s", this->plugin_path_.ToString());
    this->WriteConfigLine(&file, "; Plugin to use for Shift-F4");
    this->WriteConfigLine(&file, "PluginName = %s", this->plugin_file_.ToString());

    // Return success
    return true;
}

/**
 * Toggles the CursorLock setting
 */
void TSettings::ToggleCursorLockMode() noexcept
{
    if (this->cursor_lock_mode_ == true)
    {
        this->cursor_lock_mode_ = false;
    }
    else
    {
        this->cursor_lock_mode_ = true;
    }
}

/**
 * Toggles the CompareMode setting
 */
void TSettings::ToggleCompareMode() noexcept
{
    if (this->compare_mode_ == true)
    {
        this->compare_mode_ = false;
    }
    else
    {
        this->compare_mode_ = true;
    }
}

/**
 * Toggles the MarkerMode setting
 */
void TSettings::ToggleMarkerMode() noexcept
{
    if (this->marker_mode_ == true)
    {
        this->marker_mode_ = false;
    }
    else
    {
        this->marker_mode_ = true;
    }
}

/**
 * Evaluates a color string and translates into the color id
 * @param color_name The color name
 * @return The color id
 */
TColor TSettings::GetColorId(const char* color_name) noexcept
{
    // Validate color
    if (color_name == nullptr) return TColor::BLACK;

    if (_stricmp(color_name, "black") == 0)  return TColor::BLACK;
    if (_stricmp(color_name, "blue") == 0)   return TColor::BLUE;
    if (_stricmp(color_name, "red") == 0)    return TColor::RED;
    if (_stricmp(color_name, "green") == 0)  return TColor::GREEN;
    if (_stricmp(color_name, "yellow") == 0) return TColor::YELLOW;
    if (_stricmp(color_name, "gray") == 0)   return TColor::GRAY;
    if (_stricmp(color_name, "white") == 0)  return TColor::WHITE;
    return TColor::BLACK;
}

/**
 * Evaluates a color id and translates into the color string
 * @param color_id the color id
 * @return the readable name of the color
 */
const char* TSettings::GetColorName(TColor color_id) noexcept
{
    if (color_id == TColor::BLACK)   return reinterpret_cast<const char*>("black");
    if (color_id == TColor::WHITE)   return reinterpret_cast<const char*>("white");
    if (color_id == TColor::GREEN)   return reinterpret_cast<const char*>("green");
    if (color_id == TColor::YELLOW)  return reinterpret_cast<const char*>("yellow");
    if (color_id == TColor::GRAY)    return reinterpret_cast<const char*>("gray");
    if (color_id == TColor::BLUE)    return reinterpret_cast<const char*>("blue");
    if (color_id == TColor::RED)     return reinterpret_cast<const char*>("red");
    return reinterpret_cast<const char*>("black");
}
