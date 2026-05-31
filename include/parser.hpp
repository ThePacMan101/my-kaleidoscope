#pragma once
#include "memory"
#include "ast.hpp"

namespace parser{
    int advance();
    int peek();
    std::unique_ptr<ast::Expr> expression();
    std::unique_ptr<ast::Expr> binary_operator_remainder(int,std::unique_ptr<ast::Expr>);
    std::unique_ptr<ast::Expr> number();
    std::unique_ptr<ast::Expr> paren();
    std::unique_ptr<ast::Expr> identifier();
    std::unique_ptr<ast::Expr> primary();
} 
