#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <iostream>

namespace parser{

static int curr_tok;

static int get_next_token(){
    return curr_tok = lexer::get_tok();
}

static std::unique_ptr<ast::ExprAST> log_error(const char * str){
    fprintf(stderr,"[error]: %s\n",str);
    return nullptr;
}

static std::unique_ptr<ast::PrototypeAST> log_prototype_error(const char* str){
    log_error(str);
    return nullptr;
}

static std::unique_ptr<ast::ExprAST> number_expr(){
    // curr token is a number
    auto result = std::make_unique<ast::NumberExprAST>(
        lexer::get_num_val()
    );
    get_next_token();
    return std::move(result);
}

static std::unique_ptr<ast::ExprAST> paren_expr(){
    get_next_token(); // skip '('
    auto expr = expression();
    if(!expr) return nullptr;
    if(curr_tok!=')') return log_error("expected ')'.");
    get_next_token(); // skip ')'
    return expr;
}

static std::unique_ptr<ast::ExprAST> identifier(){

}

static std::unique_ptr<ast::ExprAST> expression(){
    // switch (curr_tok){
    //     case lexer::tok_eof:
    //     case lexer::tok_def:
    //     case lexer::tok_extern:
    //     case lexer::tok_identifier:
    //     case lexer::tok_number:
    // }
}

}