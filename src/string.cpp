// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new empty string object.
 */
TString::TString() noexcept
{
    // Reset data
    this->buffer_ = nullptr;
    this->buffer_size_ = 0;
}

/**
 * Move constructor.
 * @param source_string The source string to move.
 */
TString::TString(TString&& source_string) noexcept : TString(std::exchange(source_string.buffer_, nullptr))
{
}

/**
 * Creates a new string object from the specified string object.
 * @param source_string The string object to use to initialize the new string object.
 */
TString::TString(const TString& source_string) : TString(source_string.buffer_)
{
}

/**
 * Creates a new string object with a specified size and data.
 * @param size The initial size of the new string (excluding the binary zero).
 * @param filler The character to use to initialize the string with.
 */
TString::TString(std::size_t size, char filler)
{
    // Reset data
    this->buffer_ = nullptr;
    this->buffer_size_ = 0;

    // Ensure valid input
    if (size > HE_STRING_MAX_SIZE) throw std::invalid_argument("String too long");

    // Create the new string
    this->buffer_ = new char[size + 1];

    // Inititalize the memory with zeroes
    memset(this->buffer_, 0, size + 1);

    // Inititalize the memory with the filling char
    memset(this->buffer_, filler, size);

    // Remember the string length (including the binary zero)
    this->buffer_size_ = size + 1;
}

/**
 * Creates a new string object from the specified character string.
 * @param source_string The character string to use to initialize the new string object.
 */
TString::TString(const char* source_string)
{
    // Reset data
    this->buffer_ = nullptr;
    this->buffer_size_ = 0;

    // Assign the string
    this->operator =(source_string);
}

/**
 * Frees the internal string memory.
 */
TString::~TString()
{
    this->Free();
}

/**
 * Frees the memory used for the string.
 */
void TString::Free() noexcept
{
    // Free memory
    if (this->buffer_ != nullptr) delete[] this->buffer_;

    // Clear the data
    this->buffer_ = nullptr;
    this->buffer_size_ = 0;
}

/**
 * Move assignment.
 * @param source_string The character string to move.
 */
TString& TString::operator=(TString&& source_string) noexcept
{
    // move string
    std::swap(this->buffer_, source_string.buffer_);

    // Return this reference
    return *this;
}

/**
 * Provides the assignment operator for character strings.
 * @param source_string The character string to assign.
 * @return A reference to the string object.
 */
TString& TString::operator=(const char* source_string)
{
    // Free any previous string buffer
    this->Free();

    // Validate source string
    if (source_string == nullptr) return *this;

    // Ensure valid input
    if (strlen(source_string) > HE_STRING_MAX_SIZE) throw std::invalid_argument("String too long");

    // Determine the size of the buffer for the string data (+ terminating 0)
    this->buffer_size_ = strlen(source_string) + 1;

    // Allocate the buffer
    this->buffer_ = new char[this->buffer_size_];

    // Clear the buffer
    memset(this->buffer_, 0, this->buffer_size_);

    // Copy the data
    strncpy_s(this->buffer_, this->buffer_size_, source_string, this->buffer_size_ - 1);

    // Return this reference
    return *this;
}

/**
 * Provides the assignment operator for TString strings.
 * @param source_string The string object to assign.
 * @return A reference to the string object.
 */
TString& TString::operator=(const TString& source_string)
{
    // Check for self-assignment
    if (this == &source_string) return *this;

    // Assign the character string
    return this->operator=(source_string.buffer_);
}

/**
 * Provides the compound assignment operator for character strings.
 * @param character The character to assign.
 * @return A reference to the string object.
 */
TString& TString::operator+=(const char character)
{
    char temp_string[2];

    // Create a temporary string
    temp_string[0] = character;
    temp_string[1] = 0;

    // Append the character
    return this->operator+=(temp_string);
}

/**
 * Provides the compound assignment operator for character strings.
 * @param source_string The character string to assign.
 * @return A reference to the string object.
 */
TString& TString::operator+=(const char* source_string)
{
    // Validate parameter
    if (source_string == nullptr) return *this;

    // Ensure the first string is not empty
    if (this->buffer_ == nullptr)
    {
        // Just assign the data
        this->operator =(source_string);

        // Return this reference
        return *this;
    }

    // Determine the length of the new string
    const std::size_t temp_size = this->Length() + strlen(source_string) + 1;

    // Ensure valid input
    if (temp_size > HE_STRING_MAX_SIZE) throw std::invalid_argument("String too long");

    // Allocate the buffer_
    auto temp_buffer = new char[temp_size];

    // Clear the buffer_
    memset(temp_buffer, 0, temp_size);

    // Copy the data
    strcpy_s(temp_buffer, temp_size, this->buffer_);
    strcat_s(temp_buffer, temp_size, source_string);

    // Free any previous string data
    this->Free();

    // Assign the new string
    this->buffer_ = temp_buffer;
    this->buffer_size_ = temp_size;

    // Return this reference
    return *this;
}

