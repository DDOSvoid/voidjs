#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

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
  
 private:
  builtins::GlobalObject* global_obj_;
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;
  ObjectFactory* object_factory_; 
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
