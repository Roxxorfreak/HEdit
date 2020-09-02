// Copyright (c) 2020 Roxxorfreak

#include "headers_test.hpp"

TEST(TFormula, Evaluate)
{
    TFormula formula;

    // Evaluate some valid terms
    ASSERT_EQ(42, formula.Evaluate(TString("42")));
    ASSERT_EQ(42, formula.Evaluate(TString("21+21")));
    ASSERT_EQ(42, formula.Evaluate(TString("21*2")));
    ASSERT_EQ(42, formula.Evaluate(TString("2*15h")));
    ASSERT_EQ(42, formula.Evaluate(TString("10b*0x15")));
    ASSERT_EQ(42, formula.Evaluate(TString("10b*0xff&21")));
    ASSERT_EQ(42, formula.Evaluate(TString("50d-2*100b")));
    ASSERT_EQ(42, formula.Evaluate(TString("50d-10h/10b")));
    ASSERT_EQ(-1, formula.Evaluate(TString("0xffffffffffffffff")));
    ASSERT_EQ(510, formula.Evaluate(TString("10b*0xff|21")));
    ASSERT_EQ(4294967295, formula.Evaluate(TString("0xffffffff")));

    // Evaluate some invalid numbers
    ASSERT_EQ(0, formula.Evaluate(TString("50d-10h/10x")));
    ASSERT_EQ(TFormulaError::INVALID_NUMBER, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("0x1g")));
    ASSERT_EQ(TFormulaError::INVALID_NUMBER, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("1=2")));
    ASSERT_EQ(TFormulaError::INVALID_NUMBER, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("1=2")));
    ASSERT_EQ(TFormulaError::INVALID_NUMBER, formula.GetLastError());

    // Evaluate some incomplete expressions
    ASSERT_EQ(0, formula.Evaluate(TString("3-")));
    ASSERT_EQ(TFormulaError::EXP_INCOMPLETE, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("+42")));
    ASSERT_EQ(TFormulaError::EXP_INCOMPLETE, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("+/-")));
    ASSERT_EQ(TFormulaError::EXP_INCOMPLETE, formula.GetLastError());

    // Evaluate some divisions by zero
    ASSERT_EQ(0, formula.Evaluate(TString("10b/0x00")));
    ASSERT_EQ(TFormulaError::DIV_BY_ZERO, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("3/2*0")));
    ASSERT_EQ(TFormulaError::DIV_BY_ZERO, formula.GetLastError());
    ASSERT_EQ(0, formula.Evaluate(TString("42/0xf0&0x0f")));
    ASSERT_EQ(TFormulaError::DIV_BY_ZERO, formula.GetLastError());
}
