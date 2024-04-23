#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

class VM {
 public:
  VM(types::Object* obj, types::LexicalEnvironment* env)
    : global_obj_(obj), global_env_(env)
  {}

  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  void PushExecutionContext(ExecutionContext* ctx) { execution_ctxs_.push_back(ctx); }
  void PopExecutionContext() { execution_ctxs_.pop_back(); }
  
  types::LexicalEnvironment* GetGlobalEnv() const { return global_env_; }
  // void SetGlobalEnv(types::LexicalEnvironment* env) { global_env_ = env; }
  
  types::Object* GetGlobalObject() const { return global_obj_; }
  // void SetGlobalObject(types::Object* obj) { global_object_ = obj; }
  
 private:
  types::Object* global_obj_;
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
