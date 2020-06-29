// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new file instance initializing the buffer for the cache (if the
 * file uses caching) and assigns a file name.
 * @param file_name The name of the file to manage.
 * @param use_cache true to use caching, false to do not.
 */
TFile::TFile(const char* file_name, bool use_cache)
    : file_buffer_(nullptr),
    cache_start_(0),
    cache_length_(0),
    file_cursor_(0),
    file_handle_(nullptr),
    file_name_(nullptr),
    file_attribute_(TFileAttribute::NORMAL)
{
    // If caching is to be used, allocate memory for the cache
    this->use_cache_ = use_cache;
    if (this->use_cache_)
    {
        this->file_buffer_ = new unsigned char[HE_FILE_CACHE_SIZE];
    }

    // Store file name for later use
    this->file_name_ = file_name;
}

/**
 * Destructor, frees the cache buffer and closes the associated file.
 */
TFile::~TFile()
{
    // Check if a cache was allocate
    if (this->file_buffer_ != nullptr)
    {
        // Free the file cache
        delete[] this->file_buffer_;

        // Clear the cache handle
        this->file_buffer_ = nullptr;
    }

    // Close the file
    this->Close();
}

/**
 * Tries to open the file, setting the internal file attribute to
 * TFileAttribute::ACCESS_DENIED if the file cannot be openend for any reason.
 * @param mode The file mode to use for opening (TFileMode).
 * @return true on success, false otherwise.
 */
bool TFile::Open(TFileMode mode) noexcept
{
    // Close any open files
    this->Close();

    // Reset cache
    this->cache_start_ = 0;
    this->cache_length_ = 0;
    this->file_cursor_ = 0;

    // Attempt to open the file in the specified mode
    if (mode == TFileMode::READ)
    {
        #if defined(WIN32) && !defined(__BORLANDC__)
        this->file_handle_ = _fsopen(this->file_name_, "rb", _SH_DENYNO);
        #else
        this->file_handle_ = fopen(this->file_name_, "rb");
        #endif
        this->file_attribute_ = TFileAttribute::READ_ONLY;
    }
    else if (mode == TFileMode::READWRITE)
    {
        #if defined(WIN32) && !defined(__BORLANDC__)
        this->file_handle_ = _fsopen(this->file_name_, "r+b", _SH_DENYNO);
        #else
        this->file_handle_ = fopen(this->file_name_, "r+b");
        #endif
        this->file_attribute_ = TFileAttribute::NORMAL;
    }
    else if (mode == TFileMode::CREATE)
    {
        #if defined(WIN32) && !defined(__BORLANDC__)
        this->file_handle_ = _fsopen(this->file_name_, "w+b", _SH_DENYNO);
        #else
        this->file_handle_ = fopen(this->file_name_, "w+b");
        #endif
        this->file_attribute_ = TFileAttribute::NORMAL;
    }

    if (this->file_handle_ != nullptr)
    {
        // Valid file handle
        return true;
    }
    else
    {
        // Invalid file handle, report as "File access denied"
        this->file_attribute_ = TFileAttribute::ACCESS_DENIED;
        return false;
    }
}

/**
 * Closes the associated file, if open.
 */
void TFile::Close() noexcept
{
    // Ensure a valid file handle
    if (file_handle_ == nullptr) return;

    // Close the file
    fclose(file_handle_);

    // Clear the file handle
    this->file_handle_ = nullptr;

    // Clear the cache, if any
    if (this->file_buffer_ != nullptr) memset(this->file_buffer_, 0, HE_FILE_CACHE_SIZE);
}

/**
 * Reads some bytes from the current position in the file into a buffer.
 * @param buffer The buffer to read the data into.
 * @param length The size of the buffer.
 * @return The number of bytes read from the file.
 */
