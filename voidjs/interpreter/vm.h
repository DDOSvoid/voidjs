#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

class StringTable;
class ObjectFactory;

class VM {
 public:
  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  void PushExecutionContext(ExecutionContext* ctx) { execution_ctxs_.push_back(ctx); }
  void PopExecutionContext() { execution_ctxs_.pop_back(); }
  
  types::LexicalEnvironment* GetGlobalEnv() const { return global_env_; }
  void SetGlobalEnv(types::LexicalEnvironment* env) { global_env_ = env; }
  
  builtins::GlobalObject* GetGlobalObject() const { return global_obj_; }
  void SetGlobalObject(builtins::GlobalObject* obj) { global_obj_ = obj; }

  ObjectFactory* GetObjectFactory() const { return object_factory_; }
  void SetObjectFactory(ObjectFactory* object_factory) { object_factory_ = object_factory; }

  builtins::JSObject* GetObjectPrototype() const { return object_proto_; }
  void SetObjectPrototype(builtins::JSObject* object_proto) { object_proto_ = object_proto; }

  builtins::JSObject* GetObjectConstructor() const { return object_ctor_; }
  void SetObjectConstructor(builtins::JSObject* object_ctor) { object_ctor_ = object_ctor; }

 private:
  builtins::GlobalObject* global_obj_;
  builtins::JSObject* object_proto_;
  builtins::JSObject* object_ctor_;
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;
  ObjectFactory* object_factory_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
