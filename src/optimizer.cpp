#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <memory>

#include "optimizer.hpp"

namespace opt{

Optimizer::~Optimizer() = default;

Optimizer::Optimizer(llvm::LLVMContext& context){
    function_pass_manager = std::make_unique<llvm::FunctionPassManager>();
    loop_analysis_manager = std::make_unique<llvm::LoopAnalysisManager>();
    function_analysis_manager = std::make_unique<llvm::FunctionAnalysisManager>();
    CGSCC_analysis_manager = std::make_unique<llvm::CGSCCAnalysisManager>();
    module_analysis_manager = std::make_unique<llvm::ModuleAnalysisManager>();

    pass_instrumentation_callbacks = std::make_unique<llvm::PassInstrumentationCallbacks>();
    standard_instrumentations = std::make_unique<llvm::StandardInstrumentations>(context, 
        true /*debugging log*/
    );

    standard_instrumentations->registerCallbacks(
        *pass_instrumentation_callbacks,
        module_analysis_manager.get()
    );

}
}