#pragma once

#include <memory>

// aux namespace to avoid include pollution
namespace llvm {
    class Function;
    class LLVMContext;
}

namespace opt {

class Optimizer{
    private:
        std::unique_ptr<llvm::FunctionPassManager> function_pass_manager;
        std::unique_ptr<llvm::LoopAnalysisManager> loop_analysis_manager;
        std::unique_ptr<llvm::FunctionAnalysisManager> function_analysis_manager;
        std::unique_ptr<llvm::CGSCCAnalysisManager> CGSCC_analysis_manager;
        std::unique_ptr<llvm::ModuleAnalysisManager> module_analysis_manager;

        std::unique_ptr<llvm::PassInstrumentationCallbacks> pass_instrumentation_callbacks;
        std::unique_ptr<llvm::StandardInstrumentations> standard_instrumentations;

    public:
        Optimizer(llvm::LLVMContext& context);
        ~Optimizer();

        void optimize(llvm::Function* function);

};

}