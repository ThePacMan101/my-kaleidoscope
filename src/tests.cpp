#include "parser.hpp"
#include "lexer.hpp"

namespace tests{

void parser(){
    parser::advance();
    while(true) {
        fprintf(stdout, "ready> ");
        switch (parser::peek()){
            case lexer::tok_eof: return;
            case ';': parser::advance(); break;
            default:{
                if(auto expr = parser::expression()){
                    fprintf(stdout,"parsed expression!\n");
                }else{
                    parser::advance();
                }
                break;
            }
        }
    }
    (void)parser::expression();
}

}
