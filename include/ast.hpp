#pragma once
#include <memory>
#include <vector>

namespace ast{

class NumberExpr;
class VariableExpr;
class BinaryExpr;
class CallExpr;
class Prototype;
class Function;

class Visitor{
    public:
        virtual ~Visitor() = default;

        virtual void visit(NumberExpr&   node) = 0;
        virtual void visit(NumberExpr&   node) = 0;
        virtual void visit(VariableExpr& node) = 0;
        virtual void visit(BinaryExpr&   node) = 0;
        virtual void visit(CallExpr&     node) = 0;
        virtual void visit(Prototype& node) = 0;
        virtual void visit(Function&  node) = 0;
};


class Expr {
    public:
        virtual ~Expr() = default;
        virtual void accept(Visitor& visitor) = 0;  
};

class NumberExpr : public Expr {
    double Val; 
    public:
        NumberExpr(double Val) :Val {Val} {}
        void accept(Visitor& visitor) override {
            visitor.visit(*this);
        }
};

class VariableExpr : public Expr{
    std::string name;
    public:
        VariableExpr(const std::string& name): name {name} {}
        void accept(Visitor& visitor) override {
            visitor.visit(*this);
        }
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
        void accept(Visitor& visitor) override {
            visitor.visit(*this);
        }
};

class CallExpr : public Expr{
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    public:
        CallExpr(
            const std::string& callee,
            std::vector<std::unique_ptr<Expr>> args
        ) : callee {callee}, args {std::move(args)} {}
        void accept(Visitor& visitor) override {
            visitor.visit(*this);
        }
};

// this represents only the interface to a function
class Prototype {
    std::string name;
    std::vector<std::string> args;
    public:
        Prototype(const std::string& name, std::vector<std::string> args) 
        : name {name}, args {std::move(args)} {}
        const std::string& get_name() const {return name; }
        void accept(Visitor& visitor) {
            visitor.visit(*this);
        }
};

// this represents the function itself
class Function {
    std::unique_ptr<Prototype> proto;
    std::unique_ptr<Expr> body;
    public:
        Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
        : proto {std::move(proto)}, body {std::move(body)} {}
        void accept(Visitor& visitor) {
            visitor.visit(*this);
        }
};



}
