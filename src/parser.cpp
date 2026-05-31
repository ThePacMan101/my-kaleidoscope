#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <array>
#include <iostream>

namespace parser{
static int curr_tok;
constexpr std::array<int,256> binary_operator_precedence = []() consteval {
    std::array<int,256> table{};
    for(int& val: table) val = -1;
    
    table['>'] = 10;
    table['<'] = 10;
    table['+'] = 20;
    table['-'] = 20;
    table['*'] = 40;
    table['/'] = 40;
    
    return table;
}();

static int get_next_token();
static std::unique_ptr<ast::Expr> log_error(const char *);
static std::unique_ptr<ast::Prototype> log_prototype_error(const char*);
static int get_precedence(int);
static std::unique_ptr<ast::Expr> expression();
static std::unique_ptr<ast::Expr> binary_operator_remainder(int,std::unique_ptr<ast::Expr>);
static std::unique_ptr<ast::Expr> number();
static std::unique_ptr<ast::Expr> paren();
static std::unique_ptr<ast::Expr> identifier();
static std::unique_ptr<ast::Expr> primary();

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

static int get_precedence(int tok){
    if(!isascii(tok))
        return -1;

    int token_precedence = binary_operator_precedence[tok];
    if(token_precedence <= 0) return -1;
    return token_precedence;
}

static std::unique_ptr<ast::Expr> expression(){
    auto left = primary();
    if(!left) return nullptr;

    return binary_operator_remainder(0,std::move(left));
}

static std::unique_ptr<ast::Expr> binary_operator_remainder
    (int expr_precedence, 
    std::unique_ptr<ast::Expr> left){
    // precedence climbing algorithm implementation
    while(true){
        // check if the current token's precedence is too low
        // this could happen both when it's an invalid token 
        // (ex: EOF) or when the operator's precedence 
        // is indeed lower. In both cases, we return left
        int curr_precedence = get_precedence(curr_tok);
        if(curr_precedence < expr_precedence) return left;
        
        // if we get here, we are certain we see a operator
        int current_binary_operator = curr_tok;
        get_next_token(); // skips the operator

        // now we need to parse what's on the right
        // notice that this could be a parenthesized 
        // expression so we respect the fact that 
        // parenthesized expressions have maximum precedence
        auto right = primary();
        if(!right) return nullptr;

        // after we have the primaries on the right and on the left, we
        // need to decice whether we bind the rhs expression to the
        // lhs directly using our current binary operator or if the rhs
        // should still be bound to a further rhs expression
        // to check that, we need to see the next binary operator
        int next_binary_operator = curr_tok;
        int next_precedence = get_precedence(next_binary_operator);
        if(curr_precedence < next_precedence){
            // if indeed we need to bind the rhs to the further rhs expression, we
            // do so by recursively calling binary_operator_remainder
  
            right = std::move(binary_operator_remainder(curr_precedence+1,std::move(right)));
            if(!right) return nullptr;
            // once this ends, we are sure the rhs should bind to the current lhs
        }
        left = std::make_unique<ast::BinaryExpr>(
            current_binary_operator,
            std::move(left),
            std::move(right)
        );
    }
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