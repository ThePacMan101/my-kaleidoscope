#include <iostream>
#include "parser.hpp"
#include "lexer.hpp"

int main(void){
    auto expr = NumberExprAST(4.2);
    (void) lexer::get_tok();
    std::cout << "Hello, World!\n";
    return 0;
}