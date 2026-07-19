#pragma once
#include "ast.hpp"

namespace parser{
    int advance();
    int peek();
    std::unique_ptr<ast::Expr> expression();
    std::unique_ptr<ast::Expr> number();
    std::unique_ptr<ast::Expr> paren();
    std::unique_ptr<ast::Expr> identifier();
    std::unique_ptr<ast::Expr> primary();
    std::unique_ptr<ast::Expr> if_else();
    std::unique_ptr<ast::Prototype> prototype();
    std::unique_ptr<ast::Function> definition();
    std::unique_ptr<ast::Prototype> extern_();
    std::unique_ptr<ast::Function> top_level_expr();
} 
