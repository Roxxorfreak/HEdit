// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_VALUE_PROCESSOR_HPP_

    // Header included
    #define HEDIT_SRC_VALUE_PROCESSOR_HPP_

    // Define the sizes of values in bytes.
    // These values are used to calculate the number of bytes to display (for hex-decimal and binary values).
    enum class TValueSize : std::size_t {
        BYTE = 1,   //!< The value size: byte
        WORD = 2,   //!< The value size: word
        DWORD = 4,  //!< The value size: double word
        QWORD = 8   //!< The value size: quad word
    };

    // The macro for accessing the underlying value of a TValueSize
    #define EV(x) static_cast<std::size_t>(x)

    // The number formats for the value processor
    enum class TNumberFormat : int32_t {
        HEX,    //!< The number format for the value processor: hex-decimal
        DEC,    //!< The number format for the value processor: decimal
        BIN     //!< The number format for the value processor: binary
    };

    /**
     * @brief The class that encapsulates a value processor, used to create strings with formatted values.
     * The default format is hex-decimal (TNumberFormat::HEX).
     */
    class TValueProcessor
    {
    private:
        TNumberFormat format_ = TNumberFormat::HEX;
    public:
        void SetNumberFormat(TNumberFormat format);
        TString FormatValue(int64_t value, TValueSize value_size, bool use_sign = false) const;
    };

#endif  // HEDIT_SRC_VALUE_PROCESSOR_HPP_
