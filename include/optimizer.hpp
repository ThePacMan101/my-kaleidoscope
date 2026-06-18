#pragma once

#include <memory>

namespace llvm {
    class LLVMContext;
    class Function;
}

namespace opt {

class Optimizer{
    private:

        struct Impl;
        std::unique_ptr<Impl> pimpl;
        /*
        std::unique_ptr<llvm::ModuleAnalysisManager> module_analysis_manager;
        std::unique_ptr<llvm::CGSCCAnalysisManager> CGSCC_analysis_manager; // (C)all (G)raph (S)trongly (C)onnected (C)omponent
        std::unique_ptr<llvm::FunctionAnalysisManager> function_analysis_manager;
        std::unique_ptr<llvm::LoopAnalysisManager> loop_analysis_manager;
        
        // We only run passes in functions

        std::unique_ptr<llvm::PassBuilder> pass_builder;
        std::unique_ptr<llvm::FunctionPassManager> function_pass_manager;

        std::unique_ptr<llvm::PassInstrumentationCallbacks> pass_instrumentation_callbacks;
        std::unique_ptr<llvm::StandardInstrumentations> standard_instrumentations;
        */


    public:
        Optimizer(llvm::LLVMContext& context);
        ~Optimizer();

        void optimize(llvm::Function* function);

};

}