/**
 * Provides the compound assignment operator for TString strings.
 * @param source_string The string object to assign.
 * @return A reference to the string object.
 */
TString& TString::operator+=(const TString& source_string)
{
    // Check for self-assignment
    if (this == &source_string) return *this;

    // Assign the character string
    return this->operator+=(source_string.buffer_);
}

/**
 * Provides the + operator for character strings.
 * @param source_string The string object to assign.
 * @return A reference to the string object.
 */
const TString TString::operator+(const char* source_string) const
{
    TString target_string;

    // Assign the original string
    target_string = this->buffer_;

    // Add the string
    target_string += source_string;

    // Return the result
    return TString(target_string);
}

/**
 * Provides the + operator for character strings.
 * @param source_string The string object to assign.
 * @return A reference to the string object.
 */
const TString TString::operator+(const TString& source_string) const
{
    TString target_string;

    // Assign the original string
    target_string = this->buffer_;

    // Add the string
    target_string += source_string;

    // Return the result
    return TString(target_string);
}

/**
 * Provides the read-only [] operator for strings.
 * Used to reference the character at the specified offset.
 * @param offset The offset of the character to receive.
 * @return The character at the specified offset in the string.
 */
char TString::operator[](std::size_t offset) const
{
    // Ensure a valid string
    if (this->buffer_ == nullptr) throw std::invalid_argument("Invalid string");

    // Ensure a valid offset
    if (offset > this->buffer_size_ - 1) throw std::out_of_range("Invalid string offset");

    // Return the char at the specified offset
    return this->buffer_[offset];
}

/**
 * Provides the [] operator for strings.
 * Used to reference the character at the specified offset.
 * @param offset The offset of the character to receive.
 * @return The character at the specified offset in the string.
 */
char& TString::operator[](std::size_t offset)
{
    // Ensure a valid string
    if (this->buffer_ == nullptr) throw std::invalid_argument("Invalid string");

    // Ensure a valid offset
    if (offset > this->buffer_size_ - 1) throw std::out_of_range("Invalid string offset");

    // Return the char at the specified offset
    return this->buffer_[offset];
}

/**
 * Returns a pointer to the string object currently in memory.
 * @return The pointer to the internal buffer.
 */
TString::operator char* () noexcept
{
    // Return pointer to string buffer
    return this->buffer_;
}

/**
 * Returns a pointer to the string object currently in memory.
 * @return The pointer to the internal buffer.
 */
TString::operator char const* () const noexcept
{
    // Return pointer to string buffer_
    return this->buffer_;
}

/**
 * Returns a pointer to the string object currently in memory.
 * @return The pointer to the internal buffer.
 */
char const* TString::ToString() const noexcept
{
    // Return pointer to string buffer_
    return this->buffer_;
}

/**
 * Returns the sub-string at the specified zero-based offset with the specified lenght.
 * @param offset The zero-based starting offset of the sub-string to get.
 * @param length The length of the sub-string to return or -1 (default) to return the whole string up the terminating zero.
 * @return The sub-string or an empty string on error.
 */
TString TString::SubString(std::size_t offset, int32_t length) const noexcept
{
    TString result;
    std::size_t chars_to_copy;

    // Ensure a valid string
    if (this->buffer_ == nullptr) return TString("");

    // Ensure a valid offset
    if (offset > this->buffer_size_ - 1) return TString("");

    // Get the number of characters to copy
    if (length < 0)
    {
        chars_to_copy = strlen(this->buffer_) - offset;
    }
    else
    {
        chars_to_copy = static_cast<std::size_t>(length);
    }

    // Create buffer for sub-string
    auto buffer = new char[chars_to_copy + 1];

    // Clear the buffer
    memset(buffer, 0, chars_to_copy + 1);

    // Copy string-data to new buffer
    strncpy_s(buffer, chars_to_copy + 1, &this->buffer_[offset], chars_to_copy);
 
    // Copy buffer to TString object
    result = buffer;

    // Free buffer
    delete[] buffer;

    // Return the char at the specified offset
    return result;
}

