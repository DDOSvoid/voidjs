#ifndef VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
#define VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H

#include <unordered_set>

#include "voidjs/ir/ast.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/lexical_environment.h"

namespace voidjs {

class JSHandleScope;

class ExecutionContext {
 public:
  ExecutionContext(types::LexicalEnvironment* lex_env,
                   types::LexicalEnvironment* var_env,
                   types::Object* obj)
    : lexical_environment_(lex_env),
      variable_environment_(var_env),
      this_binding_(obj) {
    label_set_.insert(u"");
  }
  
  bool HasLabel(std::u16string_view label) {
    return label_set_.find(label) != label_set_.end();
  }
  void AddLabel(std::u16string_view label) {
    cur_label_ = label;
    label_set_.insert(label);
  }
  void DeleteLabel(std::u16string_view label) {
    label_set_.erase(label);
  }
  
  bool InIteration() const { return iteration_depth_ > 0; }
  void EnterIteration() { ++iteration_depth_; }
  void ExitIteration() { --iteration_depth_; }

  bool InSwitch() const { return switch_depth_ > 0; }
  void EnterSwitch() { ++switch_depth_; }
  void ExitSwitch() { --switch_depth_;}

  JSValueType* ExpandHandleScopeBlock() {
    if (handle_scope_current_block_index_ + 1 == handle_scope_blocks_.size()) {
      auto block = new std::array<JSValueType, HANDLE_SCOPE_BLOCK_SIZE>{};
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
  
  types::LexicalEnvironment* GetLexicalEnvironment() const {
    return lexical_environment_;
  }
  void SetLexicalEnvironment(types::LexicalEnvironment* lexical_env) {
    lexical_environment_ = lexical_env;
  }
  
  types::LexicalEnvironment* GetVariableEnvironment() const {
    return variable_environment_;
  }
  void SetVariabelEnvironment(types::LexicalEnvironment* variable_environment) {
    variable_environment_ = variable_environment;
  }

  types::Object* GetThisBinding() const { return this_binding_; }
  void SetThisBinding(types::Object* this_binding) { this_binding_ = this_binding; }

  static void EnterGlobalCode(VM* vm, ast::AstNode* ast_node);
  static void EnterEvalCode(VM* vm);
  static void EnterFunctionCode(
    VM* vm, ast::AstNode* ast_node, builtins::JSFunction* F, JSValue this_arg, const std::vector<JSValue>& args);
  static void DeclarationBindingInstantiation(VM* vm, ast::AstNode* ast_node, const std::vector<JSValue>& args);
  
 private:
  // label set
  std::unordered_set<std::u16string_view> label_set_;  // contain "" initially
  std::u16string_view cur_label_;

  // iteration
  std::size_t iteration_depth_ {0};

  // switch 
  std::size_t switch_depth_ {0};

  // handle scope
  static constexpr std::size_t HANDLE_SCOPE_BLOCK_SIZE = 10 * 1024;
  std::vector<std::array<JSValueType, HANDLE_SCOPE_BLOCK_SIZE>> handle_scope_blocks_;
  JSValueType* handle_scope_current_block_pos_ {nullptr};
  JSValueType* handle_scope_current_block_end_ {nullptr};
  std::int32_t handle_scope_current_block_index_ {-1};

  // 
  types::LexicalEnvironment* lexical_environment_ {nullptr};
  types::LexicalEnvironment* variable_environment_ {nullptr};
  types::Object* this_binding_ {nullptr};

  friend JSHandleScope;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
