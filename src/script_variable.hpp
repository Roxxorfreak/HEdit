// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_SCRIPT_VARIABLE_HPP_

    // Header included
    #define HEDIT_SRC_SCRIPT_VARIABLE_HPP_

    /**
     * @brief The structure for managing the data of a script variable.
     */
    class TScriptVariable
    {
    public:
        bool is_signed_ = { false };  //!< The "signed" flag, true if the variable is signed, false otherwise.
        uint64_t value_ = { 0 };      //!< The current variable value.
        TString name_;                //!< The name of the variable as referred in the script code.
    public:
        int32_t AsSignedInt32() const noexcept;
        uint64_t AsUnsignedInt64() const noexcept;
        uint32_t AsUnsignedInt32() const noexcept;
        uint16_t AsUnsignedInt16() const noexcept;
        uint8_t AsUnsignedInt8() const noexcept;
        void Add(int64_t value) noexcept;
        void Sub(int64_t value) noexcept;
    };

#endif  // HEDIT_SRC_SCRIPT_VARIABLE_HPP_
