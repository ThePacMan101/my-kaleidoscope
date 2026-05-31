#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <array>
#include <iostream>

namespace parser{

static int curr_tok;
consteval std::array<int,256> build_precedence_data(){
    std::array<int,256> table{};
    for(int& val: table) val = -1;

    table['<'] = 10;
    table['+'] = 20;
    table['-'] = 20;
    table['*'] = 40;
    
    return table;
}
constexpr std::array<int,256> binary_operator_precedence = build_precedence_data();

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

static int get_tok_precedence(){
    if(!isascii(curr_tok))
        return -1;

    int token_precedence = binary_operator_precedence[curr_tok];
    if(token_precedence <= 0) return -1;
    return token_precedence;
}


static std::unique_ptr<ast::Expr> expression(){
    // TODO
    return nullptr;
}

static std::unique_ptr<ast::Expr> number(){
    // curr token is a number literal
    auto result = std::make_unique<ast::NumberExpr>(
        lexer::get_num_val()
    );
    get_next_token();
    return std::move(result);
}

static std::unique_ptr<ast::Expr> paren(){
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
    std::vector<std::unique_ptr<ast::Expr>> args;
    if(curr_tok != ')'){
        while(true){
            if(auto arg = expression())
                args.push_back(std::move(arg));
            else
                return nullptr;
            
            if(curr_tok == ')') break;

            if(curr_tok != ',') return log_error("Expected ')' or ',' in argument list.");
            get_next_token();
        }
    }   
    get_next_token(); // skip ')'

    return std::make_unique<ast::CallExpr>(identifier_name, std::move(args));
}

static std::unique_ptr<ast::Expr> primary(){
    switch (curr_tok){
        default: 
            return log_error("Unknown token when expecting an expression.");
        case lexer::tok_identifier:
            return identifier();    
        case lexer::tok_number:
            return number();    
        case '(':
            return paren();    
    }
}



}