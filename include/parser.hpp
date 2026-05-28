#pragma once

class ExprAST {
    public:
        virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
    double Val; 
    public:
        NumberExprAST(double Val) :Val {Val} {}
};