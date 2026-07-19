#include "ast.hpp"
#include "astVisitor.hpp"

namespace ast {

void NumberExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void VariableExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void BinaryExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void CallExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void IfExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void ForExpr::accept(Visitor& visitor){
    visitor.visit(*this);
}
void Prototype::accept(Visitor& visitor){
    visitor.visit(*this);
}
void Function::accept(Visitor& visitor){
    visitor.visit(*this);
}

}