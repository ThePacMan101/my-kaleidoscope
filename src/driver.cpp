#include "parser.hpp"
#include "lexer.hpp"
#include "astPrinter.hpp"
#include "codegen.hpp"
#include "optimizer.hpp"
#include "driver.hpp"
#include "kaleidoscopeJIT.hpp"
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

Driver::Driver(): 
context {std::make_unique<llvm::LLVMContext>()},
module {std::make_unique<llvm::Module>("Kaleidoscope",*context)} {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    auto expected_JIT = llvm::orc::KaleidoscopeJIT::Create();
    if(!expected_JIT){
        llvm::errs() << "Failed to create JIT: " << expected_JIT.takeError() << "\n";
        exit(1);
    }
    JIT = std::move(expected_JIT.get());
    
    module->setDataLayout(JIT->getDataLayout());
}

Driver::~Driver() = default;

void Driver::handle_function(llvm::Function* ast_IR,ast::CodeGenerator& code_generator,opt::Optimizer& code_optimizer){
    fprintf(stdout, "-----------------------------\n");        
    fprintf(stdout, "------ Unoptimized  IR ------\n");        
    fprintf(stdout, "-----------------------------\n");        
    ast_IR->print(llvm::errs());

    // run the opt passes on the function
    code_optimizer.optimize(ast_IR);
    
    fprintf(stdout, "-----------------------------\n");        
    fprintf(stdout, "------- Optimized  IR -------\n");
    fprintf(stdout, "-----------------------------\n");        
    ast_IR->print(llvm::errs()); 
}

void Driver::handle_definition(ast::CodeGenerator& code_generator,opt::Optimizer& code_optimizer){
    if (auto ast = parser::definition()){
        code_generator.visit(*ast);
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            fprintf(stdout, "Parsed a function definition.\n");
            ast::Printer().print(ast.get());
            
            handle_function(static_cast<llvm::Function*>(ast_IR),code_generator,code_optimizer);

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

void Driver::handle_top_level_expr(ast::CodeGenerator& code_generator,opt::Optimizer& code_optimizer){
    // Evaluate a top-level expression into an anonymous function.
    if (auto ast = parser::top_level_expr()){
        code_generator.visit(*ast);
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            fprintf(stdout, "Parsed a top-level expr\n");
            ast::Printer().print(ast.get());

            handle_function(static_cast<llvm::Function*>(ast_IR),code_generator,code_optimizer);
            
            static_cast<llvm::Function*>(ast_IR)->eraseFromParent();
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::repl(){
    auto code_generator = ast::CodeGenerator(*context,*module);
    auto code_optimizer = opt::Optimizer(*context);
    
    fprintf(stdout, "ready> ");
    // Prime the first token.
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
            handle_definition(code_generator,code_optimizer);
            break;
        case lexer::tok_extern:
            handle_extern(code_generator);
            break;
        default:
            handle_top_level_expr(code_generator,code_optimizer);
            break;
        }
    }
    fprintf(stdout, "-----------------------------\n");
    module->print(llvm::errs(),nullptr);
}

