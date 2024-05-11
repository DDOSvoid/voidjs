#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/interpreter/execution_context.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

class StringTable;
class ObjectFactory;
class Interpreter;
class GlobalConstants;

class VM {
 public:
  VM(Interpreter* interpreter);

  ~VM();

  Interpreter* GetInterpreter() const { return interpreter_; }
  
  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  void PushExecutionContext(ExecutionContext* ctx) { execution_ctxs_.push_back(ctx); }
  void PopExecutionContext() { execution_ctxs_.pop_back(); }
  
  PROPERTY_ACCESSORS(JSHandle<types::LexicalEnvironment>, GlobalEnv, global_env_)
  PROPERTY_ACCESSORS(JSHandle<builtins::GlobalObject>, GlobalObject, global_obj_)
  
  PROPERTY_ACCESSORS(ObjectFactory*, ObjectFactory, object_factory_)

  PROPERTY_ACCESSORS(GlobalConstants*, GlobalConstants, global_constants_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSObject>, ObjectPrototype, object_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, ObjectConstructor, object_ctor_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, FunctionPrototype, function_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, FunctionConstructor, function_ctor_)

  PROPERTY_ACCESSORS(JSHandle<builtins::JSArray>, ArrayPrototype, array_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, ArrayConstructor, array_ctor_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSString>, StringPrototype, string_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, StringConstructor, string_ctor_)

  PROPERTY_ACCESSORS(JSHandle<builtins::JSBoolean>, BooleanPrototype, boolean_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, BooleanConstructor, boolean_ctor_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSNumber>, NumberPrototype, number_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, NumberConstructor, number_ctor_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSMath>, MathObject, math_obj_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, ErrorPrototype, error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, ErrorConstructor, error_ctor_)
  
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, EvalErrorPrototype, eval_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, EvalErrorConstructor, eval_error_ctor_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, RangeErrorPrototype, range_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, RangeErrorConstructor, range_error_ctor_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, ReferenceErrorPrototype, reference_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, ReferenceErrorConstructor, reference_error_ctor_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, SyntaxErrorPrototype, syntax_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, SyntaxErrorConstructor, syntax_error_ctor_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, TypeErrorPrototype, type_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, TypeErrorConstructor, type_error_ctor_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, URIErrorPrototype, uri_error_proto_)
  PROPERTY_ACCESSORS(JSHandle<builtins::JSFunction>, URIErrorConstructor, uri_error_ctor_)

  PROPERTY_ACCESSORS(JSHandle<builtins::JSError>, Exception, exception_)
  bool HasException() const { return !exception_.IsEmpty(); }
  void ClearException() { exception_ = JSHandle<builtins::JSError>{}; }

  JSValue* ExpandHandleScopeBlock();

  std::vector<JSHandle<JSValue>> GetRoots();

 private:
  friend class JSHandleScope;

 private:
  // standard builtin objects
  JSHandle<builtins::JSObject> object_proto_;
  JSHandle<builtins::JSFunction> object_ctor_;
  
  JSHandle<builtins::JSFunction> function_proto_;
  JSHandle<builtins::JSFunction> function_ctor_;

  JSHandle<builtins::JSArray> array_proto_;
  JSHandle<builtins::JSFunction> array_ctor_;

  JSHandle<builtins::JSString> string_proto_;
  JSHandle<builtins::JSFunction> string_ctor_;

  JSHandle<builtins::JSBoolean> boolean_proto_;
  JSHandle<builtins::JSFunction> boolean_ctor_;

  JSHandle<builtins::JSNumber> number_proto_;
  JSHandle<builtins::JSFunction> number_ctor_;

  JSHandle<builtins::JSMath> math_obj_;
  
  JSHandle<builtins::JSError> error_proto_;
  JSHandle<builtins::JSFunction> error_ctor_;
  
  JSHandle<builtins::JSError> eval_error_proto_;
  JSHandle<builtins::JSFunction> eval_error_ctor_;

  JSHandle<builtins::JSError> range_error_proto_;
  JSHandle<builtins::JSFunction> range_error_ctor_;

  JSHandle<builtins::JSError> reference_error_proto_;
  JSHandle<builtins::JSFunction> reference_error_ctor_;
  
  JSHandle<builtins::JSError> syntax_error_proto_;
  JSHandle<builtins::JSFunction> syntax_error_ctor_;

  JSHandle<builtins::JSError> type_error_proto_;
  JSHandle<builtins::JSFunction> type_error_ctor_;

  JSHandle<builtins::JSError> uri_error_proto_;
  JSHandle<builtins::JSFunction> uri_error_ctor_;

  // 
  JSHandle<builtins::GlobalObject> global_obj_;
  JSHandle<types::LexicalEnvironment> global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;

  //  
  ObjectFactory* object_factory_;

  //
  GlobalConstants* global_constants_;
  
  // handle scope
  static constexpr std::size_t HANDLE_SCOPE_BLOCK_SIZE = 10 * 1024;
  std::vector<std::array<JSValue, HANDLE_SCOPE_BLOCK_SIZE>> handle_scope_blocks_;
  JSValue* handle_scope_current_block_pos_ {nullptr};
  JSValue* handle_scope_current_block_end_ {nullptr};
  std::int32_t handle_scope_current_block_index_ {-1};

  //
  JSHandle<builtins::JSError> exception_;

  // 
  Interpreter* interpreter_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
