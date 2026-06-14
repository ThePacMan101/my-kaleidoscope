#include "parser.hpp"
#include "lexer.hpp"
#include "astPrinter.hpp"
#include "codegen.hpp"
#include "driver.hpp"
#include "llvm/IR/Module.h"

Driver::Driver(){
    context = std::make_unique<llvm::LLVMContext>();
    module  = std::make_unique<llvm::Module>("Kaleidoscope",*context);
}

Driver::~Driver() = default;

void Driver::handle_definition(ast::CodeGenerator& code_generator){
    if (auto ast = parser::definition()){
        code_generator.visit(*ast);
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            fprintf(stdout, "Parsed a function definition.\n");
            ast::Printer().print(ast.get());
            fprintf(stdout, "-----------------------------\n");        
            ast_IR->print(llvm::errs());
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::handle_extern(ast::CodeGenerator& code_generator) {
    if (auto ast = parser::extern_()){
        code_generator.visit(*ast);
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            fprintf(stdout, "Parsed an extern\n");
            ast::Printer().print(ast.get());
            fprintf(stdout, "-----------------------------\n");
            ast_IR->print(llvm::errs());
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::handle_top_level_expr(ast::CodeGenerator& code_generator){
    // Evaluate a top-level expression into an anonymous function.
    if (auto ast = parser::top_level_expr()){
        code_generator.visit(*ast);
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            fprintf(stdout, "Parsed a top-level expr\n");
            ast::Printer().print(ast.get());
            fprintf(stdout, "-----------------------------\n");
            ast_IR->print(llvm::errs());
            static_cast<llvm::Function*>(ast_IR)->eraseFromParent();
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::repl(){
    // Prime the first token.
    auto code_generator = ast::CodeGenerator(*context,*module);

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
            handle_definition(code_generator);
            break;
        case lexer::tok_extern:
            handle_extern(code_generator);
            break;
        default:
            handle_top_level_expr(code_generator);
            break;
        }
    }
    fprintf(stdout, "-----------------------------\n");
    module->print(llvm::errs(),nullptr);
}

