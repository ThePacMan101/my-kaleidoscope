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
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

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

void Driver::reinitialize_module(){
    module  = std::make_unique<llvm::Module>("Kaleidoscope",*context);
    module->setDataLayout(JIT->getDataLayout());
}

void Driver::reinitialize_context(){
    context = std::make_unique<llvm::LLVMContext>();
}

void Driver::handle_function(llvm::Function* ast_IR){
    opt::Optimizer(*context).optimize(ast_IR);
    ast_IR->print(llvm::errs()); 
}

void Driver::handle_definition(){
    if (auto ast = parser::definition()){
        ast::CodeGenerator code_generator(*context,*module,function_prototypes);
        code_generator.visit(*ast);
        
        // adds function to the map
        function_prototypes[
            ast->get_prototype()->get_name()
        ] = std::make_unique<ast::Prototype>(*ast->get_prototype());
        
        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            
            handle_function(static_cast<llvm::Function*>(ast_IR));

            auto thread_safe_module = llvm::orc::ThreadSafeModule(std::move(module),std::move(context));
            reinitialize_context();
            reinitialize_module();
            
            // hand to the JIT
            if(auto err = JIT->addModule(std::move(thread_safe_module))){
                llvm::errs() << "JIT error: " << err << "\n";
            }
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::handle_extern() {
    if (auto ast = parser::extern_()){
        ast::CodeGenerator code_generator(*context,*module,function_prototypes);
        code_generator.visit(*ast);

        // adds function to the map
        function_prototypes[
            ast->get_name()
        ] = std::make_unique<ast::Prototype>(*ast);

        auto* ast_IR = code_generator.get_current_value();
        if(ast_IR){
            ast_IR->print(llvm::errs());
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::handle_top_level_expr(){
    // Evaluate a top-level expression into an anonymous function.
    if (auto ast = parser::top_level_expr()){
        llvm::Function* ast_IR = nullptr;
        {
            ast::CodeGenerator code_generator(*context,*module,function_prototypes);
            code_generator.visit(*ast);
            ast_IR = static_cast<llvm::Function*>(code_generator.get_current_value());
        }
        if(ast_IR){

            handle_function(ast_IR);

            auto resource_tracker = JIT->getMainJITDylib().createResourceTracker();
            auto thread_safe_module = llvm::orc::ThreadSafeModule(std::move(module),std::move(context));
            reinitialize_context();
            reinitialize_module();
            
            // hand to the JIT
            if(auto err = JIT->addModule(std::move(thread_safe_module),resource_tracker)){
                llvm::errs() << "JIT Error (addModule): " << err << "\n";
                return;
            }

            auto expected_expr_symbol = JIT->lookup("__anon_expr");
            if(!expected_expr_symbol){
                llvm::errs() << "JIT Error (lookup): " << expected_expr_symbol.takeError() << "\n";
                if(auto err = resource_tracker->remove())
                    llvm::errs() << "JIT Error (clenup): " << err << "\n";
                return;
            }
            auto expr_symbol = expected_expr_symbol.get();
            // execute compiled code
            double (*compiled_func)() = expr_symbol.toPtr<double (*)()>();
            fprintf(stdout, "\n> %f\n", compiled_func());

            // remove the anon function
            if(auto err = resource_tracker->remove()){
                llvm::errs() << "JIT Error (remove): " << err << "\n";
            }
        }
    }
    else{
        // Skip token for error recovery.
        parser::advance();
    }
}

void Driver::repl(){
    fprintf(stdout, "ready> ");
    // Prime the first token.    
    parser::advance();
    while (true){
        fprintf(stdout, "\nready> ");
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
    fprintf(stdout, "-----------------------------\n");
    module->print(llvm::errs(),nullptr);
}

