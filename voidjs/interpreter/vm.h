#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/interpreter/execution_context.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

class StringTable;
class ObjectFactory;
class Interpreter;

class VM {
 public:
  VM(Interpreter* interpreter)
    : interpreter_{interpreter}
  {}

  Interpreter* GetInterpreter() const { return interpreter_; }
  
  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  void PushExecutionContext(ExecutionContext* ctx) { execution_ctxs_.push_back(ctx); }
  void PopExecutionContext() { execution_ctxs_.pop_back(); }
  
  PROPERTY_ACCESSORS(types::LexicalEnvironment*, GlobalEnv, global_env_)
  PROPERTY_ACCESSORS(builtins::GlobalObject*, GlobalObject, global_obj_)
  PROPERTY_ACCESSORS(ObjectFactory*, ObjectFactory, object_factory_)
  
  PROPERTY_ACCESSORS(builtins::JSObject*, ObjectPrototype, object_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, ObjectConstructor, object_ctor_)
  
  PROPERTY_ACCESSORS(builtins::JSFunction*, FunctionPrototype, function_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, FunctionConstructor, function_ctor_)
  
  PROPERTY_ACCESSORS(builtins::JSError*, ErrorPrototype, error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, ErrorConstructor, error_ctor_)
  
  PROPERTY_ACCESSORS(builtins::JSError*, EvalErrorPrototype, eval_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, EvalErrorConstructor, eval_error_ctor_)
  PROPERTY_ACCESSORS(builtins::JSError*, RangeErrorPrototype, range_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, RangeErrorConstructor, range_error_ctor_)
  PROPERTY_ACCESSORS(builtins::JSError*, ReferenceErrorPrototype, reference_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, ReferenceErrorConstructor, reference_error_ctor_)
  PROPERTY_ACCESSORS(builtins::JSError*, SyntaxErrorPrototype, syntax_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, SyntaxErrorConstructor, syntax_error_ctor_)
  PROPERTY_ACCESSORS(builtins::JSError*, TypeErrorPrototype, type_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, TypeErrorConstructor, type_error_ctor_)
  PROPERTY_ACCESSORS(builtins::JSError*, URIErrorPrototype, uri_error_proto_)
  PROPERTY_ACCESSORS(builtins::JSFunction*, URIErrorConstructor, uri_error_ctor_)

  PROPERTY_ACCESSORS(builtins::JSError*, Exception, exception_)
  bool HasException() const { return exception_; }
  void ClearException() { exception_ = nullptr; }

 private:
  // standard builtin objects
  builtins::JSObject* object_proto_;
  builtins::JSFunction* object_ctor_;
  
  builtins::JSFunction* function_proto_;
  builtins::JSFunction* function_ctor_;
  
  builtins::JSError* error_proto_;
  builtins::JSFunction* error_ctor_;
  
  builtins::JSError* eval_error_proto_;
  builtins::JSFunction* eval_error_ctor_;

  builtins::JSError* range_error_proto_;
  builtins::JSFunction* range_error_ctor_;

  builtins::JSError* reference_error_proto_;
  builtins::JSFunction* reference_error_ctor_;
  
  builtins::JSError* syntax_error_proto_;
  builtins::JSFunction* syntax_error_ctor_;

  builtins::JSError* type_error_proto_;
  builtins::JSFunction* type_error_ctor_;

  builtins::JSError* uri_error_proto_;
  builtins::JSFunction* uri_error_ctor_;

  // 
  builtins::GlobalObject* global_obj_;
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;

  //  
  ObjectFactory* object_factory_;

  //
  builtins::JSError* exception_ {nullptr};

  Interpreter* interpreter_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
