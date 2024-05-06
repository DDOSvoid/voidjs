#include "voidjs/gc/js_handle.h"

#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

JSHandleScope::JSHandleScope(VM* vm)
  : vm_(vm) {
  auto context = vm->GetExecutionContext();
  
  prev_pos_ = context->handle_scope_current_block_pos_;
  prev_end_ = context->handle_scope_current_block_end_;
  prev_index_ = context->handle_scope_current_block_index_;
}

JSHandleScope::~JSHandleScope() {
  auto context = vm_->GetExecutionContext();

  context->handle_scope_current_block_pos_ = prev_pos_;
  context->handle_scope_current_block_end_ = prev_end_;
  context->handle_scope_current_block_index_ = prev_index_;
}

std::uintptr_t JSHandleScope::NewHandle(VM* vm, JSValueType value) {
  auto context = vm->GetExecutionContext();
  
  auto addr = context->handle_scope_current_block_pos_;
  if (addr == context->handle_scope_current_block_end_) {
    addr = context->ExpandHandleScopeBlock();
  }

  *addr = value;
  context->handle_scope_current_block_pos_ = addr + 1; 
  return reinterpret_cast<std::uintptr_t>(addr);
}

}  // namespace voidjs
