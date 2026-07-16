#pragma once
#include <llvm/IR/IRBuilder.h>

#include <memory>
#include <string>
#include <map>
#include "astVisitor.hpp"

namespace llvm {
    class Value;
    class Module;
    class LLVMContext;
}

namespace ast {
    
class CodeGenerator : public ast::Visitor {
    private:
        std::unique_ptr<llvm::IRBuilder<>> builder;

        llvm::LLVMContext& context;
        llvm::Module& module;
        std::map<std::string, llvm::Value *> named_values;
        const std::unordered_map<std::string, std::unique_ptr<Prototype>>& function_prototypes;

        // auxiliary for return values from visit methods
        llvm::Value* current_value = nullptr;

    public:
        CodeGenerator(
            llvm::LLVMContext&,
            llvm::Module&,
            const std::unordered_map<std::string, std::unique_ptr<ast::Prototype>>&);
        ~CodeGenerator();

        llvm::Value* get_current_value() const {return current_value;}

        void visit(NumberExpr& node) override;
        void visit(VariableExpr& node) override;
        void visit(BinaryExpr& node) override;
        void visit(CallExpr& node) override;
        void visit(Prototype& node) override;
        void visit(Function& node) override;
    
};

}
