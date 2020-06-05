// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_MARKER_HPP_

    // Header included
    #define HEDIT_SRC_MARKER_HPP_

    /**
     * @brief The class that encapsulates the functions that are needed to mark an area within a file.
     */
    class TMarker
    {
    public:
        int64_t start_ = { -1 };    //!< The (absolute) starting offset of the marker within the file.
        int64_t end_ = { -1 };      //!< The (absolute) ending offset of the marker within the file.
    public:
        void Clear() noexcept;
        bool IsSelected(int64_t position) noexcept;
        int64_t Length() noexcept;
        int64_t Start() const noexcept;
    };

#endif  // HEDIT_SRC_MARKER_HPP_
