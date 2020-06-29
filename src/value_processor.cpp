// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Sets the number format for the value processor.
 * @param format The new value format (see TNumberFormat).
 */
void TValueProcessor::SetNumberFormat(TNumberFormat format)
{
    this->format_ = format;
}

/**
 * Formats an integer value for output.
 * @param value The value to format for output.
 * @param value_size The size (in bytes) of the value (see TValueSize).
 * @param number_format The number format for the output of number (see TNumberFormat).
 * @param use_sign true to format the output with +/-sign, false otherwise.
 */
TString TValueProcessor::FormatValue(int64_t value, TValueSize value_size, bool use_sign) const
{
    TString sign;
    char buffer[65];  // The buffer must be long enough to hold a 64bit number in binary representation

    // Check if to use sign
    if (use_sign == true)
    {
        // Use sign
        if (value < 0)
        {
            // Value is negative, use sign and convert value
            sign = "-";
            value = std::llabs(value);
        }
        else
        {
            // Value is positive, use sign and leave value
            sign = "+";
        }
    }
    else
    {
        // Don't display sign
        sign = "";
    }

    switch (this->format_)
    {
    case TNumberFormat::DEC:
        snprintf(buffer, sizeof(buffer), "%s%" PRIi64, sign.ToString(), value);
        break;
    case TNumberFormat::HEX:
        if (value_size == TValueSize::BYTE)  snprintf(buffer, sizeof(buffer), "%s0x%02" PRIX8, sign.ToString(), static_cast<uint8_t>(value));
        if (value_size == TValueSize::WORD)  snprintf(buffer, sizeof(buffer), "%s0x%04" PRIX16, sign.ToString(), static_cast<uint16_t>(value));
        if (value_size == TValueSize::DWORD) snprintf(buffer, sizeof(buffer), "%s0x%08" PRIX32, sign.ToString(), static_cast<uint32_t>(value));
        if (value_size == TValueSize::QWORD) snprintf(buffer, sizeof(buffer), "%s0x%016" PRIX64, sign.ToString(), static_cast<uint64_t>(value));
        break;
    case TNumberFormat::BIN:
        TString bin;
        bin.BinaryFromInt(value, EV(value_size) * 8);
        snprintf(buffer, sizeof(buffer), "%s%s", sign.ToString(), bin.ToString());
        break;
    }

    // Return the string
    return TString(buffer);
}
