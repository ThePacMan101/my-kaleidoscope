#include <iostream>
#include "astPrinter.hpp"
#include "ast.hpp"

namespace ast{

void Printer::print_ident() const {
    for(int i = 0 ; i < indentation ; ++i)
        std::cout << "  ";
}

void Printer::print(Expr* node){
    indentation = 0;
    if(node) 
        node->accept(*this);
    else    
        std::cout << "(void)\n";
}

void Printer::print(Function* node){
    indentation = 0;
    if(node) 
        node->accept(*this);
    else    
        std::cout << "(void)\n";
}

void Printer::print(Prototype* node){
    indentation = 0;
    if(node) 
        node->accept(*this);
    else    
        std::cout << "(void)\n";
}

void Printer::visit(NumberExpr& node) {
    print_ident();
    std::cout << "NumberExpr (" << node.get_val() << ")\n";
}

void Printer::visit(BinaryExpr& node){
    print_ident();
    std::cout << "BinaryExpr '" << node.get_op() <<"'\n";
    indentation++;
    if(node.get_left()) node.get_left()->accept(*this);
    if(node.get_right()) node.get_right()->accept(*this);
    indentation--;
}

void Printer::visit(VariableExpr& node){
    print_ident();
    std::cout << "VariableExpr (" << node.get_name() <<")\n";
}

void Printer::visit(CallExpr& node){
    print_ident();
    std::cout << "CallExpr: " << node.get_callee() << " (";
    auto& args = node.get_args();
    if(args.size() > 0){
        std::cout<<"\n";
        indentation++;
        for(auto& arg: args) arg->accept(*this);
        indentation--;
    }
    print_ident();
    std::cout << ")\n";
}

void Printer::visit(IfExpr& node){
    auto cond  = node.get_condition();
    auto then  = node.get_then();
    auto else_ = node.get_else();

    print_ident();
    std::cout << "IfExpr: [\n";
    indentation++;

    print_ident();
    std::cout << "IF(";
    indentation++;
    cond->accept(*this);
    indentation--;
    std::cout << ")\n";

    print_ident();
    std::cout << "THEN(";
    indentation++;
    then->accept(*this);
    indentation--;
    std::cout << ")\n";

    print_ident();
    std::cout << "ELSE(";
    indentation++;
    else_->accept(*this);
    indentation--;
    std::cout << ")\n";

    print_ident();
    indentation--;
    std::cout << "]\n";
}

void Printer::visit(Prototype& node){
    print_ident();
    std::cout << "Prototype: " << node.get_name() << " (";
    auto& args = node.get_args();
    if(args.size() > 0){
        std::cout<<"\n";
        indentation++;
        for(auto& arg : args){
            print_ident();
            std::cout << arg << "\n";
        }
        indentation--;
    }
    print_ident();
    std::cout << ")\n";
}

void Printer::visit(Function& node){
    print_ident();
    auto proto = node.get_prototype();
    std::cout << "Function: " << proto->get_name() << " (";
    auto& args = proto->get_args();
    if(args.size() > 0){
        std::cout<<"\n";
        indentation++;
        for(auto& arg : args){
            print_ident();
            std::cout << arg << "\n";
        }
        indentation--;
    }
    print_ident();
    std::cout << ") Body: {\n";
    indentation++;
    node.get_body()->accept(*this);
    indentation--;
    print_ident();
    std::cout << "}\n";
}

}