uint32_t TFile::Read(unsigned char* buffer, uint32_t length) noexcept
{
    // Clear the buffer
    memset(buffer, 0, length);

    // Check, if file opened
    if (this->file_handle_ == nullptr) return 0;

    // Check if to read from file cache
    if (this->use_cache_ == false)
    {
        // Cache disabled, read from file
        return fread(buffer, 1, length, this->file_handle_);
    }

    // Read from cache
    const auto bytes_read = this->ReadFromCache(buffer, length);
    if (bytes_read == 0) return 0;

    // Advance position in file
    this->file_cursor_ += bytes_read;

    // Return number of bytes read from the file
    return bytes_read;
}

/**
 * Reads some bytes from the specified position in the file into a buffer.
 * @param buffer The buffer to read the data into.
 * @param length The size of the buffer.
 * @param position The position to read at.
 * @return The number of bytes read from the file.
 */
uint32_t TFile::ReadAt(unsigned char* buffer, uint32_t length, int64_t position) noexcept
{
    // Try to set file pointer position
    if (this->Seek(position) == false) return 0;

    // Try to read there
    return this->Read(buffer, length);
}

/**
 * Writes some bytes from a buffer into the file at the current position.
 * @param buffer The buffer with the data.
 * @param length The number of bytes to write.
 * @return The number of bytes written.
 */
uint32_t TFile::Write(const unsigned char* buffer, uint32_t length) noexcept
{
    // Check, if file opened
    if (this->file_handle_ == nullptr) return 0;

    // If caching is enabled, position the file cursor on the correct position
    if (use_cache_ == true)
    {
        _fseeki64(this->file_handle_, this->file_cursor_, SEEK_SET);
    }

    // Writes go always directly to the file
    const auto bytes_written = fwrite(buffer, 1, length, this->file_handle_);

    // Flush the stream
    fflush(this->file_handle_);

    // Ensure the bytes were written
    if (bytes_written == 0) return 0;

    // Assign the
    this->file_cursor_ += bytes_written;

    // Refresh the cache (if caching is used)
    if (this->use_cache_) this->ReadIntoCache();

    // Return the number of bytes written
    return bytes_written;
}

/**
 * Writes some bytes from a buffer into the file at the the specified position.
 * @param buffer The buffer with the data.
 * @param length The number of bytes to write.
 * @param position The position to write at.
 * @return The number of bytes written.
 */
uint32_t TFile::WriteAt(const unsigned char* buffer, uint32_t length, int64_t position) noexcept
{
    // Try to set file pointer position
    if (this->Seek(position) == false) return 0;

    // Try to write there
    return this->Write(buffer, length);
}

/**
 * Positions the file cursor.
 * @param position The new position of the file pointer.
 * @return true on success, false otherwise.
 */
bool TFile::Seek(int64_t position) noexcept
{
    // Check, if file opened
    if (this->file_handle_ == nullptr) return false;

    // Ensure a valid (positive) position
    if (position < 0) return false;

    // Check if caching is used
    if (this->use_cache_)
    {
        // Set the position of the virtual file pointer
        this->file_cursor_ = position;

        // Always return success
        return true;
    }
    else
    {
        // Set the position of the real file pointer
        if (_fseeki64(this->file_handle_, position, SEEK_SET) < 0) return false;

        // Return success
        return true;
    }
}

/**
 * Reads one line of text from the current position in the file.
 * If no data can be read, an empty line is returned.
 * @return A string with the read line.
 */
TString TFile::ReadLine()
{
    TString buffer;
    unsigned char tempbuffer[2];

    // Initialize the temporary buffer
    tempbuffer[1] = 0;

    // Process the whole file
    while (!this->IsEOF())
    {
        // Read one byte
        if (this->Read(tempbuffer, 1) < 1) continue;

        // Skip CR
        if (tempbuffer[0] == 0x0D) continue;

        // End with LF
        if (tempbuffer[0] == 0x0A) break;

        // Append the character
        buffer += static_cast<char>(tempbuffer[0]);
    }

    // Return the string
    return buffer;
}

/**
 * Returns true if end of file is reached, false otherwise.
 * @return true if end of file is reached, false otherwise.
 */