/**
 * Converts the string to lower case.
 * @return A reference to this string object.
 */
TString& TString::ToLower() noexcept
{
    // Ensure a string object is available
    if (this->buffer_ == nullptr) return *this;

    // Get string length
    const auto length = strlen(this->buffer_);

    // Convert the string to lower case
    for (std::size_t i = 0; i < length; i++)
    {
        if (isupper(this->buffer_[i]) != 0) this->buffer_[i] = static_cast<char>(tolower(this->buffer_[i]));
    }

    // Return a reference to this string object
    return *this;
}

/**
 * Converts the string to upper case.
 * @return A reference to this string object.
 */
TString& TString::ToUpper() noexcept
{
    // Ensure a string object is available
    if (this->buffer_ == nullptr) return *this;

    // Get string length
    const auto length = strlen(this->buffer_);

    // Convert the string to upper case
    for (std::size_t i = 0; i < length; i++)
    {
        if (islower(this->buffer_[i]) != 0) this->buffer_[i] = static_cast<char>(toupper(this->buffer_[i]));
    }

    // Return a reference to this string object
    return *this;
}

/**
 * Removes leading and trailing spaces from the string.
 * The following characters are removed: Space (0x20) and Tab (0x09).
 * @return A reference to this string object.
 */
TString& TString::Trim()
{
    // Ensure a valid string object
    if (this->buffer_ == nullptr) return *this;

    // Trim the string
    // Create memory for the string backup
    const auto temp_size = strlen(this->buffer_) + 1;
    auto temp_string = new char[temp_size];

    // Inititalize the new memory with zeroes
    memset(temp_string, 0, temp_size);

    // Copy the original string to the new buffer_
    strcpy_s(temp_string, temp_size, this->buffer_);

    // Inititalize the old memory with zeroes
    memset(this->buffer_, 0, this->buffer_size_);

    // Now trim the string (in the new buffer)
    // Trim right (replace every space and tab character by binary zero)
    for (int32_t i = static_cast<int32_t>(strlen(temp_string)) - 1; i >= 0; i--)
    {
        if (temp_string[i] == 0x20) temp_string[i] = 0x00;
        if (temp_string[i] == 0x09) temp_string[i] = 0x00;
        if (temp_string[i] != 0x00) break;
    }

    // Trim left
    int32_t string_start = 0;
    for (int32_t i = static_cast<int32_t>(strlen(temp_string)) - 1; i >= 0; i--)
    {
        if ((temp_string[i] != 0x20) && (temp_string[i] != 0x09)) string_start = i;
    }

    // Copy string back, totally trimmed
    strcpy_s(this->buffer_, this->buffer_size_, &temp_string[string_start]);

    // Free the temporary buffer_
    delete[] temp_string;

    // Return a reference to this string object
    return *this;
}

/**
 * Allocates the specified number of bytes for a new string. The string is initialized with binary zeros.
 * @param size The initial size of the new string (excluding the binary zero).
 * @return A pointer to the new internal string object.
 */
char* TString::New(std::size_t size)
{
    return this->New(size, 0);
}

/**
 * Allocates the specified number of bytes for a new string, filled with the specified character.
 * @param size The initial size of the new string (excluding the binary zero).
 * @param filler The character to use to initialize the string with.
 * @return A pointer to the new internal string object.
 */
char* TString::New(std::size_t size, char filler)
{
    // Ensure valid input
    if (size > HE_STRING_MAX_SIZE) return nullptr;

    // Free any previous string
    this->Free();

    // Create the new string
    this->buffer_ = new char[size + 1];

    // Inititalize the memory with zeroes
    memset(this->buffer_, 0, size + 1);

    // Inititalize the memory with the filling char
    memset(this->buffer_, filler, size);

    // Remember the string length (including the binary zero)
    this->buffer_size_ = size + 1;

    // Return a pointer to the string
    return this->buffer_;
}

/**
 * Returns the size of the internal buffer of the string.
 * @return The size of the internal buffer of the string.
 */
std::size_t TString::Size() const noexcept
{
    return this->buffer_size_;
}

/**
 * Returns the length of the string. If there is no internal string object, 0 is returned.
 * @return The length of the string.
 */
std::size_t TString::Length() const noexcept
{
    return (this->buffer_ == nullptr) ? 0 : strlen(this->buffer_);
}

/**
 * Checks if the string is empty.
 * @return true if the string is empty, false otherwise.
 */
bool TString::IsEmpty() noexcept
{
    return (this->Length() == 0) ? true : false;
}

