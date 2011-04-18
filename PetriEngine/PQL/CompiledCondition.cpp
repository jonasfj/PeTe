#include "CompiledCondition.h"
#include "Contexts.h"

#include <assert.h>
#include <string>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/IRBuilder.h>

//#include <iostream>

using namespace llvm;
using namespace std;

namespace PetriEngine{
namespace PQL{

CompiledCondition::~CompiledCondition(){
	if(_cond)
		delete _cond;
	_cond = NULL;
	if(_engine)
		delete _engine;
	_engine = NULL;
}

void CompiledCondition::analyze(AnalysisContext &context){
	_cond->analyze(context);
}

bool CompiledCondition::compile(){
	InitializeNativeTarget();
	// Assume we're on main thread...
	LLVMContext &context = getGlobalContext();

	// Initialize module
	Module* module = new Module("Compiled function", context);

	// Create exection engine
	ExecutionEngine* engine = EngineBuilder(module).create();

	/********** Generate code **********/

	//Get a type for representing an integer pointer
	//Maybe this should be unsigned integer pointer type...
	PointerType* integerPointerType = PointerType::get(IntegerType::get(module->getContext(), 32), 0);

	//Create function type, for our function, int*, int* -> bool
	vector<const Type*> paramType;
	paramType.push_back(integerPointerType);
	paramType.push_back(integerPointerType);
	FunctionType* functionType = FunctionType::get(IntegerType::get(module->getContext(), 8), paramType, false);

	//Declare new function
	Function* function = Function::Create(functionType, GlobalValue::ExternalLinkage, "evaluate", module);
	//Use C calling convention
	function->setCallingConv(CallingConv::C);	//TODO: Read documentation and reconsider this

	//Get arguments from function
	Function::arg_iterator args = function->arg_begin();
	Value* marking = args++;
	Value* valuation = args++;
	marking->setName("marking");
	valuation->setName("valuation");

	//Create function block
	BasicBlock* functionBlock = BasicBlock::Create(module->getContext(), "functionBlock", function, 0);

	//Generate code
	CodeGenerationContext codeGenContext(marking, valuation, functionBlock, context);
	Value* result = _cond->codegen(codeGenContext);

	//Zero extend the result, e.g. make it a 8 bit bool
	CastInst* retval = new ZExtInst(result, IntegerType::get(module->getContext(), 8), "retval", functionBlock);

	//Create a return instruction
	ReturnInst::Create(module->getContext(), retval, functionBlock);

	/********** Optimize and Compile **********/

	// Create function pass manager, to optimize query
	FunctionPassManager optimizer(module);
	optimizer.add(new TargetData(*engine->getTargetData()));
	optimizer.add(createBasicAliasAnalysisPass());
	optimizer.add(createInstructionCombiningPass());
	optimizer.add(createReassociatePass());
	optimizer.add(createGVNPass());
	optimizer.add(createCFGSimplificationPass());
	optimizer.doInitialization();

	// Verify function, errors written to stderr
	if(verifyFunction(*function))
		return false;

	// Optimize function
	optimizer.run(*function);

	// Compile the function
	_nativeFunction = (bool(*)(const MarkVal*, const VarVal*))engine->getPointerToFunction(function);

	return _nativeFunction != NULL;
}

bool CompiledCondition::evaluate(const EvaluationContext &context) const{
	assert(_nativeFunction);
	return _nativeFunction(context.marking(), context.assignment());
}

Value* CompiledCondition::codegen(CodeGenerationContext &context) const{
	return  _cond->codegen(context);
}

double CompiledCondition::distance(DistanceContext& context) const{
	return _cond->distance(context);
}

string CompiledCondition::toString() const{
	return _cond->toString();
}

} // PQL
} // PetriEngine
