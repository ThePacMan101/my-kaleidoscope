#pragma once
#include "ast.hpp"

namespace ast{

class Visitor{
    public:
        virtual ~Visitor() = default;
        
        virtual void visit(NumberExpr&   node) = 0;
        virtual void visit(VariableExpr& node) = 0;
        virtual void visit(BinaryExpr&   node) = 0;
        virtual void visit(CallExpr&     node) = 0;
        virtual void visit(IfExpr&       node) = 0;
        virtual void visit(ForExpr&      node) = 0;
        virtual void visit(Prototype&    node) = 0;
        virtual void visit(Function&     node) = 0;
};

}