/**
 * Compares the string object with the specified character string.
 * @param string The character string to compare the current string object to.
 * @return true if the strings are equal, false otherwise.
 */
bool TString::Equals(const char* string) noexcept
{
    // If one of the strings is not initialized, they are considered NOT equal
    if ((this->buffer_ == nullptr) || (string == nullptr)) return false;

    // Compary strings
    return (strcmp(this->buffer_, string) == 0);
}

/**
 * Compares the string object with the specified character string for the specified length.
 * @param string The character string to compare the current string object to.
 * @param length The number of characters to compare.
 * @return true if the strings are equal, false otherwise.
 */
bool TString::Equals(const char* string, std::size_t length) noexcept
{
    // If one of the strings is not initialized, they are considered NOT equal
    if ((this->buffer_ == nullptr) || (string == nullptr)) return false;

    // Compary strings
    return (strncmp(this->buffer_, string, length) == 0);
}

/**
 * Compares the string object with the specified character string (case insensitive).
 * @param string The character string to compare the current string object to.
 * @return true if the strings are equal, false otherwise.
 */
bool TString::EqualsCI(const char* string) noexcept
{
    // If one of the strings is not initialized, they are considered NOT equal
    if ((this->buffer_ == nullptr) || (string == nullptr)) return false;

    // Compary strings
    return (_stricmp(this->buffer_, string) == 0);
}

/**
 * Compares the string object with the specified character string for the specified length (case insensitive).
 * @param string The character string to compare the current string object to.
 * @param length The number of characters to compare.
 * @return true if the strings are equal, false otherwise.
 */
bool TString::EqualsCI(const char* string, std::size_t length) noexcept
{
    // If one of the strings is not initialized, they are considered NOT equal
    if ((this->buffer_ == nullptr) || (string == nullptr)) return false;

    // Compary strings
    return (_strnicmp(this->buffer_, string, length) == 0);
}

/**
 * Inserts a string snippet into the current string, enhancing the string if necessary.
 * Previous contents are overwritten, not shifted!
 * @param index The The zero-based index where to insert the string snippet.
 * @param insert_char The char to insert.
 * @return A pointer to the internal string object.
 */
TString& TString::InsertAt(int32_t index, const char insert_char)
{
    char stuff[2];

    // Create a string with the char
    stuff[0] = insert_char;
    stuff[1] = 0;

    // Insert the character
    this->InsertAt(index, stuff);

    // Return a reference to this string object
    return *this;
}

/**
 * Inserts a string snippet into the current string, enhancing the string if necessary.
 * Previous contents are overwritten, not shifted!
 * @param index The zero-based index where to insert the string snippet.
 * @param insert_string The string snippet to insert.
 * @return A pointer to the internal string object.
 */
TString& TString::InsertAt(int32_t index, const char* insert_string)
{
    // Calculate the minimum size of the resulting string, excluding the binary zero.
    const std::size_t new_string_length = static_cast<std::size_t>(index) + strlen(insert_string);

    // Check if the new string will be larger than the old one
    if ((new_string_length + 1) > this->buffer_size_)
    {
        // The new string will be larger, create a new string object
        auto new_string = new char[new_string_length + 1];

        // Reset the new string
        memset(new_string, 0, new_string_length + 1);

        // Clear the new string
        memset(new_string, ' ', new_string_length);

        // If old string data is present, copy that
        if (this->buffer_ != nullptr)
        {
            // Copy the old string data
            memcpy(new_string, this->buffer_, strlen(this->buffer_));

            // Delete the old string object
            delete[] this->buffer_;
        }

        // Assign the new string
        this->buffer_ = new_string;

        // Assign the new size
        this->buffer_size_ = new_string_length + 1;
    }

    // Insert the new string snippet
    memcpy(&this->buffer_[index], insert_string, strlen(insert_string));

    // Return a reference to this string object
    return *this;
}

/**
 * Checks if the string ends with the specified string snippet.
 * @param compare_string The string snippet to check for.
 * @param case_sensitive true to compare case sensitive, false otherwise.
 * @return true, if the string ends with the specified snippet, false otherwise.
 */
bool TString::EndsWith(const char* compare_string, bool case_sensitive) const noexcept
{
    bool equal = false;

    // Validate data
    if (this->buffer_ == nullptr) return false;
    if (compare_string == nullptr) return false;

    // If the snippet is longer than the string, the string cannot end with the snippet
    if (strlen(compare_string) > strlen(this->buffer_)) return false;

    // Determine the position to compare
    const auto position = strlen(this->buffer_) - strlen(compare_string);

    if (case_sensitive)
    {
        equal = (strcmp(&this->buffer_[position], compare_string) == 0);
    }
    else
    {
        equal = (_stricmp(&this->buffer_[position], compare_string) == 0);
    }

    return equal;
}

