#pragma once

#include <memory>
#include "optimizer.hpp"

namespace ast{
    class CodeGenerator;
}

namespace llvm {
    class Value;
    class Module;
    class LLVMContext;
}

class Driver {
    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;

        void handle_extern(ast::CodeGenerator&);
        void handle_definition(ast::CodeGenerator&,opt::Optimizer&);
        void handle_top_level_expr(ast::CodeGenerator&,opt::Optimizer&);
    public:
        Driver();
        ~Driver();
        void repl();
};