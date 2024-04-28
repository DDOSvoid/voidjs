#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/execution_context.h"
#include "voidjs/interpreter/string_table.h"

namespace voidjs {

class VM {
 public:
  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  void PushExecutionContext(ExecutionContext* ctx) { execution_ctxs_.push_back(ctx); }
  void PopExecutionContext() { execution_ctxs_.pop_back(); }
  
  types::LexicalEnvironment* GetGlobalEnv() const { return global_env_; }
  void SetGlobalEnv(types::LexicalEnvironment* env) { global_env_ = env; }
  
  builtins::GlobalObject* GetGlobalObject() const { return global_obj_; }
  void SetGlobalObject(builtins::GlobalObject* obj) { global_obj_ = obj; }

  StringTable* GetStringTable() const { return string_table_; }
  void SetStringTable(StringTable* string_table) { string_table_ = string_table; }
  
 private:
  builtins::GlobalObject* global_obj_;
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;
  StringTable* string_table_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
