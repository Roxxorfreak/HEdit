// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new config file instance and assigns a file name.
 * @param file_name The name of the file to manage.
 */
TConfigFile::TConfigFile(const char* file_name) : TFile(file_name, true)
{
}

/**
 * Reads the next configuration entry (key and value) - if any - from the config file.
 * A configuration entry has the form "key=value" on a single line.
 * Empty lines and comments are ignored. Comments are indicated by hash key (#), semi-colon (;) or double slash (//).
 * The config file must have been opened before calling this function.
 * @param entry A pointer to a structure that receives the entry data (key and value).
 * @return true, if an entry was read, false otherwise.
 */
bool TConfigFile::ReadNextEntry(TConfigEntry* entry)
{
    // Validate entry pointer
    if (entry == nullptr) return false;

    // Read the file line by line
    do
    {
        // Read the next line
        auto line = this->ReadLine();

        // Skip empty lines
        if (line.Length() == 0) continue;

        // Removed leading and trailing spaces
        line.Trim();

        // Skip comments (starting with ; or # or //)
        if (line.Equals(";", 1)) continue;
        if (line.Equals("#", 1)) continue;
        if (line.Equals("//", 2)) continue;

        // A config line was found, split into key and value
        auto divider_pos = line.IndexOf('=');

        // If no divider (=) is found ignore the line
        if (divider_pos < 0) continue;

        // Split into key and value
        auto key = line.SubString(0, divider_pos);
        auto value = line.SubString(static_cast<std::size_t>(divider_pos) + 1, -1);

        // Assign key and value
        entry->key = key.Trim();
        entry->value = value.Trim();

        // Successfully read config entry
        return true;

    } while (!this->IsEOF());

    // No entry was found
    return false;
}

/**
 * Write a newline to the specified (text) file.
 */
void TConfigFile::WriteNewline() noexcept
{
    // Write newline string
    this->Write(reinterpret_cast<const unsigned char*>(HE_LINE_BREAK_STRING), strlen(HE_LINE_BREAK_STRING));
}

/**
 * Writes a single line to the config file. The appropriate newline character(s) are appended.
 * @param file The file to write to.
 * @param format The string to write to the file.
 * @param ... additional arguments
 */
#if !defined(_MSC_VER)
__attribute__((format(printf, 2, 3)))
#endif
void TConfigFile::WriteConfigLine(const char* format, ...) noexcept
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
    this->Write(reinterpret_cast<const unsigned char*>(string), strlen(string));

    // Append newline
    this->Write(reinterpret_cast<const unsigned char*>(HE_LINE_BREAK_STRING), strlen(HE_LINE_BREAK_STRING));

    // End the argument processing
    va_end(arguments);
}
