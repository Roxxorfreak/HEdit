// Copyright (c) 2021 Roxxorfreak

#ifndef HEDIT_SRC_FORMULA_HPP_

    // Header included
    #define HEDIT_SRC_FORMULA_HPP_

    // The operand types (possible number formats)
    enum class TFormulaOperand : int32_t {
        DEC,
        HEX,
        BIN
    };

    // The errors that may occur during the evaluation of the formula
    enum class TFormulaError : int32_t {
        NONE,
        INVALID_OPERATOR,
        INVALID_NUMBER,
        EXP_INCOMPLETE,
        DIV_BY_ZERO
    };

    /**
     * @brief The class that is used to evaluate the formulas from the formula editor.
     */
    class TFormula
    {
    private:
        TFormulaError last_error_ = { TFormulaError::NONE };  //!< The error code of the last operation.
    public:
        TFormulaError GetLastError() const noexcept;
        TString GetLastErrorString() const;
        int64_t Evaluate(TString term);
    private:
        int64_t EvaluateOperand(TString term);
    };

#endif  // HEDIT_SRC_FORMULA_HPP_
