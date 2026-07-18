#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "logger.hpp"
#include <array>
#include <iostream>
#include <memory>

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
// ===========================================================================================
// helpers
int advance();
int peek();
static int get_precedence(int);
static std::unique_ptr<ast::Expr> binary_operator_remainder(int,std::unique_ptr<ast::Expr>);
// ===========================================================================================
// parsers
std::unique_ptr<ast::Expr> expression();
std::unique_ptr<ast::Expr> number();
std::unique_ptr<ast::Expr> paren();
std::unique_ptr<ast::Expr> identifier();
std::unique_ptr<ast::Expr> primary();
std::unique_ptr<ast::Prototype> prototype();
std::unique_ptr<ast::Function> definition();
std::unique_ptr<ast::Prototype> extern_();
std::unique_ptr<ast::Function> top_level_expr();
// ===========================================================================================

int advance(){
    return curr_tok = lexer::get_tok();
}

int peek(){
    return curr_tok;
}

static int get_precedence(int tok){
    if(!isascii(tok))
        return -1;

    int token_precedence = binary_operator_precedence[tok];
    if(token_precedence <= 0) return -1;
    return token_precedence;
}

std::unique_ptr<ast::Expr> expression(){
    auto left = primary();
    if(!left) return nullptr;

    return binary_operator_remainder(0,std::move(left));
}

std::unique_ptr<ast::Expr> binary_operator_remainder
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
        advance(); // skips the operator

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

std::unique_ptr<ast::Expr> number(){
    // curr token is a number literal
    auto result = std::make_unique<ast::NumberExpr>(
        lexer::get_num_val()
    );
    advance();
    return std::move(result);
}

std::unique_ptr<ast::Expr> paren(){
    // curr token is a '('
    advance(); // skip '('
    auto expr = expression();
    if(!expr) return nullptr;
    if(curr_tok!=')') return logger::error<std::unique_ptr<ast::Expr>>("Expected ')'");
    advance(); // skip ')'
    return expr;
}

std::unique_ptr<ast::Expr> identifier(){
    // curr_tok is an identifier
    std::string identifier_name = lexer::get_identifier_str();
    advance(); // skip the identifer
    if(curr_tok != '(') 
        // when I parse some identifier, if I don't call it, I presume it's just a variable
        return std::make_unique<ast::VariableExpr>(identifier_name);
    advance(); // skip '('
    std::vector<std::unique_ptr<ast::Expr>> args;
    if(curr_tok != ')'){
        while(true){
            if(auto arg = expression())
                args.push_back(std::move(arg));
            else
                return nullptr;
            
            if(curr_tok == ')') break;

            if(curr_tok != ',') return logger::error<std::unique_ptr<ast::Expr>>("Expected ')' or ',' in argument list");
            advance();
        }
    }   
    advance(); // skip ')'

    return std::make_unique<ast::CallExpr>(identifier_name, std::move(args));
}

std::unique_ptr<ast::Expr> primary(){
    switch (curr_tok){
        default: 
            return logger::error<std::unique_ptr<ast::Expr>>("Unknown token when expecting an expression");
        case lexer::tok_identifier:
            return identifier();    
        case lexer::tok_number:
            return number();    
        case '(':
            return paren();
        case lexer::tok_if:
            return if_else();
    }
}

std::unique_ptr<ast::Expr> if_else(){
    advance(); // skip "if"
    
    auto cond = expression();
    if(!cond) 
        return logger::error<std::unique_ptr<ast::Expr>>("Expected expression after \"if\"");
    if(curr_tok != lexer::tok_then)
        return logger::error<std::unique_ptr<ast::Expr>>("Expected \"then\" after \"if\" condition");
    advance(); // skip "then"
    
    auto then = expression();
    if(!then) 
        return logger::error<std::unique_ptr<ast::Expr>>("Expected expression after \"then\"");
    if(curr_tok != lexer::tok_else)
        return logger::error<std::unique_ptr<ast::Expr>>("Expected \"else\" after \"then\" expression");
    advance(); // skip "else"

    auto else_ = expression();
    if(!else_)
        return logger::error<std::unique_ptr<ast::Expr>>("Expected expression after \"else\"");
    
    return std::make_unique<ast::IfExpr>(std::move(cond),std::move(then),std::move(else_));
}

std::unique_ptr<ast::Prototype> prototype(){
    if(curr_tok != lexer::tok_identifier)
        return logger::error<std::unique_ptr<ast::Prototype>>("Expected function name in prototype");

    std::string fn_name = lexer::get_identifier_str();
    
    advance(); // skip function name

    if(curr_tok != '(')
        return logger::error<std::unique_ptr<ast::Prototype>>("Expected '(' after function name in prototype");
    
    std::vector<std::string> arg_names;
    while(advance() == lexer::tok_identifier)
        arg_names.push_back(lexer::get_identifier_str());
    if(curr_tok != ')')
        return logger::error<std::unique_ptr<ast::Prototype>>("Expected ')' after function arguments in protype");
    
    advance(); // skip ')'

    return std::make_unique<ast::Prototype>(fn_name,std::move(arg_names));
}

std::unique_ptr<ast::Function> definition(){
    advance(); // skip 'def'
    auto proto = prototype();
    if(!proto) return nullptr;
    if(auto expr = expression())
        return std::make_unique<ast::Function>(std::move(proto),std::move(expr));
    return nullptr;
}

std::unique_ptr<ast::Prototype> extern_(){
    advance(); // skip 'extern'
    return prototype();
}

std::unique_ptr<ast::Function> top_level_expr(){
    // we need top level expressions in order
    // to be able to type expressions outside functions
    if(auto expr = expression()){
        // make a fake anonymous function and then just return the expression result
        auto proto = std::make_unique<ast::Prototype>("__anon_expr",std::vector<std::string>());
        return std::make_unique<ast::Function>(std::move(proto),std::move(expr));
    }
    return nullptr;
}

}