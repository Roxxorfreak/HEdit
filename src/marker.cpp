// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Clears the marker.
 */
void TMarker::Clear() noexcept
{
    this->start_ = -1;
    this->end_ = -1;
}

/**
 * Checks if the specified position lays in the marked area of the file.
 * @param position The position to check.
 * @return true if the specified position lays in the marked area, false otherwise.
 */
bool TMarker::IsSelected(int64_t position) noexcept
{
    if ((this->start_ == -1) || (this->end_ == -1)) return false;
    return ((position >= hedit_min(this->start_, this->end_)) && (position <= hedit_max(this->start_, this->end_))) ? true : false;
}

/**
 * Returns the length of the marker (in bytes).
 * @return The length of the marker (in bytes).
 */
int64_t TMarker::Length() const noexcept
{
    // Validate marker range
    if ((this->start_ == -1) || (this->end_ == -1)) return 0;

    // Calculate length
    const int64_t length = this->start_ - this->end_;

    // Return the length
    return std::llabs(length) + 1;
}

/**
 * Returns the start address (file offset) of the marker.
 * @return The start address of the marker.
 */
int64_t TMarker::Start() const noexcept
{
    return hedit_min(this->start_, this->end_);
}

