#include "parser.hpp"
#include "lexer.hpp"

namespace tests{

static void handle_definition() {
  if (parser::definition()) {
    fprintf(stdout, "Parsed a function definition.\n");
  } else {
    // Skip token for error recovery.
    parser::advance();
  }
}

static void handle_extern() {
  if (parser::extern_()) {
    fprintf(stdout, "Parsed an extern\n");
  } else {
    // Skip token for error recovery.
    parser::advance();
  }
}

static void handle_top_level_expr() {
  // Evaluate a top-level expression into an anonymous function.
  if (parser::top_level_expr()) {
    fprintf(stdout, "Parsed a top-level expr\n");
  } else {
    // Skip token for error recovery.
    parser::advance();
  }
}

void repl(){
    // Prime the first token.
    fprintf(stdout, "ready> ");
    parser::advance();
    while (true) {
        fprintf(stdout, "ready> ");
        switch (parser::peek()) {
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
