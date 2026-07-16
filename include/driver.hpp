#pragma once

#include <memory>
#include "optimizer.hpp"
#include "kaleidoscopeJIT.hpp"

namespace ast{
    class CodeGenerator;
}

namespace llvm {
    class Value;
    class Module;
    class LLVMContext;
    class Function;
}

class Driver {
    private:
        std::unique_ptr<llvm::orc::KaleidoscopeJIT> JIT;
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;

        void handle_extern(ast::CodeGenerator&);
        void handle_function(llvm::Function*,ast::CodeGenerator&,opt::Optimizer&);
        void handle_definition(ast::CodeGenerator&,opt::Optimizer&);
        void handle_top_level_expr(ast::CodeGenerator&,opt::Optimizer&);
        void reinitialize_module();
        
    public:
        Driver();
        ~Driver();
        void repl();
};