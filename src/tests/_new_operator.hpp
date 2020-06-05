// Copyright(C) 2020 Roxxorfreak

#ifndef HEDIT_SRC_TESTS__NEW_OPERATOR_HPP_

// Header included
#define HEDIT_SRC_TESTS__NEW_OPERATOR_HPP_

#include <new>

// Class for toggeling memory failure
class NewOperator
{
public:
    bool should_fail = false;
    void ShouldFail() {
        this->should_fail = true;
    }
    void ShouldNotFail() {
        this->should_fail = false;
    }
};

// Make available everywhere
extern NewOperator New;

#endif   // HEDIT_SRC_TESTS__NEW_OPERATOR_HPP_
