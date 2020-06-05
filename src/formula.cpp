// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Returns the error code of the last evaluation.
 * @return The error code of the last evaluation.
 */
TFormulaError TFormula::GetLastError() const noexcept
{
    return this->last_error_;
}

/**
 * Returns the error of the last evaluation as string.
 * @return The error of the last evaluation as string.
 */
TString TFormula::GetLastErrorString() const
{
    if (this->last_error_ == TFormulaError::NONE) return TString("None");
    if (this->last_error_ == TFormulaError::INVALID_OPERATOR) return TString("Invalid operator");
    if (this->last_error_ == TFormulaError::INVALID_NUMBER) return TString("Invalid number");
    if (this->last_error_ == TFormulaError::EXP_INCOMPLETE) return TString("Expression incomplete");
    if (this->last_error_ == TFormulaError::DIV_BY_ZERO) return TString("Division by zero");
    return TString("Unknown Error");
}

/**
 * Evaluates the specified term.
 * @param term The string with the term to evaluate.
 * @return The result of the evaluation as 64bit integer.
 */
int64_t TFormula::Evaluate(TString term)
{
    // No part of the formula may be empty (e.g. 12+*3 is invalid)
    if (term.IsEmpty())
    {
        this->last_error_ = TFormulaError::EXP_INCOMPLETE;
        return 0;
    }

    // Removes all blanks and makes the string lower case
    term.StripChar(' ');
    term.ToLower();

    // Divide the formula into terms, starting with the "lowest" operator
    // Priority: AND, OR, MULTIPLY, DIVIDE, ADD, SUBTRACT
    auto operator_pos = term.IndexOf('-');
    if (operator_pos == -1) operator_pos = term.IndexOf('+');
    if (operator_pos == -1) operator_pos = term.IndexOf('/');
    if (operator_pos == -1) operator_pos = term.IndexOf('*');
    if (operator_pos == -1) operator_pos = term.IndexOf('|');
    if (operator_pos == -1) operator_pos = term.IndexOf('&');

    // No operator in expression -> operand found
    if (operator_pos < 0) return this->EvaluateOperand(term);

    // Get the operator
    const char operator_char = term[static_cast<std::size_t>(operator_pos)];

    // Extract the left part of the formula
    TString left_term = term;
    left_term[static_cast<std::size_t>(operator_pos)] = 0;
    // Extract the right part of the formula
    TString right_term = TString(term.SubString(static_cast<std::size_t>(operator_pos + 1)));

    // Recursively compute both parts of the formula
    const auto left_value = this->Evaluate(left_term);
    if (this->last_error_ != TFormulaError::NONE) return 0;
    const auto right_value = this->Evaluate(right_term);
    if (this->last_error_ != TFormulaError::NONE) return 0;

    // Perform operation based on operator
    if (operator_char == '&') return (left_value & right_value);
    if (operator_char == '|') return (left_value | right_value);
    if (operator_char == '*') return (left_value * right_value);
    if (operator_char == '/')
    {
        if (right_value != 0)
        {
            return (left_value / right_value);
        }
        else
        {
            this->last_error_ = TFormulaError::DIV_BY_ZERO;
            return 0;
        }
    }
    if (operator_char == '+') return (left_value + right_value);
    if (operator_char == '-') return (left_value - right_value);

    // Invalid operator
    this->last_error_ = TFormulaError::INVALID_OPERATOR;
    return 0;
}

/**
 * Evaluates the specified operand (numerical constant).
 * @param term The string with the operand to evaluate.
 * @return The result of the evaluation as 64bit integer.
 */
int64_t TFormula::EvaluateOperand(TString term)
{
    // Get length of the operand
    const auto length = term.Length();

    // Validate length of operand
    if (length == 0)
    {
        this->last_error_ = TFormulaError::INVALID_NUMBER;
        return 0;
    }

    // Assume the operand is a decimal number
    auto operand_type = TFormulaOperand::DEC;

    // Check for prefix or suffix (indicating decimal, hex-decimal or binary number)
    if (term.Equals("0x", 2))
    {
        operand_type = TFormulaOperand::HEX;
        TString temp = TString(term.SubString(2));
        term = temp;
    }
    else if (term[length - 1] == 'd')
    {
        operand_type = TFormulaOperand::DEC;
        term[length - 1] = 0;
    }
    else if (term[length - 1] == 'h')
    {
        operand_type = TFormulaOperand::HEX;
        term[length - 1] = 0;
    }
    else if (term[length - 1] == 'b')
    {
        operand_type = TFormulaOperand::BIN;
        term[length - 1] = 0;
    }

    // Validate length of the remaining number
    if (term.Length() == 0)
    {
        this->last_error_ = TFormulaError::INVALID_NUMBER;
        return 0;
    }

    // No operand value found yet
    auto valid = false;
    this->last_error_ = TFormulaError::INVALID_NUMBER;
    int64_t operand_value = 0;

    if (operand_type == TFormulaOperand::HEX)
    {
        operand_value = term.ParseHex(&valid);
    }
    else if (operand_type == TFormulaOperand::BIN)
    {
        operand_value = term.ParseBin(&valid);
    }
    else  // Only decimal numbers remaining
    {
        operand_value = term.ParseDec(&valid);
    }

    // Set last error
    if (valid == true) this->last_error_ = TFormulaError::NONE;

    // Return the operand value
    return operand_value;
}
