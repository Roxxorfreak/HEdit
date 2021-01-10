// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_CONFIG_FILE_HPP_

    // Header included
    #define HEDIT_SRC_CONFIG_FILE_HPP_

    /**
     * @brief The class that encapsulates a single entry in the config file.
     * @details A config file entry has the format "key=value" on a single line in the file.
     * Lines that start with #, ; or // are treated as comments and cannot be accessed as config entry.
     */
    struct TConfigEntry
    {
        TString key;    //!< The config entry key.
        TString value;  //!< The config entry value as string
        bool is(const char* val) const { return key.EqualsCI(val); } //!< Checks if the config entry matches the specified key.
    };

    /**
     * @brief The class that encapsulates a config file object.
     * It is used to read and write the HEdit config file.
     */
    class TConfigFile : public TFile
    {
    public:
        explicit TConfigFile(const char* file_name);
        TConfigFile(const TConfigFile& source) = delete;
        TConfigFile& operator=(const TConfigFile& source) = delete;
        TConfigFile(TConfigFile&&) = delete;
        TConfigFile& operator=(TConfigFile&&) = delete;
        bool ReadNextEntry(TConfigEntry* entry);
        void WriteNewline() noexcept;
        void WriteConfigLine(const char* format, ...) noexcept;
    };

#endif  // HEDIT_SRC_CONFIG_FILE_HPP_
