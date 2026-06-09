#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "codegen.hpp"
#include "logger.hpp"

namespace ast {

/*
Receives a node, accepts it (i.e. generates code for the node) then returns the current_value. 

This is used in order to keep the code readable while maintaining the accept method  return type as void
*/
#define codegen(node) [&]() -> llvm::Value*{ \
    (node)->accept(*this);  \
    return current_value;   \
}()

CodeGenerator::~CodeGenerator() = default;

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module  = std::make_unique<llvm::Module>("Kaleidoscop",*context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context); 
}

void CodeGenerator::visit(NumberExpr& node){
    current_value = llvm::ConstantFP::get(*context,llvm::APFloat(node.get_val()));
}

void CodeGenerator::visit(VariableExpr& node){
    llvm::Value* val = named_values[node.get_name()];
    if(val){
        current_value = val; 
    }else{
        current_value = logger::error<llvm::Value*>("Unknown variable name");
    }
}
void CodeGenerator::visit(BinaryExpr& node){
    llvm::Value* left = codegen(node.get_left()); 
    
    auto op = node.get_op();

    llvm::Value* right = codegen(node.get_right()); 
    
    // whenever I refer to Float here, assume arbitrary precision 
    switch (op){
        case '>' :{
            // (F)loating (Cmp)are (G)reater (T)hen
            left = builder->CreateFCmpUGT(left,right,"cmptmp");
            // (U)nsigned (I)nteger (To) (F)loating (P)oint
            current_value = builder->CreateUIToFP(
                left,
                llvm::Type::getDoubleTy(*context),
                "booltmp"
            );
            break;
        }
        case '<' :{
            // (F)loating (Cmp)are (L)esser (T)hen
            left = builder->CreateFCmpULT(left,right,"cmptmp");
            // (U)nsigned (I)nteger to (F)loating (P)oint
            current_value = builder->CreateUIToFP(
                left,
                llvm::Type::getDoubleTy(*context),
                "booltmp"
            );
            break;
        }
        case '+' :{
            current_value = builder->CreateFAdd(left,right,"addtmp");
            break;
        }
        case '-' :{
            current_value = builder->CreateFSub(left,right,"subtmp");
            break;
        }
        case '*' :{
            current_value = builder->CreateFMul(left,right,"multmp");
            break;
        }
        case '/' :{
            current_value = builder->CreateFDiv(left,right,"divtmp");
            break;
        }
        
        default:{
            current_value = logger::error<llvm::Value*>("Invalid binary operator");
        }
    }
}
void CodeGenerator::visit(CallExpr& node){
    auto& callee = node.get_callee();
    auto& args = node.get_args();
    
    llvm::Function* callee_IR = module->getFunction(callee);
    if(!callee_IR){
        current_value = logger::error<llvm::Value*>("Unknown function referenced");
        return;
    } 
    
    if(callee_IR->arg_size() != args.size()){
        current_value = logger::error<llvm::Value*>("Incorrect number of arguments for function "+callee);
        return;
    }
    std::vector<llvm::Value*> args_IR;
    for(unsigned i = 0, e = args.size(); i != e ; ++i){
        args_IR.push_back(codegen(args[i]));
        if(!args_IR.back()){
            current_value = nullptr;
            return;
        }
    }
    current_value = builder->CreateCall(callee_IR,args_IR,"calltmp");
    return;
}
void CodeGenerator::visit(Prototype& node){
    auto& function_name = node.get_name();
    auto& args = node.get_args();
    // The only types in kaleidoscope are doubles, so all parameters are doubles!
    std::vector<llvm::Type*> params(
        args.size(),
        llvm::Type::getDoubleTy(*context)
    );

    // Every function in kaleidoscope must return a double!
    llvm::FunctionType* function_t = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*context),
        params,
        false   // is_variadic_arg? (we don't have those in kaleidoscope!)
    );

    llvm::Function* function = llvm::Function::Create(
        function_t,
        llvm::Function::ExternalLinkage,
        function_name,
        module.get()
    );

    unsigned index = 0;
    for(auto& arg: function->args()){
        arg.setName(args[index++]);
    }

    current_value = function;
}
void CodeGenerator::visit(Function& node){
    ast::Prototype* prototype = node.get_prototype();
    ast::Expr* body = node.get_body(); 

    // First try to find the function from a extern declaration
    llvm::Function* function = module->getFunction(prototype->get_name());

    // If we don't find an extern declaration, then this must be a function definition
    // now we generate the prototype 
    if(!function)
        function = static_cast<llvm::Function*>(codegen(prototype));

    // codegen failed for some reason....
    if(!function){
        current_value = nullptr;
        return;
    }

    // We are at a function definition, if it's already defined (i.e. it's body is nonempty)
    // we are trying to do a function redefinition, which is not allowed
    if(!function->empty()){
        current_value = logger::error<llvm::Function*>("Functions cannot be redefined");
        return;
    }

    // Now we must generate the function body
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context,"entry",function);
    builder->SetInsertPoint(block);

    // erase previous namespace and populate it if current function's param names
    // my namespace is always just composed of the params of the function
    named_values.clear();
    for(auto& arg: function->args()){
        named_values[std::string(arg.getName())] = &arg;
    }

    // in kaleidoscope, every function's body is a return 
    // statement for the evaluation of a expression
    // e.g.
    // def add(x y) x + y ;
    // so if I call add(1,2), it evaluates to 3
    if(llvm::Value* return_value = codegen(body)){
        builder->CreateRet(return_value);

        // validade code??
        llvm::verifyFunction(*function);

        current_value = function;
        return;
    }
    // If I get here there was a error reading 
    // the body, so we delete the function
    function->eraseFromParent();
    current_value = nullptr;
}

#undef codegen

}