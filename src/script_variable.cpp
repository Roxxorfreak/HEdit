// Copyright (c) 2021 Roxxorfreak

#include "headers.hpp"

/**
 * Returns the current variable value as signed 32bit integer.
 * The upper 32 bit of the internal 64 bit memory are discarded.
 * @return The current variable value as signed 32bit integer.
 */
int32_t TScriptVariable::AsSignedInt32() const noexcept
{
    return static_cast<int32_t>(this->value_ & 0xFFFFFFFF);
}

/**
 * Returns the current variable value as unsigned 64bit integer (qword).
 * @return The current variable value as unsigned 64bit integer (qword).
 */
uint64_t TScriptVariable::AsUnsignedInt64() const noexcept
{
    return this->value_;
}

/**
 * Returns the current variable value as unsigned 32bit integer (dword).
 * @return The current variable value as unsigned 32bit integer (dword).
 */
uint32_t TScriptVariable::AsUnsignedInt32() const noexcept
{
    return static_cast<uint32_t>(this->value_ & 0xFFFFFFFF);
}

/**
 * Returns the current variable value as unsigned 16bit integer (word).
 * @return The current variable value as unsigned 16bit integer (word).
 */
uint16_t TScriptVariable::AsUnsignedInt16() const noexcept
{
    return static_cast<uint16_t>(this->value_ & 0x0000FFFF);
}

/**
 * Returns the current variable value as unsigned 8bit integer (byte).
 * @return The current variable value as unsigned 8bit integer (byte).
 */
uint8_t TScriptVariable::AsUnsignedInt8() const noexcept
{
    return static_cast<uint8_t>(this->value_ & 0x000000FF);
}

/**
 * Adds the specified value to the variable.
 * Takes into account the signedness of the variable value.
 * @param value The value to add to the variable.
 */
void TScriptVariable::Add(int64_t value) noexcept
{
    if (this->is_signed_)
    {
        auto temp_signed = static_cast<int64_t>(this->value_);
        temp_signed += value;
        this->value_ = static_cast<uint64_t>(temp_signed);
    }
    else
    {
        auto temp_unsigned = this->value_;
        temp_unsigned += static_cast<uint64_t>(value);
        this->value_ = temp_unsigned;
    }
}

/**
 * Subtracts the specified value from the variable.
 * Takes into account the signedness of the variable value.
 * @param value The value to subtract from the variable.
 */
void TScriptVariable::Sub(int64_t value) noexcept
{
    if (this->is_signed_)
    {
        auto temp_signed = static_cast<int64_t>(this->value_);
        temp_signed -= value;
        this->value_ = static_cast<uint64_t>(temp_signed);
    }
    else
    {
        auto temp_unsigned = this->value_;
        temp_unsigned -= static_cast<uint64_t>(value);
        this->value_ = temp_unsigned;
    }
}
