// Copyright (c) 2021 Roxxorfreak

#include "_new_operator.hpp"

// Create a global instance of the class
NewOperator New;

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](size_t _Size)
{
    if (New.should_fail) {
        throw std::bad_alloc();
    }
    return std::malloc(_Size);
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](size_t _Size, std::nothrow_t const&) noexcept
{
    if (New.should_fail) {
        return nullptr;
    }
    return std::malloc(_Size);
}
