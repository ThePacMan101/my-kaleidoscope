#pragma once
#include <memory>
#include <vector>

namespace ast{

class Expr {
    public:
        virtual ~Expr() = default;
};

class NumberExpr : public Expr {
    double Val; 
    public:
        NumberExpr(double Val) :Val {Val} {}
};

class VariableExpr : public Expr{
    std::string name;
    public:
        VariableExpr(const std::string& name): name {name} {}
};

class BinaryExpr : public Expr{
    char op;
    std::unique_ptr<Expr> left,right;
    public:
        BinaryExpr(
            const char& op,
            std::unique_ptr<Expr> left,
            std::unique_ptr<Expr> right
        ) : op {op}, left{std::move(left)}, right{std::move(right)} {}
};

class CallExpr : public Expr{
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    public:
        CallExpr(
            const std::string& callee,
            std::vector<std::unique_ptr<Expr>> args
        ) : callee {callee}, args {std::move(args)} {}
};

// this represents only the interface to a function
class Prototype {
    std::string name;
    std::vector<std::string> args;
    public:
        Prototype(const std::string& name, std::vector<std::string> args) 
        : name {name}, args {std::move(args)} {}

        const std::string& get_name() const {return name; }
};

// this represents the function itself
class Function {
    std::unique_ptr<Prototype> proto;
    std::unique_ptr<Expr> body;
    public:
        Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
        : proto {std::move(proto)}, body {std::move(body)} {}
};



}
