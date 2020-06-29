// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_CONFIG_FILE_HPP_

    // Header included
    #define HEDIT_SRC_CONFIG_FILE_HPP_

    /**
     * @brief A single entry read from the config file.
     */
    struct TConfigEntry
    {
        TString key;    //!< The config entry key.
        TString value;  //!< The config entry value as string
    };

    /**
     * @brief The class that encapsulates a config file object.
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
    };

#endif  // HEDIT_SRC_CONFIG_FILE_HPP_
