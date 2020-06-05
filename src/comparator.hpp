// Copyright (c) 2020 Roxxorfreak

#ifndef HEDIT_SRC_COMPARATOR_HPP_

    // Header included
    #define HEDIT_SRC_COMPARATOR_HPP_

    /**
     * @brief The comparator engine that can be used to compare data between editors.
     */
    class TComparator
    {
    private:
        int32_t editors_ = { 0 };                       //!< The number of editors currently added to the comparator engine.
        TEditor* editor_[HE_MAX_EDITORS] = { nullptr }; //!< An array of pointers to the editors to compare.
    public:
        bool CompareOffset(int64_t offset) noexcept;
        int32_t Add(TEditor* editor) noexcept;
        int32_t EditorCount() const noexcept;
    };

#endif  // HEDIT_SRC_COMPARATOR_HPP_
