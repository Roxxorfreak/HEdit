// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_STRING_HPP_

    // Header included
    #define HEDIT_SRC_STRING_HPP_

    // The maximum size of the string (excluding the binary zero)
    constexpr int32_t HE_STRING_MAX_SIZE = 65534;

    /**
     * @brief The HEdit implementation of a character string (8bit).
     * @details This class encapsulates a character string with all usual string operations.
     */
    class TString
    {
    private:
        // cppcheck-suppress unsafeClassCanLeak
        char* buffer_ = { nullptr };    //!< The buffer for the binary data of the string.
        std::size_t buffer_size_ = {};  //!< The size of the internal string buffer (including binary zero).
    public:
        TString() noexcept;
        TString(TString&& source_string) noexcept;
        TString(const TString& source_string);
        explicit TString(std::size_t size, char filler = ' ');
        explicit TString(const char* source_string);
        ~TString();
        void Free() noexcept;
        TString& operator=(TString&& source_string) noexcept;
        TString& operator=(const char* source_string);
        TString& operator=(const TString& source_string);
        TString& operator+=(const char character);
        TString& operator+=(const char* source_string);
        TString& operator+=(const TString& source_string);
        const TString operator+(const char* source_string) const;
        const TString operator+(const TString& source_string) const;
        char operator[](std::size_t offset) const;
        char& operator[](std::size_t offset);
        operator char* () noexcept;
        operator char const* () const noexcept;
        char const* ToString() const noexcept;
        TString SubString(std::size_t offset, int32_t length = -1) const;
        TString& ToLower() noexcept;
        TString& ToUpper() noexcept;
        TString& Trim();
        char* New(std::size_t size);
        char* New(std::size_t size, char filler);
        std::size_t Size() const noexcept;
        std::size_t Length() const noexcept;
        bool IsEmpty() noexcept;
        bool Equals(const char* string) const noexcept;
        bool Equals(const char* string, std::size_t length) const noexcept;
        bool EqualsCI(const char* string) const noexcept;
        bool EqualsCI(const char* string, std::size_t length) const noexcept;
        TString& InsertAt(int32_t index, const char insert_char);
        TString& InsertAt(int32_t index, const char* insert_string);
        bool EndsWith(const char* compare_string, bool case_sensitive = true) const noexcept;
        bool EndsWith(const TString& compare_string, bool case_sensitive = true) const noexcept;
        int32_t IndexOf(char char_code) noexcept;
        TString& StripChar(char char_code) noexcept;
        TString& RemoveRight(std::size_t count) noexcept;
        int64_t ParseDec(bool* success = nullptr) noexcept;
        int64_t ParseHex(bool* success = nullptr) noexcept;
        int64_t ParseBin(bool* success = nullptr) noexcept;
        TString& BinaryFromInt(int64_t value, std::size_t bit_count);
    };

    // The String List type. Actually a std::vector<TString>
    typedef std::vector<TString> TStringList;

#endif  // HEDIT_SRC_STRING_HPP_
