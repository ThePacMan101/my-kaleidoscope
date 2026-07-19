#pragma once
#include <memory>
#include <string>
#include <vector>

namespace ast{

class Visitor;

class Expr {
    public:
        virtual ~Expr() = default;
        virtual void accept(Visitor& visitor) = 0;  
};

class NumberExpr : public Expr {
    double val; 
    public:
        NumberExpr(double val) :val {val} {}
        double get_val() const {return val;}
        
        void accept(Visitor& visitor) override;
};

class VariableExpr : public Expr{
    std::string name;
    public:
        VariableExpr(const std::string& name): name {name} {}
        const std::string& get_name() const {return name;}
        
        void accept(Visitor& visitor) override;
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
        char  get_op()    const {return op;}
        Expr* get_left()  const {return left.get();}
        Expr* get_right() const {return right.get();}
        
        void accept(Visitor& visitor) override;
};

class CallExpr : public Expr{
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    public:
        CallExpr(
            const std::string& callee,
            std::vector<std::unique_ptr<Expr>> args
        ) : callee {callee}, args {std::move(args)} {}
        const std::string& get_callee() const {return callee;}
        const std::vector<std::unique_ptr<Expr>>& get_args() const {return args;}
        
        void accept(Visitor& visitor) override;
};

class IfExpr : public Expr{
    std::unique_ptr<Expr> condition,then,else_;
    public:
        IfExpr(
            std::unique_ptr<Expr> condition,
            std::unique_ptr<Expr> then,
            std::unique_ptr<Expr> else_
        ) : condition {std::move(condition)}, then {std::move(then)}, else_ {std::move(else_)} {};
        Expr* get_condition() const {return condition.get();}
        Expr* get_then() const {return then.get();}
        Expr* get_else() const {return else_.get();}
        
        void accept(Visitor& visitor) override;
};

class ForExpr : public Expr{
    std::string var;
    std::unique_ptr<Expr> start,end,step,body;
    public:
        ForExpr(
            std::string var,
            std::unique_ptr<Expr> start,
            std::unique_ptr<Expr> end,
            std::unique_ptr<Expr> step,
            std::unique_ptr<Expr> body
        ) : var {var}, start{std::move(start)}, end{std::move(end)}, step{std::move(step)},body{std::move(body)} {}
        const std::string& get_var() const {return var;}
        Expr* get_start() const {return start.get();}
        Expr* get_end() const {return end.get();}
        Expr* get_step() const {return step.get();}
        Expr* get_body() const {return body.get();}

        void accept(Visitor& visitor) override;
};

// this represents only the interface to a function
class Prototype {
    std::string name;
    std::vector<std::string> args;
    public:
        Prototype(const std::string& name, std::vector<std::string> args) 
        : name {name}, args {std::move(args)} {}
        const std::string& get_name() const {return name; }
        const std::vector<std::string>& get_args() const {return args;}
        
        void accept(Visitor& visitor);
};

// this represents the function itself
class Function {
    std::unique_ptr<Prototype> prototype;
    std::unique_ptr<Expr> body;
    public:
        Function(std::unique_ptr<Prototype> prototype, std::unique_ptr<Expr> body)
        : prototype {std::move(prototype)}, body {std::move(body)} {}
        Prototype* get_prototype() const {return prototype.get();}
        Expr* get_body() const {return body.get();}
        
        void accept(Visitor& visitor);
};



}
