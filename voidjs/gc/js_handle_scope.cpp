#include "voidjs/gc/js_handle.h"

#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

JSHandleScope::JSHandleScope(VM* vm)
  : vm_(vm) {
  prev_pos_ = vm_->handle_scope_current_block_pos_;
  prev_end_ = vm_->handle_scope_current_block_end_;
  prev_index_ = vm_->handle_scope_current_block_index_;
}

JSHandleScope::~JSHandleScope() {
  vm_->handle_scope_current_block_pos_ = prev_pos_;
  vm_->handle_scope_current_block_end_ = prev_end_;
  vm_->handle_scope_current_block_index_ = prev_index_;
}

std::uintptr_t JSHandleScope::NewHandle(VM* vm, JSValueType value) {
  auto addr = vm->handle_scope_current_block_pos_;
  if (addr == vm->handle_scope_current_block_end_) {
    addr = vm->ExpandHandleScopeBlock();
  }

  *addr = value;
  vm->handle_scope_current_block_pos_ = addr + 1; 
  return reinterpret_cast<std::uintptr_t>(addr);
}

}  // namespace voidjs
