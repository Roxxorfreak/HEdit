// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Compares the given (relative) offset of all registered editors, returning
 * true if the byte at the specified offset is the same in all files.
 * @param offset The offset to compare, relative to the current file position of each editor.
 * @return true if the bytes at the specified position are equal in all editors, false otherwise.
 */
bool TComparator::CompareOffset(int64_t offset) noexcept
{
    // Assume all bytes are equal
    bool equal = true;

    // Start with the second editor and compare all editors with the previous editor.
    // If the two bytes are not equal, return false
    for (int32_t i = 1; i < this->editors_; i++)
    {
        unsigned char byte1 = 0;
        unsigned char byte2 = 0;
        if (this->editor_[i - 1] != nullptr) this->editor_[i - 1]->ReadBytesAtOffset(this->editor_[i - 1]->GetFileOffset() + offset, &byte1, 1);
        if (this->editor_[i] != nullptr) this->editor_[i]->ReadBytesAtOffset(this->editor_[i]->GetFileOffset() + offset, &byte2, 1);
        if (byte1 != byte2) equal = false;
    }

    // Return equality
    return equal;
}

/**
 * Register an editor instance for later comparation use and return the
 * local id under which the editor is registered.
 * @param editor The editor to add to the comparator.
 * @return The id of the newly added editor, or -1 on failure.
 */
int32_t TComparator::Add(TEditor* editor) noexcept
{
    // Ensure there is enough space for another editor
    if (this->editors_ >= HE_MAX_EDITORS) return -1;

    // Add the editor
    this->editor_[this->editors_] = editor;
    this->editors_++;

    // Return the editor id
    return (this->editors_ - 1);
}

/**
 * Returns the number of registered editors.
 * @return The number of registered editors.
 */
int32_t TComparator::EditorCount() const noexcept
{
    return this->editors_;
}
