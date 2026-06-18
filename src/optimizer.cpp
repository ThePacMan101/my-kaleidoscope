#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <memory>

#include "optimizer.hpp"

namespace opt{

struct Optimizer::Impl {
    std::unique_ptr<llvm::ModuleAnalysisManager> module_analysis_manager;
    std::unique_ptr<llvm::CGSCCAnalysisManager> CGSCC_analysis_manager; // (C)all (G)raph (S)trongly (C)onnected (C)omponent
    std::unique_ptr<llvm::FunctionAnalysisManager> function_analysis_manager;
    std::unique_ptr<llvm::LoopAnalysisManager> loop_analysis_manager;


    // We only run passes in functions


    std::unique_ptr<llvm::PassBuilder> pass_builder;
    std::unique_ptr<llvm::FunctionPassManager> function_pass_manager;


    std::unique_ptr<llvm::PassInstrumentationCallbacks> pass_instrumentation_callbacks;
    std::unique_ptr<llvm::StandardInstrumentations> standard_instrumentations;
};


Optimizer::~Optimizer() = default;

Optimizer::Optimizer(llvm::LLVMContext& context) : pimpl {std::make_unique<Impl>()}
{
    pimpl->function_pass_manager = std::make_unique<llvm::FunctionPassManager>();
    pimpl->loop_analysis_manager = std::make_unique<llvm::LoopAnalysisManager>();
    pimpl->function_analysis_manager = std::make_unique<llvm::FunctionAnalysisManager>(); 
    pimpl->CGSCC_analysis_manager = std::make_unique<llvm::CGSCCAnalysisManager>(); 
    pimpl->module_analysis_manager = std::make_unique<llvm::ModuleAnalysisManager>();

   
    pimpl->pass_instrumentation_callbacks = std::make_unique<llvm::PassInstrumentationCallbacks>();
    pimpl->standard_instrumentations = std::make_unique<llvm::StandardInstrumentations>(context, 
        true /*debugging log*/
    );

    pimpl->standard_instrumentations->registerCallbacks(
        *pimpl->pass_instrumentation_callbacks,
        pimpl->module_analysis_manager.get()
    );

    pimpl->pass_builder = std::make_unique<llvm::PassBuilder>();

    pimpl->pass_builder.get()->registerModuleAnalyses(*pimpl->module_analysis_manager);
    pimpl->pass_builder.get()->registerFunctionAnalyses(*pimpl->function_analysis_manager);
    pimpl->pass_builder.get()->crossRegisterProxies(
        *pimpl->loop_analysis_manager,
        *pimpl->function_analysis_manager,
        *pimpl->CGSCC_analysis_manager,
        *pimpl->module_analysis_manager
    );

    // Transform passes:

    // peephole optimizations
    pimpl->function_pass_manager->addPass(llvm::InstCombinePass());
    // reassociate expressions
    pimpl->function_pass_manager->addPass(llvm::ReassociatePass());
    // eliminate common subexpressions (Global Value Numbering)
    pimpl->function_pass_manager->addPass(llvm::GVNPass());
    // simplify the control flow graph
    pimpl->function_pass_manager->addPass(llvm::SimplifyCFGPass());
}

void Optimizer::optimize(llvm::Function* function){
    pimpl->function_pass_manager->run(*function,*pimpl->function_analysis_manager);
}

}