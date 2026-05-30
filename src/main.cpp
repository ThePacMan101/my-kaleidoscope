#include <iostream>
#include "ast.hpp"
#include "lexer.hpp"

int main(void){
    auto expr = ast::NumberExpr(4.2);
    (void) lexer::get_tok();
    std::cout << "Hello, World!\n";
    return 0;
}