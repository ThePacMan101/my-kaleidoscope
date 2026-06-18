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

    public:
        Optimizer(llvm::LLVMContext& context);
        ~Optimizer();

        void optimize(llvm::Function* function);

};

}