#ifndef VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
#define VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H

#include <unordered_set>

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/lexical_environment.h"

namespace voidjs {

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

  bool InSwitch() const { return switch_depth_; }
  void EnterSwitch() { ++switch_depth_; }
  void ExitSwitch() { --switch_depth_;}
  
  types::LexicalEnvironment* GetLexicalEnvironment() const {
    return lexical_environment_;
  }
  types::LexicalEnvironment* GetVariableEnvironment() const {
    return variable_environment_;
  }
  
 private:
  std::unordered_set<std::u16string_view> label_set_;  // contain "" initially
  std::u16string_view cur_label_;

  std::size_t iteration_depth_;

  std::size_t switch_depth_;
  
  types::LexicalEnvironment* lexical_environment_;
  types::LexicalEnvironment* variable_environment_;
  types::Object* this_binding_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
