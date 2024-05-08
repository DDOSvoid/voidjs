#include "voidjs/interpreter/vm.h"

#include "voidjs/types/object_factory.h"
#include "voidjs/interpreter/string_table.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/interpreter/global_constants.h"

namespace voidjs {

VM::VM(Interpreter* interpreter)
  : interpreter_{interpreter},
    object_factory_{new ObjectFactory{this, new Heap{this}, new StringTable{this}}},
    global_constants_{new GlobalConstants{this}}
{
  global_constants_->Initialize();
}

VM::~VM() {
  delete object_factory_;
  
  delete global_constants_;
}

JSValue* VM::ExpandHandleScopeBlock() {
  if (handle_scope_current_block_index_ + 1 == handle_scope_blocks_.size()) {
    auto block = new std::array<JSValue, HANDLE_SCOPE_BLOCK_SIZE>{};
    handle_scope_blocks_.push_back(*block);
    handle_scope_current_block_pos_ = block->data();
    handle_scope_current_block_end_ = block->data() + block->size();
    ++handle_scope_current_block_index_;
    return block->data();
  } else {
    auto block = &handle_scope_blocks_.back();
    handle_scope_current_block_pos_ = block->data();
    handle_scope_current_block_end_ = block->data() + block->size();
    ++handle_scope_current_block_index_;
    return block->data();
  }
}

std::vector<JSHandle<JSValue>> VM::GetRoots() {
  std::vector<JSHandle<JSValue>> handles;

  // Execution Contexts
  handles.reserve(3 * execution_ctxs_.size());
  for (auto ctx : execution_ctxs_) {
    handles.push_back(ctx->GetVariableEnvironment().As<JSValue>());
    handles.push_back(ctx->GetLexicalEnvironment().As<JSValue>());
    handles.push_back(ctx->GetThisBinding().As<JSValue>());
  }

  // HandelScope
  for (std::int32_t idx = 0; idx < handle_scope_current_block_index_; ++idx) {
    JSValue* limit = idx == handle_scope_current_block_index_ ?
      handle_scope_current_block_pos_ :
      handle_scope_blocks_[idx].data() + handle_scope_blocks_[idx].size();
    for (JSValue* start = handle_scope_blocks_[idx].data(); start < limit; ++start) {
      handles.push_back(JSHandle<JSValue>{reinterpret_cast<std::uintptr_t>(start)});
    }
  }

  return handles;
}

}  // namespace voidjs
