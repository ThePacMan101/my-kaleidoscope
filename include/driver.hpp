#pragma once

#include <memory>
#include "optimizer.hpp"
#include "kaleidoscopeJIT.hpp"
#include "ast.hpp"

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
        std::unordered_map<std::string,std::unique_ptr<ast::Prototype>> function_prototypes;

        void handle_extern();
        void handle_function(llvm::Function*);
        void handle_definition();
        void handle_top_level_expr();
        void reinitialize_module();
        void reinitialize_context();
        
    public:
        Driver();
        ~Driver();
        void repl();
};