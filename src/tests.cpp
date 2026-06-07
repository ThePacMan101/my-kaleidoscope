#include "parser.hpp"
#include "lexer.hpp"
#include "astPrinter.hpp"

namespace tests{
static void handle_definition(){
    if (auto ast = parser::definition()){
        fprintf(stdout, "Parsed a function definition.\n");
        ast::Printer().print(ast.get());        
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

static void handle_extern() {
    if (auto ast = parser::extern_()){
        fprintf(stdout, "Parsed an extern\n");
        ast::Printer().print(ast.get());
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

static void handle_top_level_expr(){
    // Evaluate a top-level expression into an anonymous function.
    if (auto ast = parser::top_level_expr()){
        fprintf(stdout, "Parsed a top-level expr\n");
        ast::Printer().print(ast.get());
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void repl(){
    // Prime the first token.
    fprintf(stdout, "ready> ");
    parser::advance();
    while (true){
        fprintf(stdout, "ready> ");
        switch (parser::peek()){
        case lexer::tok_eof:
            return;
        case ';': // ignore top-level semicolons.
            parser::advance();
            break;
        case lexer::tok_def:
            handle_definition();
            break;
        case lexer::tok_extern:
            handle_extern();
            break;
        default:
            handle_top_level_expr();
            break;
        }
    }
}

}
