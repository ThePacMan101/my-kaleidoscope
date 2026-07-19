#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "codegen.hpp"
#include "logger.hpp"

namespace ast {

/*
Receives a node, accepts it (i.e. generates code for the node) then returns the current_value. 

This is used in order to keep the code readable while maintaining the accept method's return type as void
*/
#define codegen(node) [&]() -> llvm::Value*{ \
    (node)->accept(*this);  \
    return current_value;   \
}()

CodeGenerator::~CodeGenerator() = default;

CodeGenerator::CodeGenerator(
    llvm::LLVMContext& context,
    llvm::Module& module,
    const std::unordered_map<std::string, std::unique_ptr<Prototype>>& function_prototypes):
        context {context}, module {module}, function_prototypes {function_prototypes} {
    builder = std::make_unique<llvm::IRBuilder<>>(context);
}

void CodeGenerator::visit(NumberExpr& node){
    current_value = llvm::ConstantFP::get(context,llvm::APFloat(node.get_val()));
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
    if(!left){
        current_value = nullptr;
        return;
    } 
    
    auto op = node.get_op();

    llvm::Value* right = codegen(node.get_right()); 
    if(!right){
        current_value = nullptr;
        return;
    }
    
    // whenever I refer to Float here, assume arbitrary precision 
    switch (op){
        case '>' :{
            // (F)loating (Cmp)are (G)reater (T)hen
            left = builder->CreateFCmpUGT(left,right,"cmptmp");
            // (U)nsigned (I)nteger (To) (F)loating (P)oint
            current_value = builder->CreateUIToFP(
                left,
                llvm::Type::getDoubleTy(context),
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
                llvm::Type::getDoubleTy(context),
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
    
    llvm::Function* callee_IR = nullptr;
    callee_IR = module.getFunction(callee);

    // If the IR for the function is in another module, I need to generate it again
    if(!callee_IR){
        auto fi = function_prototypes.find(callee);
        if(fi != function_prototypes.end()){
            auto prototype = fi->second.get();
            callee_IR = static_cast<llvm::Function*>(codegen(prototype)); 
        }
    } 
    // If I still couldn't find the function, I don't know it.
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
void CodeGenerator::visit(IfExpr& node){
    auto cond  = node.get_condition();
    auto then  = node.get_then();
    auto else_ = node.get_else();

    llvm::Value* cond_IR = codegen(cond);
    if(!cond_IR){
        current_value = logger::error<llvm::Value*>("Invalid if condition");
        return;
    }

    // Compare Floats ONE: (O)rdered (N)ot (E)quals
    // APFloat: (A)rbitrary (P)recision Float
    cond_IR = builder->CreateFCmpONE(
        // (cond_IR) != (0.0f)
        cond_IR, llvm::ConstantFP::get(context,llvm::APFloat(0.0)),
        // Instruction label
        "ifcond" 
    );

    // grab the current function that is being generated
    llvm::Function* this_func = builder->GetInsertBlock()->getParent();

    // create a then block and attach to the end of the function
    llvm::BasicBlock* then_block = llvm::BasicBlock::Create(context,"then",this_func);
    
    // create the else and after blocks but doesn't attach them to the function yet
    llvm::BasicBlock* else_block = llvm::BasicBlock::Create(context,"else");
    llvm::BasicBlock* aftr_block  = llvm::BasicBlock::Create(context,"aftr");

    // creates a (Br)anch with a (Cond)ition targeting the then and else blocks
    builder->CreateCondBr(cond_IR,then_block,else_block);

    // next instructions will be appended at the end of then block
    builder->SetInsertPoint(then_block);

    // generates the body of then block
    llvm::Value* then_IR = codegen(then);
    if(!then_IR){
        current_value = logger::error<llvm::Value*>("Invalid then expression");
        return;
    }

    // "goto aftr" at the end of then block
    builder->CreateBr(aftr_block);

    // we need to make sure we have the latest then_block, because it could
    // have changed when we called codegen on then's.
    // changes can happen, for example, when then's body is also a if-else stmt.
    // We need an up to date value for then_block, since it will be used
    // when generating the phi node in the aftr block
    then_block = builder->GetInsertBlock();

    // now we insert the else block at the end of the function being generated
    this_func->insert(this_func->end(),else_block);
    // and next instructions will be appended at the end of else block
    builder->SetInsertPoint(else_block);

    // generates the body of the else block
    llvm::Value* else_IR = codegen(else_);
    if(!else_IR){
        current_value = logger::error<llvm::Value*>("Invalid else expression");
        return;
    }

    // "goto after" at the end of else block
    builder->CreateBr(aftr_block);
    
    // same as with then_block, we need to reset else_block after possible
    // changes made in codegen for else's body. This will be used later 
    // in the phi node.
    else_block = builder->GetInsertBlock();

    // now we generate the aftr block with the phi node!
    this_func->insert(this_func->end(),aftr_block);
    builder->SetInsertPoint(aftr_block);
    
    // phi_node is like a shrodinger's variable. It can be one thing or another
    // but with the same type, in the case of kaleidoscope, always a double type.
    // as this phi_node comes from two blocks, we need to specify that. 
    // And "iftmp" is the name of that variable in the resulting IR
    llvm::PHINode *phi_node = builder->CreatePHI(
        llvm::Type::getDoubleTy(context),
        2,
        "iftmp"
    );

    phi_node->addIncoming(then_IR,then_block);
    phi_node->addIncoming(else_IR,else_block);

    current_value = static_cast<llvm::Value*>(phi_node);
    return;
}
void CodeGenerator::visit(Prototype& node){
    auto& function_name = node.get_name();
    auto& args = node.get_args();
    // The only types in kaleidoscope are doubles, so all parameters are doubles!
    std::vector<llvm::Type*> params(
        args.size(),
        llvm::Type::getDoubleTy(context)
    );

    // Every function in kaleidoscope must return a double!
    llvm::FunctionType* function_t = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(context),
        params,
        false   // is_variadic_arg? (we don't have those in kaleidoscope!)
    );

    llvm::Function* function = llvm::Function::Create(
        function_t,
        llvm::Function::ExternalLinkage,
        function_name,
        &module
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
    llvm::Function* function = module.getFunction(prototype->get_name());

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
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context,"entry",function);
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