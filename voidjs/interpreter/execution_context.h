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
  ExecutionContext(JSHandle<types::LexicalEnvironment> lex_env,
                   JSHandle<types::LexicalEnvironment> var_env,
                   JSHandle<types::Object> obj)
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

  
  JSHandle<types::LexicalEnvironment> GetLexicalEnvironment() const {
    return lexical_environment_;
  }
  void SetLexicalEnvironment(JSHandle<types::LexicalEnvironment> lexical_env) {
    lexical_environment_ = lexical_env;
  }
  
  JSHandle<types::LexicalEnvironment> GetVariableEnvironment() const {
    return variable_environment_;
  }
  void SetVariabelEnvironment(JSHandle<types::LexicalEnvironment> variable_environment) {
    variable_environment_ = variable_environment;
  }

  JSHandle<types::Object> GetThisBinding() const { return this_binding_; }
  void SetThisBinding(JSHandle<types::Object> this_binding) { this_binding_ = this_binding; }

  static void EnterGlobalCode(VM* vm, ast::AstNode* ast_node);
  static void EnterEvalCode(VM* vm);
  static void EnterFunctionCode(
    VM* vm, ast::AstNode* ast_node, JSHandle<builtins::JSFunction> F, JSHandle<JSValue> this_arg, const std::vector<JSHandle<JSValue>>& args);
  static void DeclarationBindingInstantiation(VM* vm, ast::AstNode* ast_node, const std::vector<JSHandle<JSValue>>& args);
  
 private:
  // label set
  std::unordered_set<std::u16string_view> label_set_;  // contain "" initially
  std::u16string_view cur_label_;

  // iteration
  std::size_t iteration_depth_ {0};

  // switch 
  std::size_t switch_depth_ {0};

  // 
  JSHandle<types::LexicalEnvironment> lexical_environment_;
  JSHandle<types::LexicalEnvironment> variable_environment_;
  JSHandle<types::Object> this_binding_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
