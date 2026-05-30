#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <iostream>

namespace parser{

static int curr_tok;

static int get_next_token(){
    return curr_tok = lexer::get_tok();
}

static std::unique_ptr<ast::Expr> log_error(const char * str){
    fprintf(stderr,"[error]: %s\n",str);
    return nullptr;
}

static std::unique_ptr<ast::Prototype> log_prototype_error(const char* str){
    log_error(str);
    return nullptr;
}

static std::unique_ptr<ast::Expr> number_expr(){
    // curr token is a number literal
    auto result = std::make_unique<ast::NumberExpr>(
        lexer::get_num_val()
    );
    get_next_token();
    return std::move(result);
}

static std::unique_ptr<ast::Expr> paren_expr(){
    // curr token is a '('
    get_next_token(); // skip '('
    auto expr = expression();
    if(!expr) return nullptr;
    if(curr_tok!=')') return log_error("expected ')'.");
    get_next_token(); // skip ')'
    return expr;
}

static std::unique_ptr<ast::Expr> identifier(){
    // curr_tok is an identifier
    std::string identifier_name = lexer::get_identifier_str();
    get_next_token(); // skip the identifer
    if(curr_tok != '(') 
        // when I parse some identifier, if I don't call it, I presume it's just a variable
        return std::make_unique<ast::VariableExpr>(identifier_name);
    get_next_token(); // skip '('
    ast::Expr* args;
    


}

static std::unique_ptr<ast::Expr> expression(){
    // switch (curr_tok){
    //     case lexer::tok_eof:
    //     case lexer::tok_def:
    //     case lexer::tok_extern:
    //     case lexer::tok_identifier:
    //     case lexer::tok_number:
    // }
}

}