#pragma once
#include <memory>
#include <vector>

namespace ast{

class ExprAST {
    public:
        virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
    double Val; 
    public:
        NumberExprAST(double Val) :Val {Val} {}
};

class VariableExprAST : public ExprAST{
    std::string name;
    public:
        VariableExprAST(const std::string& name): name {name} {}
};

class BinaryExprAST : public ExprAST{
    char op;
    std::unique_ptr<ExprAST> left,right;
    public:
        BinaryExprAST(
            const char& op,
            std::unique_ptr<ExprAST> left,
            std::unique_ptr<ExprAST> right
        ) : op {op}, left{std::move(left)}, right{std::move(right)} {}
};

class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
    public:
        CallExprAST(
            const std::string& callee,
            std::vector<std::unique_ptr<ExprAST>> args
        ) : callee {callee}, args {std::move(args)} {}
};

// this represents only the interface to a function
class PrototypeAST {
    std::string name;
    std::vector<std::string> args;
    public:
        PrototypeAST(const std::string& name, std::vector<std::string> args) 
        : name {name}, args {std::move(args)} {}

        const std::string& get_name() const {return name; }
};

// this represents the function itself
class FunctionAST {
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;
    public:
        FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
        : proto {std::move(proto)}, body {std::move(body)} {}
};



}
