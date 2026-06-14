#pragma once

#include <memory>

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
        void handle_definition(ast::CodeGenerator&);
        void handle_top_level_expr(ast::CodeGenerator&);
    public:
        Driver();
        ~Driver();
        void repl();
};