#ifndef VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
#define VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H

#include <unordered_set>

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/lexical_environment.h"

namespace voidjs {

class ExecutionContext {
 public:
  void AddLabel(const std::u16string& label) {
    cur_label_ = label;
    label_set_.insert(label);
  }

  bool HasLabel(const std::u16string& label) {
    return label_set_.find(label) != label_set_.end();
  }

  void DeleteLabel(const std::u16string& label) {
    label_set_.erase(label);
  }

  types::LexicalEnvironment* GetLexicalEnvironment() const {
    return lexical_environment_;
  }
  
 private:
  std::unordered_set<std::u16string> label_set_;  // contain "" initially
  std::u16string_view cur_label_;
  
  types::LexicalEnvironment* lexical_environment_;
  types::LexicalEnvironment* variable_environment_;
  JSValue this_binding_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_EXECUTION_CONTEXT_H