/**
 * Checks, if the string ends with the specified string snippet.
 * @param compare_string The string snippet to check for.
 * @param case_sensitive true to compare case sensitive, false otherwise.
 * @return true, if the string ends with the specified snippet, false otherwise.
 */
bool TString::EndsWith(const TString& compare_string, bool case_sensitive) const noexcept
{
    return this->EndsWith(compare_string.buffer_, case_sensitive);
}

/**
 * Returns the zero-based index of the first occurence of the specified char within the string.
 * The index of the terminating zero cannot be retrieved by this function.
 * @param char_code The character to find.
 * @return The index of the first occurence of the character or -1 if character not found.
 */
int32_t TString::IndexOf(char char_code) noexcept
{
    // Ensure a valid string
    if (this->buffer_ == nullptr) return -1;

    // The character was not found yet
    int32_t position = -1;

    // Get string length
    const auto string_length = strlen(this->buffer_);

    // Process the string
    for (std::size_t i = 0; i < string_length; i++)
    {
        if (this->buffer_[i] == char_code) position = static_cast<int32_t>(i);
        if (position >= 0) break;
    }

    // Return the position
    return position;
}

/**
 * Removes all occurences of the specified characters from the string.
 * @param char_code The character to be removed from the string.
 * @return The string.
 */
TString& TString::StripChar(char char_code) noexcept
{
    // Validate the string
    if (this->buffer_ == nullptr) return *this;

    // Remove all specified chars
    for (std::size_t i = 0; i < strlen(this->buffer_); i++)
    {
        // Check for character code
        if (this->buffer_[i] == char_code)
        {
            // Char found, shift the rest of the string
            for (std::size_t j = i; j < strlen(this->buffer_); j++) this->buffer_[j] = this->buffer_[j + 1];

            // Adjust i, since the current character has just been overwritten
            i--;
        }
    }

    // Return a reference to this string object
    return *this;
}

/**
 * Removes a number of characters from the right side of the string.
 * This is done by cutting off the string, the bytes beyond the binary zero are not cleared.
 * @param count The number of characters to remove.
 * @return A reference to the string object.
 */
TString& TString::RemoveRight(std::size_t count) noexcept
{
    // Validate buffer
    if (this->buffer_ == nullptr) return *this;

    // Validate count
    if (count > strlen(this->buffer_)) return *this;

    // Check how many chars to remove
    std::size_t index = strlen(this->buffer_) - count;

    // Cut off the string
    this->buffer_[index] = 0;

    // Return a reference to this string object
    return *this;
}

/**
 * Assumes the string to be a decimal integer and returns the 64bit integer value.
 * Values out of range result in INT64_MAX, respective INT64_MIN.
 * @param success A pointer to a boolean that is set to true, if the number is a valid decimal number, to false otherwise.
 * @return The converted number or 0 on failure or overflow.
 */
int64_t TString::ParseDec(bool* success) noexcept
{
    // Nothing valid converted yet
    if (success != nullptr) *success = false;

    // Validate the string
    if (this->buffer_ == nullptr) return 0;

    // Get string length
    const auto length = strlen(this->buffer_);

    // Validate characters
    for (std::size_t i = 0; i < length; i++)
    {
        // Check for sign (i.e. first char can be + or -)
        if (((this->buffer_[i] == '+') || (this->buffer_[i] == '-')) && (i == 0)) continue;

        // Check all valid digits
        if (this->buffer_[i] == '0') continue;
        if (this->buffer_[i] == '1') continue;
        if (this->buffer_[i] == '2') continue;
        if (this->buffer_[i] == '3') continue;
        if (this->buffer_[i] == '4') continue;
        if (this->buffer_[i] == '5') continue;
        if (this->buffer_[i] == '6') continue;
        if (this->buffer_[i] == '7') continue;
        if (this->buffer_[i] == '8') continue;
        if (this->buffer_[i] == '9') continue;

        // Invalid character encountered, number invalid
        return 0;
    }

    // The integer seems valid
    if (success != nullptr) *success = true;

    // Return the number
    return static_cast<int64_t>(std::atoll(this->buffer_));
}

