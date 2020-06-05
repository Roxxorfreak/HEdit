// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_FILE_HPP_

    // Header included
    #define HEDIT_SRC_FILE_HPP_

    // The size for the file cache
    constexpr uint32_t HE_FILE_CACHE_SIZE = 8192;  //!< The size of the memory that is used for caching the file content.

    // The file modes
    enum class TFileMode : int32_t {
        READ,       //!< The file is opened for reading only.
        READWRITE,  //!< The file is opened for reading and writing.
        CREATE      //!< The file opened for creation.
    };

    // File attributes
    enum class TFileAttribute : int32_t {
        NORMAL,         //!< The file was opened for reading and writing.
        READ_ONLY,      //!< The file was opened read-only.
        ACCESS_DENIED   //!< Access to the file was denied.
    };

    /**
     * @brief The class that encapsulates a file object that can be used for platform-independant file access.
     */
    class TFile
    {
    private:
        bool use_cache_;                 //!< The flag that specifies if caching is used.
        unsigned char* file_buffer_;     //!< The buffer that is used to cache some file content, if caching is enabled.
        int64_t cache_start_;            //!< The file offset where the cache starts.
        int64_t cache_length_;           //!< The length of the cached block of data.
        int64_t file_cursor_;            //!< The current position within the opened file, if caching is enabled.
        FILE* file_handle_;              //!< The internal file handle.
        TString file_name_;              //!< The name of the file that is handled by the file class.
        TFileAttribute file_attribute_;  //!< The file attribute (see TFileAttribute).
    private:
        bool ReadIntoCache() noexcept;
        uint32_t ReadFromCache(unsigned char* buffer, uint32_t count) noexcept;
    public:
        TFile(const char* file_name, bool use_cache);
        TFile(const TFile& source) = delete;
        TFile& operator=(const TFile& source) = delete;
        TFile(TFile&&) = delete;
        TFile& operator=(TFile&&) = delete;
        ~TFile();
        bool Open(TFileMode mode) noexcept;
        void Close() noexcept;
        uint32_t Read(unsigned char* buffer, uint32_t length) noexcept;
        uint32_t ReadAt(unsigned char* buffer, uint32_t length, int64_t position) noexcept;
        uint32_t Write(const unsigned char* buffer, uint32_t length) noexcept;
        uint32_t WriteAt(const unsigned char* buffer, uint32_t length, int64_t position) noexcept;
        bool Seek(int64_t position) noexcept;
        TString ReadLine();
        bool IsEOF() noexcept;
        bool IsReadOnly() const noexcept;
        int64_t FileSize() noexcept;
        void AssignFileName(const char* file_name);
    };

#endif  // HEDIT_SRC_FILE_HPP_