bool TFile::IsEOF() noexcept
{
    if (this->use_cache_ == true)
    {
        return (file_cursor_ >= this->FileSize());
    }
    else
    {
        return (feof(this->file_handle_) != 0);
    }
}

/**
 * Returns true if the file is read-only, false otherwise.
 * @return true if the file is read-only, false otherwise.
 */
bool TFile::IsReadOnly() const noexcept
{
    return (this->file_attribute_ == TFileAttribute::READ_ONLY);
}

/**
 * Returns the size of the associated file.
 * @return The size of the associated file.
 */
int64_t TFile::FileSize() noexcept
{
    // Ensure the file is open
    if (this->file_handle_ == nullptr) return 0;

    // Remember the old file pos
    const auto old_pos = _ftelli64(this->file_handle_);

    // Jump to eof
    _fseeki64(this->file_handle_, 0, SEEK_END);

    // Retrieve current position, that equals to file length
    const auto length = _ftelli64(this->file_handle_);

    // Set old position
    _fseeki64(this->file_handle_, old_pos, SEEK_SET);

    // Return file size
    return length;
}

/**
 * Fills the cache with data, starting at the current cache start position and reading HE_FILE_CACHE_SIZE bytes.
 * @return true on success, false otherwise.
 */
bool TFile::ReadIntoCache() noexcept
{
    // Clear cache completely
    memset(this->file_buffer_, 0, HE_FILE_CACHE_SIZE);

    // Seek to the cache starting position
    const auto return_value = _fseeki64(file_handle_, this->cache_start_, SEEK_SET);
    if (return_value < 0)
    {
        this->cache_start_ = 0;
        this->cache_length_ = 0;
        return false;
    }

    // Read the data bytes from the file
    const auto bytes_read = fread(this->file_buffer_, 1, HE_FILE_CACHE_SIZE, this->file_handle_);
    if (bytes_read == 0)
    {
        this->cache_start_ = 0;
        this->cache_length_ = 0;
        return false;
    }

    // Store the current cache length
    this->cache_length_ = bytes_read;

    // Return sucess
    return true;
}

/**
 * Reads the specified number of bytes from the cache into the specified buffer.
 * If the data at the desired reading position (file_cursor_) is not yet cached, 
 * the ReadIntoCache() function is called to cache the required data.
 * @param buffer The buffer to read into.
 * @param count The number of bytes to read.
 * @return The number of bytes that were read from the cache.
 */
uint32_t TFile::ReadFromCache(unsigned char* buffer, uint32_t count) noexcept
{
    // Check, if the file cursor is in the cached range, if not refresh the cache
    if ((this->file_cursor_ < this->cache_start_) || (this->file_cursor_ >= (this->cache_start_ + this->cache_length_)) ||
       ((this->file_cursor_ + count) < this->cache_start_) || ((this->file_cursor_ + count) >= (this->cache_start_ + this->cache_length_)))
    {
        if ((HE_FILE_CACHE_SIZE / 2) > this->file_cursor_)
            this->cache_start_ = 0;
        else
            this->cache_start_ = this->file_cursor_ - (HE_FILE_CACHE_SIZE / 2);
        if (!this->ReadIntoCache()) return 0;
    }
    // Check if the whole area that is to read is in the cache, if not return less data
    const int32_t bytes_to_read = hedit_min(static_cast<int32_t>(this->cache_length_ - (this->file_cursor_ - this->cache_start_)), static_cast<int32_t>(count));
    if (bytes_to_read < 0) return 0;

    // Everything ok, return the requested data
    memset(buffer, 0, count);
    memcpy(buffer, &this->file_buffer_[static_cast<int32_t>(this->file_cursor_ - this->cache_start_)], static_cast<std::size_t>(bytes_to_read));
    return static_cast<uint32_t>(bytes_to_read);
}

/**
 * Assigns a new file name to the file class, closing the current one if open.
 * @param file_name The file name to assign to the file object.
 */
void TFile::AssignFileName(const char* file_name)
{
    // Close any open file
    this->Close();

    // Assign the new file name
    this->file_name_ = file_name;
}
