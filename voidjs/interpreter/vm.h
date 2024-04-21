#ifndef VOIDJS_INTERPRETER_VM_H
#define VOIDJS_INTERPRETER_VM_H

#include <vector>

#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

class VM {
 public:
  ExecutionContext* GetExecutionContext() const { return execution_ctxs_.back(); }
  
 private:
  // missing global object
  types::LexicalEnvironment* global_env_;
  std::vector<ExecutionContext*> execution_ctxs_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_VM_H