/**
 * Assumes the string to be a hexdecimal number and returns the 64bit integer value. No prefix (e.g 0x) or suffix (e.g. h) is allowed.
 * @param success A pointer to a boolean that is set to true, if the number is a valid hex number, to false otherwise.
 * @return The converted number or 0 on failure or overflow.
 */
int64_t TString::ParseHex(bool* success) noexcept
{
    // Nothing valid converted yet
    if (success != nullptr) *success = false;

    // Validate the string
    if (this->buffer_ == nullptr) return 0;

    // Get string length
    const auto length = strlen(this->buffer_);

    // The maximum range for a 64bit integer is 0xFFFFFFFFFFFFFFFF, thus 16 characters
    if (length > 16) return 0;

    // Check that all chars are valid
    for (std::size_t i = 0; i < length; i++)
    {
        if ((this->buffer_[i] != '0') &&
            (this->buffer_[i] != '1') &&
            (this->buffer_[i] != '2') &&
            (this->buffer_[i] != '3') &&
            (this->buffer_[i] != '4') &&
            (this->buffer_[i] != '5') &&
            (this->buffer_[i] != '6') &&
            (this->buffer_[i] != '7') &&
            (this->buffer_[i] != '8') &&
            (this->buffer_[i] != '9') &&
            (this->buffer_[i] != 'a') &&
            (this->buffer_[i] != 'b') &&
            (this->buffer_[i] != 'c') &&
            (this->buffer_[i] != 'd') &&
            (this->buffer_[i] != 'e') &&
            (this->buffer_[i] != 'f') &&
            (this->buffer_[i] != 'A') &&
            (this->buffer_[i] != 'B') &&
            (this->buffer_[i] != 'C') &&
            (this->buffer_[i] != 'D') &&
            (this->buffer_[i] != 'E') &&
            (this->buffer_[i] != 'F')) return 0;
    }

    // Start with 0
    int64_t value = 0;

    // Convert the number
    for (std::size_t i = 0; i < strlen(this->buffer_); i++)
    {
        value = value << 4;
        switch (this->buffer_[i])
        {
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                value = value | ((this->buffer_[i] - 55) & 0x0F);
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                value = value | ((this->buffer_[i] - 87) & 0x0F);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                value = value | ((this->buffer_[i] - 48) & 0x0F);
                break;
        }
    }

    // Set the validation flag, if specified
    if (success != nullptr) *success = true;

    // Return the converted number
    return value;
}

/**
 * Assumes the string to be a binary number and returns the 64bit integer value.
 * @param success A pointer to a boolean that is set to true, if the number is a valid hex number, to false otherwise.
 * @return The converted number or 0 on failure or overflow.
 */
int64_t TString::ParseBin(bool* success) noexcept
{
    // Nothing valid converted yet
    if (success != nullptr) *success = false;

    // Validate the string
    if (this->buffer_ == nullptr) return 0;

    // Get string length
    const auto length = strlen(this->buffer_);

    // The maximum range for a 64bit integer is 64 characters
    if (length > 64) return 0;

    // Check that all chars are valid
    for (std::size_t i = 0; i < length; i++)
    {
        if ((this->buffer_[i] != '0') && (this->buffer_[i] != '1')) return 0;
    }

    // Start with 0
    int64_t value = 0;

    // Convert
    for (std::size_t i = 0; i < length; i++)
    {
        value = value << 1;
        if (this->buffer_[i] == '1') value = value | 1;
    }

    // Set the validation flag, if specified
    if (success != nullptr) *success = true;

    // Return the converted number
    return value;
}

/**
 * Converts a given unsigned integer to it's binary representation (as string).
 * @param value The unsigned integer to create the binary representation of.
 * @param bit_count The number of bits to use (up to 64).
 * @return A reference to this string object.
 */
TString& TString::BinaryFromInt(int64_t value, std::size_t bit_count)
{
    // Validate bit count
    if (bit_count > 64) return *this;

    // Calculate the bitmask for the comparison
    const uint64_t bitmask = (static_cast<uint64_t>(1) << (bit_count - 1));

    // Create the buffer for the string representation
    this->New(bit_count + 1);

    // Add the single binary digits
    for (std::size_t i = 0; i < bit_count; i++) {

        // Check highmost bit
        if ((static_cast<uint64_t>(value) & bitmask) != 0)
        {
            strcat_s(this->buffer_, this->buffer_size_, "1");
        }
        else
        {
            strcat_s(this->buffer_, this->buffer_size_, "0");
        }

        // Advance to next bit
        value <<= 1;
    }

    // Return a reference to this string object
    return *this;
}
