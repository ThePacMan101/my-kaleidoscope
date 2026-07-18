#pragma once

#include "ast.hpp"
#include "astVisitor.hpp"


namespace ast {

class Printer : public Visitor {
    private:
        int indentation;
        void print_ident() const;
    public:
        Printer(): indentation {0} {}
        Printer(int indentation): indentation {indentation} {}
        void print(Expr* node);
        void print(Prototype* node);
        void print(Function* node);

        void visit(NumberExpr& node) override;
        void visit(VariableExpr& node) override;
        void visit(BinaryExpr& node) override;
        void visit(CallExpr& node) override;
        void visit(IfExpr& node) override;
        void visit(Prototype& node) override;
        void visit(Function& node) override;
};

}
